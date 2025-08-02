#include "token.h"
#include "socket_server_module.h"
#include <iostream>
#include <thread>   // 明确包含 <thread>
#include <string>
#include <limits> // For std::numeric_limits
// #include <nlohmann/json.hpp> // 假设你引入了C++ JSON库，如jsoncpp或nlohmann/json

// 包含你的编译器组件头文件
#include "parser.h"
#include "virtualMachine.h"
#include "vmCodeGenerator.h"

// 静态成员的定义 (确保这里定义了 work_guard_)
std::unique_ptr<asio::io_context> SocketServer::io_context_ = nullptr;
std::unique_ptr<asio::ip::tcp::acceptor> SocketServer::acceptor_ = nullptr;
std::unique_ptr<std::thread> SocketServer::io_thread_ = nullptr;
std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> SocketServer::work_guard_ = nullptr; // 初始化为 nullptr
SocketServer::RequestHandler SocketServer::request_handler_ = nullptr;
Parser* SocketServer::s_parser_ = nullptr;
VmCodeGenerator* SocketServer::s_code_gen_ = nullptr;
VirtualMachine* SocketServer::s_vm_ = nullptr;


// 启动接受新连接的异步操作
void SocketServer::start_accept() {
    // 为新的连接创建一个共享的 socket 对象
    auto socket = std::make_shared<asio::ip::tcp::socket>(*io_context_);
    
    // 发起异步接受操作
    acceptor_->async_accept(*socket,
        [socket](const asio::error_code& error) {
            if (!error) {
                // 如果没有错误，表示成功接受了一个连接
                std::cout << "Server: Client connected from: " << socket->remote_endpoint().address().to_string() << ":" << socket->remote_endpoint().port() << std::endl;
                // 调用 handle_client 来处理这个新连接
                SocketServer::handle_client(socket); // 调用静态成员函数
            } else {
                // 接受连接时发生错误
                std::cerr << "Server: Accept error: " << error.message() << std::endl;
            }
            // 无论接受是否成功，都继续监听下一个连接
            // 确保 acceptor_ 仍然有效且开放，以防止在 stop 期间报错
            if (acceptor_ && acceptor_->is_open()) {
                SocketServer::start_accept(); // 递归调用以接受下一个连接
            }
        });
}

// 处理客户端连接的入口函数
void SocketServer::handle_client(std::shared_ptr<asio::ip::tcp::socket> socket) {
    std::cout << "Server: handle_client entered. Initiating first read." << std::endl;
    // 为这个连接创建一个共享的 streambuf，用于读取数据
    auto read_buffer = std::make_shared<asio::streambuf>();
    // 调用 do_read 辅助函数来开始第一次读取操作
    SocketServer::do_read(socket, read_buffer);
}

// 辅助函数：封装了从客户端读取消息的逻辑
void SocketServer::do_read(std::shared_ptr<asio::ip::tcp::socket> socket,
                           std::shared_ptr<asio::streambuf> read_buffer) {
    
    // 清空 streambuf 中可能残留的旧数据，为新的读取做准备
    read_buffer->consume(read_buffer->size());

    std::cout << "Server: Reading from client "
              << socket->remote_endpoint().address().to_string() << ":"
              << socket->remote_endpoint().port()
              << std::endl;

    // 发起异步读取操作，直到遇到换行符 '\n'
    asio::async_read_until(*socket, *read_buffer, '\n',
        [socket, read_buffer](const asio::error_code& error, std::size_t bytes_transferred) {
            // 注意：lambda 表达式内部访问静态成员时，仍然需要使用类名::成员名，或者捕获 this（如果是非静态成员函数）。
            // 由于这里是静态成员函数，直接使用 SocketServer::request_handler_ 等即可。

            if (!error) {
                // 成功读取到数据
                std::cout << "Server: Read " << bytes_transferred << " bytes from client." << std::endl;
                
                std::istream is(read_buffer.get());
                std::string request_str;
                std::getline(is, request_str); // 从 streambuf 中提取一行数据（不包含换行符）

                std::cout << "Server: Extracted request: [" << request_str << "]" << std::endl;

                // 调用请求处理器，并传入编译器/VM实例的引用
                std::string response_str = "{}"; // 默认空JSON响应
                if (SocketServer::request_handler_ && SocketServer::s_parser_ && SocketServer::s_code_gen_ && SocketServer::s_vm_) {
                    std::cout << "Received request: " << request_str << std::endl; // 这是你最初希望看到的日志
                    response_str = SocketServer::request_handler_(request_str, *SocketServer::s_parser_, *SocketServer::s_code_gen_, *SocketServer::s_vm_);
                } else {
                    std::cerr << "Server: Error: Request handler or compiler/VM components not initialized." << std::endl;
                    response_str = "{\"status\": \"error\", \"message\": \"Server not fully initialized or handler missing.\"}";
                }

                // 添加换行符作为响应的结束标志，确保客户端能正确读取
                response_str += "\n"; 

                // 发起异步写入操作，将响应发送回客户端
                asio::async_write(*socket, asio::buffer(response_str),
                    [socket, read_buffer](const asio::error_code& error, std::size_t /*bytes_transferred*/) {
                        if (error) {
                            std::cerr << "Server: Write error: " << error.message() << std::endl;
                        }

                        // **** 最关键的一步：在发送完响应后，再次调度读取操作，以便接收来自同一客户端的下一条消息 ****
                        if (socket->is_open()) { // 只有当 socket 仍然开放时才继续读取
                            SocketServer::do_read(socket, read_buffer); // 递归调用以读取更多数据
                        }
                    });

            } else if (error == asio::error::eof || error == asio::error::connection_reset || error == asio::error::operation_aborted) {
                // 客户端正常断开连接（EOF）或连接被重置，或操作被取消
                std::cout << "Server: Client disconnected (EOF or reset/aborted): " << error.message() << std::endl;
                // 客户端已断开，无需再发起读取操作
            } else {
                // 发生了其他读取错误
                std::cerr << "Server: Read error: " << error.message() << std::endl;
                // 在严重错误时，可以关闭这个 socket
                if (socket->is_open()) {
                    asio::error_code ec;
                    socket->close(ec); // 关闭有问题的 socket
                }
            }
        });
}

