#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <pthread.h>

using namespace std;

void * recvThread(void *arg)
{
    char buff_rcv[256];
    memset(buff_rcv, 0, sizeof(buff_rcv));
    int SocketFD = *((int *)arg);
    while(true){
        if (read(SocketFD, buff_rcv, sizeof buff_rcv ) <= 0) {
            perror("Error receiving response");
            break;
        }
        cout << "Server response: " << buff_rcv << endl;
    }
    pthread_exit(NULL);
}

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

    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    else {
        cout << "Connection accepted" << endl;
    }

    char login[256] ;
    cout << "Jesli chcesz sie zalogowac napisz 'zaloguj'\n";
    cout << "jesli chcesz zalozyc konto napisz 'zaloz'" << endl;
    cin >> login;
    if (write(SocketFD, login, sizeof login) <= 0) {
        perror("Error sending login option");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    // Wysyłanie loginu
    cout << "Podaj login: " << endl;
    cin>>login;
    if (write(SocketFD, login, sizeof login) <= 0) {
        perror("Error sending login");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    // Wysyłanie hasła
    cout << "Podaj hasło: " << endl;
    cin>>login;
    if (write(SocketFD, login, sizeof login) <= 0) {
        perror("Error sending password");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    pthread_t thread_id;
    if( pthread_create(&thread_id, NULL, recvThread, &SocketFD) != 0 )
        printf("Failed to create thread\n");
    pthread_detach(thread_id);

    cin.ignore(); 
    char buff[256];
    // Pętla do dalszej komunikacji
    while (true) {
        memset(buff, 0, sizeof(buff));
        cout << "Enter message: ";
        cin.getline(buff, sizeof(buff)); 
        if (strcmp(buff, "exit") == 0) {
            break;
        }
        if (write(SocketFD, buff, sizeof buff) <= 0) {
            perror("Error sending message");
            break;
        }
    }

    close(SocketFD);
    return EXIT_SUCCESS;
}