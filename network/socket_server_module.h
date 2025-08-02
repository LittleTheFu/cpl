#pragma once // 防止头文件被重复包含

#include <string>
#include <memory> // For std::unique_ptr, std::shared_ptr
#include <functional> // For std::function
#include <asio/ts/buffer.hpp> // For asio::buffer
#include <asio/ts/internet.hpp> // For asio::ip::tcp
#include <thread> // 修复: 添加对 std::thread 的包含
#include <asio/streambuf.hpp> // **** 修复: 添加对 asio::streambuf 的包含 ****

// 前向声明您的编译器组件类，避免循环引用
// 这样在头文件中就不需要包含这些类的完整定义，只需要知道它们是类即可。
class Parser;
class VmCodeGenerator;
class VirtualMachine;

// SocketServer 类定义
class SocketServer {
public:
    // 定义一个函数类型，用于处理客户端请求
    // 这个回调函数将接收原始请求字符串，并返回一个响应字符串。
    // 它也接收对编译器/VM组件的引用，以便执行编译或VM操作。
    using RequestHandler = std::function<std::string(const std::string&, Parser&, VmCodeGenerator&, VirtualMachine&)>;

    // 启动服务器
    // @param port 监听的端口号
    // @param handler 处理请求的回调函数
    // @param parser 对 Parser 对象的引用
    // @param code_gen 对 VmCodeGenerator 对象的引用
    // @param vm 对 VirtualMachine 对象的引用
    static void start(int port, RequestHandler handler, Parser& parser, VmCodeGenerator& code_gen, VirtualMachine& vm);

    // 停止服务器
    static void stop();

private:
    // Asio 核心对象
    static std::unique_ptr<asio::io_context> io_context_;
    static std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
    static std::unique_ptr<std::thread> io_thread_; // 用于在单独线程中运行 io_context
    // asio::executor_work_guard 用于阻止 io_context_ 在没有待处理任务时自动退出
    static std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard_;

    // 请求处理器回调函数
    static RequestHandler request_handler_;

    // 编译器/VM组件的静态指针
    // 注意：这些指针指向的对象必须在 SocketServer::start() 调用时是有效的，
    // 并且它们的生命周期要长于服务器的运行时间，直到 SocketServer::stop() 被调用。
    static Parser* s_parser_;
    static VmCodeGenerator* s_code_gen_;
    static VirtualMachine* s_vm_;

    // 私有辅助函数
    // 启动接受新连接的异步操作
    static void start_accept();

    // 处理单个客户端连接的入口函数
    static void handle_client(std::shared_ptr<asio::ip::tcp::socket> socket);

    // 辅助函数：封装了从客户端读取消息的逻辑
    static void do_read(std::shared_ptr<asio::ip::tcp::socket> socket,
                        std::shared_ptr<asio::streambuf> read_buffer);

    // 辅助函数：封装了向客户端写入消息的逻辑 (如果需要单独的 do_write)
    // static void do_write(std::shared_ptr<asio::ip::tcp::socket> socket,
    //                      std::string message);
};