// 启动服务器的核心函数
void SocketServer::start(int port, RequestHandler handler, Parser& parser, VmCodeGenerator& code_gen, VirtualMachine& vm) {
    // 检查服务器是否已在运行
    if (io_thread_ && io_thread_->joinable()) {
        std::cout << "Server is already running." << std::endl;
        return;
    }

    // 初始化静态成员指针
    request_handler_ = handler;
    s_parser_ = &parser;
    s_code_gen_ = &code_gen;
    s_vm_ = &vm;

    try {
        io_context_ = std::make_unique<asio::io_context>();
        // 创建 work_guard_，防止 io_context_ 在没有待处理任务时自动退出
        work_guard_ = std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(asio::make_work_guard(*io_context_));

        // 初始化 acceptor_
        acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(*io_context_,
                                                               asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        std::cout << "Server listening on port " << port << std::endl;

        // 启动第一个接受连接的异步操作
        SocketServer::start_accept();

        // 在单独的线程中运行 io_context_
        io_thread_ = std::make_unique<std::thread>([]() {
            try {
                std::cout << "Server: IO context thread started." << std::endl;
                io_context_->run();
                std::cout << "Server: IO context run finished." << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Server: Asio io_context run error: " << e.what() << std::endl;
            }
        });

    } catch (const asio::system_error& e) {
        std::cerr << "Server: Socket server error: " << e.what() << std::endl;
        // 如果启动失败，确保清理状态
        if (work_guard_) work_guard_.reset();
        if (io_context_) io_context_->stop();
        if (io_thread_ && io_thread_->joinable()) io_thread_->join();
        io_context_ = nullptr;
        acceptor_ = nullptr;
        io_thread_ = nullptr;
    }
}

// 停止服务器
void SocketServer::stop() {
    std::cout << "Server: Stopping server..." << std::endl;
    // 先取消 work_guard_，允许 io_context_->run() 退出
    if (work_guard_) {
        work_guard_.reset();
    }

    // 停止 io_context_，这将取消所有挂起的异步操作
    if (io_context_) {
        io_context_->stop();
    }
    
    // 关闭 acceptor
    if (acceptor_ && acceptor_->is_open()) {
        asio::error_code ec;
        acceptor_->close(ec);
        if (ec) {
            std::cerr << "Server: Error closing acceptor: " << ec.message() << std::endl;
        }
    }

    // 等待 I/O 线程完成
    if (io_thread_ && io_thread_->joinable()) {
        io_thread_->join();
        io_thread_.reset(); // 释放线程资源
    }

    // 清理其他 Asio 对象
    io_context_.reset();
    acceptor_.reset();

    // 清空静态指针，避免 dangling pointers
    request_handler_ = nullptr; // 重置 handler
    s_parser_ = nullptr;
    s_code_gen_ = nullptr;
    s_vm_ = nullptr;

    std::cout << "Server stopped." << std::endl;
}