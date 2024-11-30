#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

using namespace std;

int main(){
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof sa);
    int port = 1100;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    cout << "addr: " << "127.0.0.1" << endl;
    cout << "port: " << port << endl;
    
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFD == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    char login[256] ;
    char password[256] ;

    cout << "Podaj login: " << endl;
    cin>>login;
    cout << "Podaj hasło: " << endl;
    cin>>password;

    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    else {
        cout << "Connection accepted" << endl;
    }

    char buff[256];
    char buff_rcv[256];

    // Wysyłanie loginu
    if (write(SocketFD, login, sizeof login) <= 0) {
        perror("Error sending login");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    // Wysyłanie hasła
    if (write(SocketFD, password, sizeof password) <= 0) {
        perror("Error sending password");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    // Pętla do dalszej komunikacji
    while (true) {
        memset(buff, 0, sizeof(buff));
        bzero(buff,256);
        cout << "Enter message: ";
        cin.ignore(); 
        cin.getline(buff, sizeof(buff)); 

        if (strcmp(buff, "exit\n") == 0) {
            break;
        }

        if (write(SocketFD, buff, sizeof buff) <= 0) {
            perror("Error sending message");
            break;
        }

        memset(buff_rcv, 0, sizeof(buff_rcv));
        bzero(buff_rcv,256);
        if (read(SocketFD, buff_rcv, sizeof buff_rcv ) <= 0) {
            perror("Error receiving response");
            break;
        }
        cout << "Server response: " << buff_rcv << endl;
    }

    close(SocketFD);
    return EXIT_SUCCESS;
}
