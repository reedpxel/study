#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define PORT 55556
#define MAX_CONNECTION_QUEUE_SIZE 5
#define BUFFER_SIZE 1024

void* session_routine(void* conn_fd)
{
    int connection_fd = *(int*)conn_fd;

    struct sockaddr_in client_sockaddr;
    unsigned sockaddr_sz = sizeof(client_sockaddr);
    int getpeername_res = getpeername(connection_fd, 
        (struct sockaddr*)&client_sockaddr, &sockaddr_sz);
    if (getpeername_res < 0)
    {
        perror("getpeername");
    }
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_sockaddr.sin_addr, client_ip, INET_ADDRSTRLEN);
    uint16_t client_port = client_sockaddr.sin_port;

    char buffer_[BUFFER_SIZE];

    while (1)
    {
        ssize_t read_bytes = read(connection_fd, buffer_, BUFFER_SIZE);
        if (read_bytes == 0)
        {
            printf("End of file received. ");
            break;
        }
        else if (read_bytes < 0)
        {
            perror("read");
            break;
        }
        if (getpeername_res >= 0)
        {
            printf("%lu bytes read from %s:%hu\n", read_bytes, client_ip, 
                client_port);
        }
        else
        {
            printf("%lu bytes read from unknown distant device\n", read_bytes);
        }

        if (write(connection_fd, buffer_, read_bytes) < 0)
        {
            perror("write");
            break;
        }
    }
    close(connection_fd);
    printf("Connection closed with %s:%hu\n", client_ip, client_port);
    free(conn_fd);
    return NULL;
}

int setup_server_socket()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // allowing multiple connections for the server socket
    int reuseaddr_option_value = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
        &reuseaddr_option_value, sizeof(reuseaddr_option_value)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    // htons, htonl convert unsigned short/long from host byte order to network
    // byte order (big-endian)
    // ntohs, ntohl do the opposite transformation
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    if (listen(socket_fd, MAX_CONNECTION_QUEUE_SIZE) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening to port %d\n", PORT);
    return socket_fd;
}

int main()
{
    int socket_fd = setup_server_socket();

    while (1)
    {
        struct sockaddr_in addr_client;
        unsigned sockaddr_sz = sizeof(addr_client);
        
        int* conn_fd = malloc(sizeof(int));
        if (conn_fd == NULL)
        {
            fprintf(stderr, "malloc returned NULL");
            continue;
        }
        *conn_fd = accept(socket_fd, (struct sockaddr*)&addr_client, 
            &sockaddr_sz);
        if (*conn_fd < 0)
        {
            perror("accept");
            free(conn_fd);
            continue;
        }
        char ip_addr_buffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr_client.sin_addr, ip_addr_buffer, 
            INET_ADDRSTRLEN);
        uint16_t client_port = ntohs(addr_client.sin_port);

        printf("Connection accepted from %s:%hu\n", ip_addr_buffer, 
            client_port);

        pthread_t session_thread;
        int pthread_create_res = pthread_create(&session_thread, NULL, 
            &session_routine, (void*)conn_fd);
        if (pthread_create_res != 0)
        {
            perror("pthread_create");
            free(conn_fd);
            continue;
        }
        int detach_res = pthread_detach(session_thread);
        if (detach_res != 0)
        {
            perror("pthread_detach");
            free(conn_fd);
        }
    }
}

