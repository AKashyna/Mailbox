#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <unistd.h>
#include <pthread.h>
#include "Database.h"

using namespace std;

void loguj(string &option, string &login, string &password, int newSocket) {
    char optionBuffer[256], loginBuffer[256], passwordBuffer[256];
    memset(loginBuffer, 0, 256);
    memset(passwordBuffer, 0, 256);
    memset(optionBuffer, 0, 256);

    // Odbiór opcji wejscia
    if (read(newSocket, optionBuffer, sizeof(optionBuffer)) <= 0) {
        cerr << "Error reading option\n";
        close(newSocket);
        pthread_exit(NULL);
    }
    option = string(optionBuffer);
    cout << "User chose option: " << option << endl;

    // Odbiór loginu
    if (read(newSocket, loginBuffer, 256) <= 0) {
        cerr << "Error reading login\n";
        close(newSocket);
        pthread_exit(NULL);
    }
    login = string(loginBuffer);
    cout << "Received login: " << login << endl;

    // Odbiór hasła
    if (read(newSocket, passwordBuffer, 256) <= 0) {
        cerr << "Error reading password\n";
        close(newSocket);
        pthread_exit(NULL);
    }
    password = string(passwordBuffer);
    cout << "Received password: " << password << endl;
}

void *socketThread(void *arg) {
    //Acceptowanie nowego uzytkownika
    int newSocket = *((int *)arg);
    string option, login, password;
    do{
        loguj(option, login, password, newSocket);
        if(option == "zaloguj" && czyIstnieje(login, password))
            break;
        if(option == "rejestruj" && !czyIstniejeUzytkownik(login)){
            if (!createAccount(login, password)) cerr << "Error adding new account\n";
            break;
        }
        if (write(newSocket, "Access denied", 14) < 0)
                perror("Error writing to socket");
    }while(1);
    if (write(newSocket, "Accept", 7) < 0){
        perror("Error writing to socket");
    }

    // Wysylanie aktualnie dostepnych wiadomosci
    vector<tuple<string, string>> topics = getTopics(login); 
    for (const auto& [sender, topic] : topics) {
        string mess = "Od: " + sender + "\nTemat: " + topic + "\n";
        cout << "message to  client: " << mess << endl;
        if (write(newSocket, mess.c_str(), mess.length()) <= 0)
            cerr << "Error while sending topics to client\n";
    }
    if (write(newSocket, "test", 5) <= 0) {
        cerr << "Error writing to socket";
    }

    //pętla do dalszej komunikacji
    while(true){
        char receiver[256], subject[256], content[1000];
        memset(receiver, 0, sizeof(receiver));
        memset(subject, 0, sizeof(subject));
        memset(content, 0, sizeof(content));

        if (read(newSocket, receiver, sizeof(receiver)) <= 0) {
            cerr << "Client disconnected or error reading\n";
            break;
        }
        cout << "Received reciver: " << receiver << std::endl;
        if (read(newSocket, subject, sizeof(subject)) <= 0) {
            cerr << "Client disconnected or error reading\n";
            break;
        }
        cout << "Received topic: " << subject << std::endl;
        if (read(newSocket, content, sizeof(content)) <= 0) {
            cerr << "Client disconnected or error reading\n";
            break;
        }
        cout << "Received message: " << content << std::endl;
        addMessage(login, receiver, subject, content);
    }
    close(newSocket);
    pthread_exit(NULL);
}

int main() {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    int port = 1100;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    cout << "addr: " << INADDR_ANY<< endl;
    cout << "port: " << port << endl;

    int my_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (my_socket == -1) {
        cerr << "Error creating socket\n";
        return 1;
    }

    if (bind(my_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding socket\n";
        close(my_socket);
        return 1;
    }

    if (listen(my_socket, 5) < 0) {
        cerr << "Error listening on socket\n";
        close(my_socket);
        return 1;
    }

    if(create() == -1){
        cerr << "Error creating Database";
    }

    pthread_t thread_id;
    // Pętla do dalszej komunikacji
    while (true) {
        int rcv_socket = accept(my_socket, nullptr, nullptr); 
        if (rcv_socket < 0) {
            cerr << "Error accepting connection\n";
            close(my_socket);
            return 1;
        }

        if( pthread_create(&thread_id, NULL, socketThread, &rcv_socket) != 0 )
           printf("Failed to create thread\n");

        pthread_detach(thread_id);
    }
    close(my_socket);
    return 0;
}
