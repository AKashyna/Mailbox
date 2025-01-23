#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <functional>
#include "client.h"

using namespace std;

struct sockaddr_in sa;
int SocketFD;

bool connect(){
    memset(&sa, 0, sizeof sa);
    int port = 1100;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");

    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("cannot create socket");
        return false;
    }
    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");
        close(SocketFD);
        return false;
    }
    if (write(SocketFD, "client", 7) <= 0) {
        perror("Error sending connection information");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    cout << "Connection accepted" << endl;
    return true;
}

bool log(const string &username, const string &password, const string &option){
    //przesylanie danych logowania
    if (write(SocketFD, option.c_str(), option.length()) <= 0) {
        perror("Error sending login option");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, username.c_str(), username.length()) <= 0) {
        perror("Error sending login");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, password.c_str(), password.length()) <= 0) {
        perror("Error sending password");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    //sprawdzanie czy zaakceptowano dane
    char rcv[256];
    memset(rcv, 0, 256);
    if (read(SocketFD, rcv, sizeof rcv ) <= 0) {
        perror("Error receiving response");
    }
    cout << "^^^^^^^^^^^^^^^" << rcv << endl;
    if(strcmp(rcv, "Accept") != 0) return false;
    return true;
}

bool send(const string &reciver, const string &topic, const string &message){
    if (write(SocketFD, "sendMessage", 12) <= 0) {
        perror("Error sending acction");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, reciver.c_str(), reciver.length()) <= 0) {
        perror("Error sending reciver");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, topic.c_str(), topic.length()) <= 0) {
        perror("Error sending topic");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, message.c_str(), message.length()) <= 0) {
        perror("Error sending message");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    return true;
}

vector<tuple<string, string>> getMessages()
{
    usleep(100000);
    if (write(SocketFD, "getMessages", 12) <= 0) {
        perror("Error sending acction");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    vector<tuple<string, string>> messages;
    char rcv[256];
    while(true){
        memset(rcv, 0, sizeof(rcv));
        if (read(SocketFD, rcv, sizeof rcv ) <= 0) {
            perror("Error receiving response");
            break;
        }
        if(strcmp(rcv, "End") == 0){
            break;
        }
        // Parsowanie odebranych danych, zakładam format "sender:topic"
        string received(rcv);
        size_t delimiterPos = received.find(':');
        if (delimiterPos != string::npos) {
            string sender = received.substr(0, delimiterPos);
            string topic = received.substr(delimiterPos + 1);
            messages.emplace_back(sender, topic);
        }
    }
    return messages;
}

string getMessage(const string &sender, const string &topic)
{
    if (write(SocketFD, "getMessage", 11) <= 0) {
        perror("Error sending acction");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, sender.c_str(), sender.length()) <= 0) {
        perror("Error sending sender");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    usleep(100000);
    if (write(SocketFD, topic.c_str(), topic.length()) <= 0) {
        perror("Error sending topic");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    char rcv[1000];
    memset(rcv, 0, sizeof(rcv));
    if (read(SocketFD, rcv, sizeof rcv ) <= 0) {
        perror("Error receiving response");
    }
    return rcv;
}