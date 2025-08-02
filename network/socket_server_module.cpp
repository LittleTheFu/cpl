#include "socket_server_module.h"
#include <iostream>
#include <thread>
#include <string>
#include <limits>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <asio/streambuf.hpp> // 确保包含

// 包含您的编译器组件头文件
#include "parser.h"
#include "virtualMachine.h"
#include "vmCodeGenerator.h"

// 辅助函数：转义字符串中的双引号和反斜杠，使其可以在JSON字符串中安全嵌入
std::string escape_json_string(const std::string& input) {
    std::string output;
    output.reserve(input.length() * 2); // Pre-allocate for efficiency
    for (char c : input) {
        if (c == '"' || c == '\\') {
            output += '\\';
        }
        output += c;
    }
    return output;
}


// 静态成员的定义
std::unique_ptr<asio::io_context> SocketServer::io_context_ = nullptr;
std::unique_ptr<asio::ip::tcp::acceptor> SocketServer::acceptor_ = nullptr;
std::unique_ptr<std::thread> SocketServer::io_thread_ = nullptr;
std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> SocketServer::work_guard_ = nullptr;
SocketServer::RequestHandler SocketServer::request_handler_ = nullptr;
Parser* SocketServer::s_parser_ = nullptr;
VmCodeGenerator* SocketServer::s_code_gen_ = nullptr;
VirtualMachine* SocketServer::s_vm_ = nullptr;


// 启动接受新连接的异步操作
void SocketServer::start_accept() {
    auto socket = std::make_shared<asio::ip::tcp::socket>(*io_context_);
    
    std::cout << "[Server Debug] Initiating async_accept for new connection." << std::endl;
    acceptor_->async_accept(*socket,
        [socket](const asio::error_code& error) {
            if (!error) {
                std::cout << "Server: Client connected from: " << socket->remote_endpoint().address().to_string() << ":" << socket->remote_endpoint().port() << std::endl;
                std::cout << "[Server Debug] Calling handle_client for new connection." << std::endl;
                SocketServer::handle_client(socket);
            } else {
                std::cerr << "Server: Accept error: " << error.message() << std::endl;
            }
            if (acceptor_ && acceptor_->is_open()) {
                SocketServer::start_accept(); // Continue listening for next connection
            } else {
                std::cout << "[Server Debug] Acceptor is closed, not restarting accept operation." << std::endl;
            }
        });
}

// 处理客户端连接的入口函数
void SocketServer::handle_client(std::shared_ptr<asio::ip::tcp::socket> socket) {
    std::cout << "[Server Debug] handle_client entered for socket: " << socket->remote_endpoint().address().to_string() << std::endl;
    auto read_buffer = std::make_shared<asio::streambuf>();
    SocketServer::do_read(socket, read_buffer);
}

// 辅助函数：封装了从客户端读取消息的逻辑
void SocketServer::do_read(std::shared_ptr<asio::ip::tcp::socket> socket,
                           std::shared_ptr<asio::streambuf> read_buffer) {
    
    read_buffer->consume(read_buffer->size()); // Clear the buffer before each read
    std::cout << "[Server Debug] Starting async_read_until for client: " << socket->remote_endpoint().address().to_string() << std::endl;

    asio::async_read_until(*socket, *read_buffer, '\n',
        [socket, read_buffer](const asio::error_code& error, std::size_t bytes_transferred) {
            std::cout << "[Server Debug] async_read_until callback triggered. Bytes transferred: " << bytes_transferred << std::endl;
            
            if (!error) {
                std::cout << "[Server Debug] Read operation successful. Bytes read: " << bytes_transferred << std::endl;
                std::istream is(read_buffer.get());
                std::string request_str;
                std::getline(is, request_str); // Read until newline

                if (!request_str.empty() && request_str.back() == '\r') {
                    request_str.pop_back();
                }
                std::cout << "[Server Debug] Received request: [" << request_str << "]" << std::endl;

                // Process the request and generate response
                std::string response_str;
                if (request_handler_) {
                    // !!! IMPORTANT: The line below is where your actual compiler/VM logic will be called.
                    // For now, it might return a simple "hello" or "Received: ..." response.
                    response_str = request_handler_(request_str, *s_parser_, *s_code_gen_, *s_vm_);
                } else {
                    response_str = "{\"status\": \"error\", \"message\": \"No request handler set on server.\"}";
                }
                response_str += "\n"; // Ensure response ends with newline

                std::cout << "[Server Debug] Generated response: [" << response_str << "]" << std::endl;
                SocketServer::do_write(socket, read_buffer, response_str);

            } else if (error == asio::error::eof || error == asio::error::connection_reset || error == asio::error::operation_aborted) {
                std::cout << "Server: Client disconnected from " << socket->remote_endpoint().address().to_string() << ": " << error.message() << std::endl;
                if (socket->is_open()) {
                    asio::error_code ec;
                    socket->close(ec); 
                    std::cout << "[Server Debug] Socket closed due to client disconnect." << std::endl;
                }
            } else {
                std::cerr << "Server: Read error for client " << socket->remote_endpoint().address().to_string() << ": " << error.message() << std::endl;
                if (socket->is_open()) {
                    asio::error_code ec;
                    socket->close(ec); 
                    std::cout << "[Server Debug] Socket closed due to read error." << std::endl;
                }
            }
        });
}

