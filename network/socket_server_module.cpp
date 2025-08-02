#include "socket_server_module.h"
#include <iostream>
#include <thread>   // 明确包含 <thread>
#include <string>
#include <limits> // For std::numeric_limits
#include <asio/ts/buffer.hpp> // For asio::buffer
#include <asio/ts/internet.hpp> // For asio::ip::tcp
#include <asio/streambuf.hpp> // 修复：添加对 asio::streambuf 的包含

// #include <nlohmann/json.hpp> // 假设你引入了C++ JSON库，如jsoncpp或nlohmann/json

// 包含你的编译器组件头文件
#include "parser.h"
#include "virtualMachine.h"
#include "vmCodeGenerator.h"

// 辅助函数：转义字符串中的双引号和反斜杠，使其可以在JSON字符串中安全嵌入
// 应该放在 .cpp 文件顶部，或者 SocketServer 类内部作为 private static 成员函数
std::string escape_json_string(const std::string& input) {
    std::string output;
    output.reserve(input.length() * 2); // 预留更多空间以防转义，最坏情况是所有字符都需要转义
    for (char c : input) {
        if (c == '"' || c == '\\') { // 如果是双引号或反斜杠，则转义
            output += '\\';
        }
        output += c;
    }
    return output;
}


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
    
    // 每次读取前清空缓冲区
    // 注意：read_buffer->consume() 会移动内部指针，而不是清空数据
    // 在这里我们希望它在每次新读之前是空的，但 async_read 默认会 append 到 streambuf 的 write-area
    // 如果你期望每次只处理一个完整的消息，并清空，那么在每次读取前消费掉所有现有数据是合理的。
    // 如果你期待消息可能分包到达，并且 streambuf 可能包含前一次读取的剩余数据，则不应完全 consume。
    // 但鉴于你的协议是 newline delimited，并且你正在检查 newline_pos，当前处理方式是合理的。
    read_buffer->consume(read_buffer->size());

    std::cout << "Server: Reading from client "
              << socket->remote_endpoint().address().to_string() << ":"
              << socket->remote_endpoint().port()
              << std::endl;

    // **** 关键修改在这里：使用 async_read 和 transfer_at_least ****
    // 这将确保只要有任何数据到达，回调就会被触发
    asio::async_read(*socket, *read_buffer, asio::transfer_at_least(1), // 只要有1个字节到达就触发回调
        [socket, read_buffer](const asio::error_code& error, std::size_t bytes_transferred) {
            std::cout << "--- DEBUG: Entered async_read callback! Error: " << error.message() 
                      << ", Bytes Transferred: " << bytes_transferred << std::endl; 
            
            if (!error) {
                // 成功读取到数据
                std::cout << "DEBUG: Inside async_read success callback! Bytes: " << bytes_transferred << std::endl; 
                std::cout << "Server: Read " << bytes_transferred << " bytes from client." << std::endl;

                // **** 这是关键的修改：打印 streambuf 中的原始字节 ****
                std::cout << "DEBUG: Raw buffer bytes (size: " << read_buffer->size() << "): [";
                bool first_byte = true;
                for (auto g = asio::buffers_begin(read_buffer->data());
                     g != asio::buffers_end(read_buffer->data()); ++g) {
                    if (!first_byte) {
                        std::cout << ", ";
                    }
                    std::cout << static_cast<int>(*g); // 将字节作为整数打印
                    first_byte = false;
                }
                std::cout << "]" << std::endl;
                // **** 关键修改结束 ****

                // 将 streambuf 的内容复制到 string
                // 注意：这个 std::string 构造函数在遇到空字符 '\0' 时可能会截断字符串
                // 但对于UTF-8 JSON，通常不会有空字符，所以这里是安全的
                std::string current_buffer_content(
                    asio::buffers_begin(read_buffer->data()),
                    asio::buffers_begin(read_buffer->data()) + read_buffer->size() 
                );
                std::cout << "DEBUG: String converted from buffer: [" << current_buffer_content << "]" << std::endl;


                // 现在，我们手动查找换行符
                size_t newline_pos = current_buffer_content.find('\n');

                if (newline_pos != std::string::npos) {
                    // 找到换行符，处理完整的消息
                    std::string request_str = current_buffer_content.substr(0, newline_pos);
                    // 消耗掉已处理的消息部分（包括换行符）
                    read_buffer->consume(newline_pos + 1); 

                    std::cout << "DEBUG: Full message (before newline): [" << request_str << "]" << std::endl;
                    
                    // 简化后的响应逻辑
                    // 关键修改：转义 request_str 中的双引号和反斜杠，使其作为 JSON 字符串的值是合法的
                    std::string escaped_request_str = escape_json_string(request_str); 
                    std::string response_str_to_send = "{\"status\": \"success\", \"message\": \"Received: " + escaped_request_str + "\"}"; // 重命名以区分
                    response_str_to_send += "\n"; // 添加换行符作为响应的结束标志

                    std::cout << "DEBUG: Attempting to write response: [" << response_str_to_send << "]" << std::endl;
                    
                    // ✅ 关键修改：将 response_str_to_send 移动或复制到 async_write 的 lambda 捕获列表中
                    // 使用 move 语义可以避免不必要的拷贝，如果 response_str_to_send 不再被当前 lambda 的后续代码使用。
                    // 对于 std::string，直接捕获 by value ([response_str_to_send]) 也会创建副本，确保生命周期。
                    asio::async_write(*socket, asio::buffer(response_str_to_send),
                        [socket, read_buffer, response_str_to_send = std::move(response_str_to_send)](const asio::error_code& error, std::size_t bytes_transferred_write) { // <-- 在这里捕获 response_str_to_send
                            std::cout << "DEBUG: Inside async_write callback!" << std::endl;
                            // response_str_to_send 现在作为 lambda 的一个副本，其生命周期将持续到此回调执行完毕
                            if (error) {
                                std::cerr << "Server: Write error: " << error.message() << std::endl;
                            } else {
                                std::cout << "Server: Wrote " << bytes_transferred_write << " bytes to client." << std::endl;
                            }

                            if (socket->is_open()) {
                                SocketServer::do_read(socket, read_buffer); 
                            }
                        });
                } else {
                    // 没有找到换行符，说明消息不完整，需要继续读取
                    // 如果缓冲区过大（例如，超过某个限制），可能需要考虑关闭连接，以防止恶意攻击或内存耗尽
                    std::cout << "DEBUG: Newline not found yet. Continuing to read." << std::endl;
                    if (socket->is_open()) {
                        SocketServer::do_read(socket, read_buffer); // 继续读取
                    }
                }

            } else if (error == asio::error::eof || error == asio::error::connection_reset || error == asio::error::operation_aborted) {
                std::cout << "Server: Client disconnected (EOF or reset/aborted): " << error.message() << std::endl;
                // 通常在这里关闭 socket，因为它不再可用
                if (socket->is_open()) {
                    asio::error_code ec;
                    socket->close(ec); 
                }
            } else {
                std::cerr << "Server: Read error: " << error.message() << std::endl;
                if (socket->is_open()) {
                    asio::error_code ec;
                    socket->close(ec); 
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
        std::cout << "Server running. Send {\"command\": \"shutdown\"} to stop, or press Ctrl+C." << std::endl; // 提示如何停止

        // 启动第一个接受连接的异步操作
        SocketServer::start_accept();

        // 在单独的线程中运行 io_context_
        // 静态成员函数不需要捕获 'this' 来访问静态成员
        io_thread_ = std::make_unique<std::thread>([]() { 
            try {
                std::cout << "Server: IO context thread started." << std::endl;
                // 运行 io_context_ (通过类名访问静态成员)
                SocketServer::io_context_->run(); 
                std::cout << "Server: IO context run finished." << std::endl;
            } catch (const asio::system_error& e) { // 捕获 Asio 系统错误
                std::cerr << "Server: Asio system error in io_thread: " << e.what() << std::endl;
            } catch (const std::exception& e) { // 捕获所有标准异常
                std::cerr << "Server: General exception in io_thread: " << e.what() << std::endl;
            } catch (...) { // 捕获所有其他未知异常
                std::cerr << "Server: Unknown exception caught in io_thread!" << std::endl;
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