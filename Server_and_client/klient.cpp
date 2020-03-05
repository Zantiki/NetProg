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

    void closeCon() {
        close(clientSocket);
    }

    void basicConnect() {
        bool kill = false;
        string input;
        connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
        while (!kill) {
            char buffer[1024];
            int firstnum, secondum;
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

            //expression = firstnum + operand + secondum;
            cout << expression << endl;
            int op = 0;
            if (operand == "+") {
                op = 0;
            } else {
                op = 1;
            }

            int msg[] = {firstnum, secondum, op};
            /*int con = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
            while( send(clientSocket, msg, 1024, 0) < 0 || con != 0){
                cout << con << endl;
                con = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
            };*/
            send(clientSocket, msg, 1024, 0);
            read(clientSocket, buffer, 1024);
            cout << buffer << endl;
            //close(clientSocket);
            cout << "Next mode (y/n)?" << endl;
            cin >> input;
            if (input == "y") kill = true;
        }
    }

    void manyConnects(int connects, int firstnum, int secondum, int op) {
        Workers conHandler(connects);
        int tempPort = port;
        conHandler.start();
        while (connects != 0) {
            int tempSocket = socket(PF_INET, SOCK_STREAM, 0);
            cout << tempSocket << endl;
            conHandler.post([tempSocket, tempPort, firstnum, secondum, op] {
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
                int operand = op;
                string expression;
                // expression = firstnum + operand + secondum;
                int msg[] = {firstnum, secondum, operand};
                int constate;
                constate = connect(tempSocket, (struct sockaddr *) &serverAddr2, addr_size2);
                /*while (constate != 0) {
                    constate = connect(tempSocket, (struct sockaddr *) &serverAddr2, addr_size2);
                }*/
                //connect(tempSocket, (struct sockaddr *) &serverAddr, addr_size);
                send(tempSocket, msg, 1024, 0);
                recv(tempSocket, message, 1024, 0);
                cout << this_thread::get_id() << endl;
                cout << "See answer blow UwU" << endl;
                cout << message << endl;
                close(tempSocket);
            });
            connects--;
        }
        conHandler.stop();


    }
};

int main() {
    Connection con("localhost", 9000);

    int firstnum, secondum;
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

    //expression = firstnum + operand + secondum;
    cout << expression << endl;
    int op = 0;
    if (operand == "+") {
        op = 0;
    } else {
        op = 1;
    }
    con.manyConnects(1, firstnum, secondum, op);
    cout << "\n\n\n\n\n" << endl;
    //con.basicConnect();
    con.manyConnects(10, firstnum, secondum, op);
    return 0;
}