// 辅助函数：封装了向客户端写入消息的逻辑 (同步版本)
void SocketServer::do_write(std::shared_ptr<asio::ip::tcp::socket> socket,
                            std::shared_ptr<asio::streambuf> read_buffer,
                            std::string response_str_to_send) { // 值传递字符串以确保生命周期
    
    std::cout << "[Server Debug] Starting SYNCHRONOUS write to client: " << socket->remote_endpoint().address().to_string() 
              << ", Bytes to send: " << response_str_to_send.length() << std::endl;

    asio::error_code write_error;
    std::size_t bytes_transferred_write = 0;
    try {
        bytes_transferred_write = asio::write(*socket, asio::buffer(response_str_to_send), write_error);
    } catch (const asio::system_error& e) {
        std::cerr << "Server: Synchronous write threw exception: " << e.what() << std::endl;
        write_error = e.code();
    }
    
    std::cout << "[Server Debug] Synchronous write attempt finished." << std::endl;
    if (write_error) {
        std::cerr << "Server: Synchronous write error: " << write_error.message() 
                  << " (Code: " << write_error.value() 
                  << ", Category: " << write_error.category().name() << ")" << std::endl;
        if (socket->is_open()) {
            asio::error_code ec_close;
            socket->close(ec_close); 
            std::cout << "[Server Debug] Socket closed due to synchronous write error." << std::endl;
        }
    } else {
        std::cout << "[Server Debug] Synchronous write successful. Wrote " << bytes_transferred_write << " bytes." << std::endl;
        if (socket->is_open()) {
            std::cout << "[Server Debug] Initiating next do_read after successful synchronous write." << std::endl;
            SocketServer::do_read(socket, read_buffer); 
        } else {
            std::cout << "[Server Debug] Socket closed after synchronous write, not initiating next read." << std::endl;
        }
    }
}


// 启动服务器的核心函数
void SocketServer::start(int port, RequestHandler handler, Parser& parser, VmCodeGenerator& code_gen, VirtualMachine& vm) {
    if (io_thread_ && io_thread_->joinable()) {
        std::cout << "Server is already running." << std::endl;
        return;
    }

    request_handler_ = handler;
    s_parser_ = &parser;
    s_code_gen_ = &code_gen;
    s_vm_ = &vm;

    try {
        io_context_ = std::make_unique<asio::io_context>();
        work_guard_ = std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(asio::make_work_guard(*io_context_));

        acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(*io_context_,
                                                               asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        std::cout << "Server listening on port " << port << std::endl;
        std::cout << "[Server Debug] io_context and acceptor initialized. Starting accept loop." << std::endl;

        SocketServer::start_accept();

        io_thread_ = std::make_unique<std::thread>([]() { 
            try {
                std::cout << "[Server Debug] IO context thread started. Running io_context." << std::endl;
                SocketServer::io_context_->run(); 
                std::cout << "[Server Debug] io_context run finished." << std::endl;
            } catch (const asio::system_error& e) {
                std::cerr << "Server: Asio system error in io_thread: " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Server: General exception in io_thread: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Server: Unknown exception caught in io_thread!" << std::endl;
            }
        });

    } catch (const asio::system_error& e) {
        std::cerr << "Server: Socket server startup error: " << e.what() << std::endl;
        if (work_guard_) work_guard_.reset();
        if (io_context_) io_context_->stop();
        if (io_thread_ && io_thread_->joinable()) io_thread_->join();
        io_context_ = nullptr;
        acceptor_ = nullptr;
        io_thread_ = nullptr;
        std::cout << "[Server Debug] Server startup failed, resources cleaned up." << std::endl;
    }
}

// 停止服务器
void SocketServer::stop() {
    std::cout << "Server: Stopping server..." << std::endl;
    if (work_guard_) {
        work_guard_.reset();
        std::cout << "[Server Debug] work_guard reset." << std::endl;
    }
    if (io_context_) {
        io_context_->stop();
        std::cout << "[Server Debug] io_context stopped." << std::endl;
    }
    if (acceptor_ && acceptor_->is_open()) {
        asio::error_code ec;
        acceptor_->close(ec);
        if (ec) {
            std::cerr << "Server: Error closing acceptor: " << ec.message() << std::endl;
        }
        std::cout << "[Server Debug] Acceptor closed." << std::endl;
    }
    if (io_thread_ && io_thread_->joinable()) {
        io_thread_->join();
        io_thread_.reset();
        std::cout << "[Server Debug] IO thread joined and reset." << std::endl;
    }

    io_context_.reset();
    acceptor_.reset();

    request_handler_ = nullptr;
    s_parser_ = nullptr;
    s_code_gen_ = nullptr;
    s_vm_ = nullptr;

    std::cout << "Server stopped." << std::endl;
    std::cout << "[Server Debug] All server resources released." << std::endl;
}