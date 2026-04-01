#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#define BUFFER_SIZE 1024
#define PORT 55555

using namespace boost::asio;

std::string remoteHost(const ip::tcp::socket& socket_) try
{
    ip::tcp::endpoint ep = socket_.remote_endpoint();
    return ep.address().to_string() + ":" + std::to_string(ep.port());
}
catch (...)
{
    return "unknown distant device";
}

void handleSession(ip::tcp::socket socket_)
{
    char buffer_[BUFFER_SIZE];
    while (true)
    {
        try
        {
            size_t readBytes = socket_.read_some(buffer(buffer_, BUFFER_SIZE));
            std::cout << readBytes << " read from " << remoteHost(socket_)
                << std::endl;
            socket_.write_some(buffer(buffer_, readBytes));
        }
        catch (const boost::system::system_error& exc)
        {
            std::cout << "Connection closed with " << remoteHost(socket_) 
                << ": " << exc.what() << std::endl;
            return;
        }
    }
}

int main()
{
    try
    {
        io_context io;
        ip::tcp::acceptor acceptor_(io, 
            ip::tcp::endpoint(ip::tcp::v4(), PORT)); 
        std::cout << "Listening to port " << PORT << std::endl;
        while (true)
        {
            ip::tcp::socket socket_(io);
            acceptor_.accept(socket_);
            std::cout << "New connection from " << remoteHost(socket_) << '\n';
            std::thread([socket_ = std::move(socket_)]() mutable 
            {
                handleSession(std::move(socket_));
            }).detach();
        }
    }
    catch (const std::exception& exc)
    {
        std::cout << exc.what() << std::endl;
    }
}

