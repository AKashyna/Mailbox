#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>

constexpr int PORT = 1101;
constexpr int BUFFER_SIZE = 1024;

void handleIncomingMessages(int server_socket) {
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        char buffer[BUFFER_SIZE] = {0};

        int bytes_received = recvfrom(
            server_socket,
            buffer,
            BUFFER_SIZE,
            0,
            (struct sockaddr *)&client_addr,
            &client_len
        );

        if (bytes_received < 0) {
            std::cerr << "Error receiving message" << std::endl;
            continue;
        }

        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        std::cout << "Message received from " << client_ip << ": " << buffer << std::endl;
    }
}

void sendBroadcastMessage() {
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        std::cerr << "Error creating UDP socket" << std::endl;
        return;
    }

    // Ustawienie opcji na wysyłanie broadcastu
    int broadcast_enable = 1;
    if (setsockopt(client_socket, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        std::cerr << "Error setting socket options for broadcast" << std::endl;
        close(client_socket);
        return;
    }

    struct sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr("192.168.0.255"); // Adres broadcast w Twojej sieci

    const char *message = "Broadcast message from server";
    int bytes_sent = sendto(
        client_socket,
        message,
        strlen(message),
        0,
        (struct sockaddr *)&broadcast_addr,
        sizeof(broadcast_addr)
    );

    if (bytes_sent < 0) {
        std::cerr << "Error sending broadcast message" << std::endl;
    } else {
        std::cout << "Broadcast message sent" << std::endl;
    }

    close(client_socket);
}

int main() {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        std::cerr << "Error creating UDP socket\n";
        return 1;
    }

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding UDP socket\n";
        close(server_socket);
        return 1;
    }

    std::cout << "Server is running and waiting for messages on port " << PORT << "..." << std::endl;

    // Wątek obsługujący przychodzące wiadomości
    std::thread incomingThread(handleIncomingMessages, server_socket);

    // Wysyłanie wiadomości broadcastowych
    sendBroadcastMessage();

    incomingThread.join();
    close(server_socket);

    return 0;
}
