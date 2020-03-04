//
// Created by darea on 02/03/2020.
//


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
#include <cstring>
#include <string>
#include <iostream>
#include "../WorkerThreads.cpp"

using namespace std;

/*void * clientThread(void *arg)
{
    printf("In thread\n");
    char message[1000];
    char buffer[1024];
    strcpy(message,"Hello");
    if( send(clientSocket , message , strlen(message) , 0) < 0)
    {
        printf("Send failed\n");
    }
    //Read the message from the server into the buffer
    if(recv(clientSocket, buffer, 1024, 0) < 0)
    {
        printf("Receive failed\n");
    }
    //Print the received message
    printf("Data received: %s\n",buffer);
    close(clientSocket);
    pthread_exit(NULL);
}*/

class Connection {

public:
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    size_t addr_size = sizeof serverAddr;
    int port;
    mutex mtx;

    Connection(const char *host, int port) {
        socklen_t addr_size;
        this->port = port;
        // Create the socket.
        clientSocket = socket(PF_INET, SOCK_STREAM, 0);
        //Configure settings of the server address
        // Address family is Internet
        serverAddr.sin_family = AF_INET;
        //Set port number, using htons function
        serverAddr.sin_port = htons(port);
        //Set IP address to localhost'
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
        //Connect the socket to the server using the address
    }

    void basicConnect() {
        connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
        char firstnum, secondum;
        string operand;
        string expression;
        cout << "First number\n" << endl;
        cin >> firstnum;
        while (operand != "+" && operand != "-") {
            cout << "plus or minus?\n" << endl;
            cin >> operand;
            if (operand != "+" && operand != "-") {
                cout << "you wrote: " << operand << ", an invalid operator \n" << endl;
            }
        }
        cout << "Second number\n" << endl;
        cin >> secondum;

        expression = string(1, firstnum) + operand + string(1, secondum);
        cout << expression << endl;
        const char *msg = expression.c_str();
        send(clientSocket, msg, strlen(msg), 0);
        read(clientSocket, buffer, 1024);
        cout << buffer << endl;
    }

    void manyConnects(int connects) {
        Workers conHandler(connects);
        int tempPort = port;
        conHandler.start();
        while (connects != 0) {
            int tempSocket = socket(PF_INET, SOCK_STREAM, 0);
            cout << tempSocket << endl;
            conHandler.post([tempSocket, tempPort] {
                struct sockaddr_in serverAddr2;
                size_t addr_size2 = sizeof serverAddr2;
                serverAddr2.sin_family = AF_INET;
                //Set port number, using htons function
                serverAddr2.sin_port = htons(tempPort);
                //Set IP address to localhost'
                serverAddr2.sin_addr.s_addr = inet_addr("127.0.0.1");
                memset(serverAddr2.sin_zero, '\0', sizeof serverAddr2.sin_zero);
                char tempBuffer[1024];
                char message[1024];
                char firstnum = '52';
                char secondum = '100';
                string operand = "+";
                string expression;
                expression = string(1, firstnum) + operand + string(1, secondum);
                const char *msg = expression.c_str();
                int constate;
                constate = connect(tempSocket, (struct sockaddr *) &serverAddr2, addr_size2);
                while (constate != 0) {
                    constate = connect(tempSocket, (struct sockaddr *) &serverAddr2, addr_size2);
                }
                //connect(tempSocket, (struct sockaddr *) &serverAddr, addr_size);
                send(tempSocket, msg, strlen(msg), 0);
                recv(tempSocket, message, 1024, 0);
                cout << this_thread::get_id() << endl;
                cout << message << endl;
                close(tempSocket);
            });
            connects--;
        }
        conHandler.stop();


    }
};

int main() {
    Connection con("localhost", 8080);
    // con.basicConnect();
    con.manyConnects(2);
    return 0;
}