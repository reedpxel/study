#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define PORT 55555
#define MAX_CONNECTION_QUEUE_SIZE 5
#define BUFFER_SIZE 1024
#define EPOLL_WAIT_ARRAY_SIZE 10

void handle_input(int fd)
{
    struct sockaddr_in client_sockaddr;
    unsigned sockaddr_sz = sizeof(client_sockaddr);
    // does not make a network query to get distant device parameters
    if (getpeername(fd, (struct sockaddr*)&client_sockaddr, &sockaddr_sz) < 0)
    {
        perror("getpeername");
        close(fd);
        return; 
    }
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_sockaddr.sin_addr, client_ip, INET_ADDRSTRLEN);
 
    char buffer[BUFFER_SIZE];
    ssize_t read_bytes = read(fd, buffer, BUFFER_SIZE);
    if (read_bytes == 0)
    {
        printf("End of file received. Connection closed with %s:%hu\n",
            client_ip, ntohs(client_sockaddr.sin_port));
        close(fd);
        return;
    }
    else if (read_bytes < 0)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }
   
    printf("%lu bytes received from %s:%hu\n", read_bytes, client_ip, 
        ntohs(client_sockaddr.sin_port));
    
    if (write(fd, buffer, read_bytes) < 0)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

int setup_server_socket()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int reuseaddr_option_value = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,
        &reuseaddr_option_value, sizeof(reuseaddr_option_value)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_socket, MAX_CONNECTION_QUEUE_SIZE) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening to port %d\n", PORT);
    return server_socket;
}

void register_incoming_connection(int server_socket, int epoll_fd)
{
    struct sockaddr_in addr_client;
    unsigned sockaddr_sz = sizeof(addr_client);
    int connection_fd = accept(server_socket, 
        (struct sockaddr*)&addr_client, &sockaddr_sz);
    if (connection_fd < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr_client.sin_addr, client_ip, 
        INET_ADDRSTRLEN);
    printf("Connection accepted from %s:%hu\n", client_ip, 
        ntohs(addr_client.sin_port));

    int flags = 0;
    if (fcntl(connection_fd, F_GETFL) < 0)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    flags |= SOCK_NONBLOCK;
    if (fcntl(connection_fd, F_SETFL, flags) < 0)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    struct epoll_event connection_evt;
    connection_evt.events = EPOLLIN | EPOLLET;
    connection_evt.data.fd = connection_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_fd,
        &connection_evt) < 0)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    int server_socket = setup_server_socket();

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = server_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &evt) < 0)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
    
    while (1)
    {
        struct epoll_event incoming_evts[EPOLL_WAIT_ARRAY_SIZE];
        int event_count = epoll_wait(epoll_fd, incoming_evts, 
            EPOLL_WAIT_ARRAY_SIZE, -1);
        if (event_count < 0)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < event_count; ++i)
        {
            if (incoming_evts[i].data.fd == server_socket)
            {
                // the only events that may occur on the server socket are
                // incoming connections. All other events occur on connection
                // descriptors
                register_incoming_connection(server_socket, epoll_fd);
            }
            else
            {
                handle_input(incoming_evts[i].data.fd);
            }
        }
    }
}

