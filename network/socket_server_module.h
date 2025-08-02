#ifndef SOCKET_SERVER_MODULE_H
#define SOCKET_SERVER_MODULE_H

#include <string>
#include <functional>
#include <memory>
#include <thread> // 需要包含线程头文件
#include <asio.hpp> // Asio库的头文件

// 前置声明你的编译器组件类
class Parser;
class VmCodeGenerator;
class VirtualMachine;

class SocketServer {
public:
    // 回调函数类型：接收请求字符串，返回响应字符串。
    // 现在我们将编译器/VM实例的引用传递给它，以便处理请求时直接访问。
    using RequestHandler = std::function<std::string(const std::string&, Parser&, VmCodeGenerator&, VirtualMachine&)>;

    // 启动服务器。
    static void start(int port, RequestHandler handler, Parser& parser, VmCodeGenerator& code_gen, VirtualMachine& vm);

    // 停止服务器
    static void stop();

private:
    // Asio 核心上下文和相关对象
    static std::unique_ptr<asio::io_context> io_context_;
    static std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
    static std::unique_ptr<std::thread> io_thread_; // 用于运行 io_context_ 的线程
    
    // work_guard 用于防止 io_context_ 在没有待处理任务时自动停止
    static std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard_;

    // 请求处理器回调函数
    static RequestHandler request_handler_;

    // 静态指针，指向在 main 中创建的编译器/VM实例
    // 注意：这些指针需要保证它们指向的对象在 SocketServer 生命周期内有效
    static Parser* s_parser_;
    static VmCodeGenerator* s_code_gen_;
    static VirtualMachine* s_vm_;

    // 私有辅助方法
    static void start_accept(); // 开始接受新的连接
    
    // 处理客户端连接的入口函数
    static void handle_client(std::shared_ptr<asio::ip::tcp::socket> socket);

    // 新增：封装单个客户端读取操作的辅助函数
    static void do_read(std::shared_ptr<asio::ip::tcp::socket> socket,
                        std::shared_ptr<asio::streambuf> read_buffer);

    // 新增：封装单个客户端写入操作的辅助函数（可选，但为了对称性可以添加）
    // static void do_write(std::shared_ptr<asio::ip::tcp::socket> socket,
    //                      std::string message);
};

#endif // SOCKET_SERVER_MODULE_H