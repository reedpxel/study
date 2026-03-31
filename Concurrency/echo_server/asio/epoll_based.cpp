#include <iostream>
#include <memory>
#include <boost/asio.hpp>

using namespace boost::asio;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(ip::tcp::socket&& socket_)
            : socket_(std::move(socket_))
    {}

    void start()
    {
        std::cout << "Connection opened with " << remoteHost() << '\n';
        scheduleRead();
    }
private:
    std::string remoteHost() try
    {
        ip::tcp::endpoint ep = socket_.remote_endpoint();
        return ep.address().to_string() + ":" + 
            std::to_string(ep.port());
    }
    catch (...)
    {
        return "unknown distant device";
    }

    void scheduleRead()
    {
        std::shared_ptr<Session> shptr = shared_from_this();
        socket_.async_read_some(buffer(arr, BUFFER_SIZE), 
            [this, shptr](std::error_code ec, size_t bytesReceived)
            {
                if (ec)
                {
                    std::cerr << "Error in read operation, message: " <<
                        ec.message() << ". Connection closed with " << 
                        remoteHost() << '\n';
                    return;
                }
                std::cout << bytesReceived << " bytes received from " << 
                    remoteHost() << '\n';
                scheduleWrite(bytesReceived);
            });
    }

    void scheduleWrite(size_t sz)
    {
        std::shared_ptr<Session> shptr = shared_from_this();
        socket_.async_write_some(buffer(arr, sz),
            [this, sz, shptr](std::error_code ec, size_t)
            {
                if (ec)
                {
                    std::cerr << "Error in write operation, message: " <<
                        ec.message() << ". Connection closed with " << 
                        remoteHost() << '\n';
                    return;
                }
                std::cout << sz << " bytes sent to " << remoteHost() << '\n';
                scheduleRead();
            });
    }

    const static size_t BUFFER_SIZE = 1024;

    ip::tcp::socket socket_;
    char arr[Session::BUFFER_SIZE] = {};
};

class Server
{
public:
    Server(io_context& io)
            : io(io)
            , acceptor_(io, ip::tcp::endpoint(ip::tcp::v4(), PORT))
    {
        std::cout << "Listening to port " << PORT << '\n';
        acceptConnection();
    }

    void acceptConnection()
    {
        acceptor_.async_accept(io, [this](std::error_code ec, 
            ip::tcp::socket socket_)
            {
                acceptConnection();
                if (ec)
                {
                    std::cerr << "Error occured in handleConnection: " << 
                        ec.message() << '\n';
                    return;
                }
                std::make_shared<Session>(std::move(socket_))->start();
            });
    }

    const static uint16_t PORT = 55555;

private:
    io_context& io;
    ip::tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        io_context io;
        Server server(io);
        io.run();
    }
    catch (const std::exception& exc)
    {
        std::cout << "Exception caught: " << exc.what() << std::endl;
    }
}

