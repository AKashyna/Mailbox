#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <string>

constexpr int PORT = 1100;
constexpr int BUFFER_SIZE = 1024;

void handleIncomingConnections(int server_socket) {
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);

        if (client_socket < 0) {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }

        char buffer[BUFFER_SIZE] = {0};
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        std::cout << "Message received: " << buffer << std::endl;

        close(client_socket);
    }
}

void connectToOtherServers(const std::vector<std::string>& ips) {
    for (const auto& ip : ips) {
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket < 0) {
            std::cerr << "Error creating socket for " << ip << std::endl;
            continue;
        }

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);

        if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
            std::cerr << "Invalid IP address: " << ip << std::endl;
            close(client_socket);
            continue;
        }

        if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Could not connect to " << ip << std::endl;
            close(client_socket);
            continue;
        }

        const char* message = "poloczono";
        send(client_socket, message, strlen(message), 0);
        std::cout << "Connected and sent message to " << ip << std::endl;

        close(client_socket);
    }
}

int main() {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    std::cout << "addr: " << INADDR_ANY << std::endl;
    std::cout << "port: " << PORT << std::endl;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket\n";
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        std::cerr << "Error listening on socket\n";
        close(server_socket);
        return 1;
    }

    std::cout << "Server is running and waiting for connections..." << std::endl;

    // List of IPs in the network (for demo purposes, update with your logic to discover IPs)
    std::vector<std::string> ips = {"10.0.2.15"};

    // Start a thread to handle incoming connections
    std::thread incomingThread(handleIncomingConnections, server_socket);

    // Connect to other servers in the network
    connectToOtherServers(ips);

    incomingThread.join();
    close(server_socket);

    return 0;
}
