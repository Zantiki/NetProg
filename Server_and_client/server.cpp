//
// Created by darea on 02/03/2020.
//
#include <iostream>
#include "../WorkerThreads.cpp"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
#include <sstream>

using namespace std;

//Todo: fix numberparser
//Todo: buffer reset

class Server {
public:
    int socket1 = socket(AF_INET, SOCK_STREAM, 0);
    int socket2;
    int clients;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    bool kill = false;
    char buffer[1024];

    Server(int threads) {
        clients = threads;
        try {
            // Address family = Internet
            serverAddr.sin_family = AF_INET;
            //Set port number, using htons function to use proper byte order
            serverAddr.sin_port = htons(8080);
            //Set IP address to localhost
            serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            //Set all bits of the padding field to 0
            memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
            //Bind the address struct to the socket
            bind(socket1, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
            cout << "Socket was binded" << endl;

        } catch (const exception e) {
            cout << e.what() << endl;
        }
        //workers(threads);
    }

    void basicCalculation() {
        cout << "Servermode: Basic Calculation" << endl;
        kill = false;
        string input;
        while (!kill) {
            cout << socket2 << endl;
            if (listen(socket1, clients) == 0)
                printf("Listening\n");
            else
                printf("Error\n");
            addr_size = sizeof serverStorage;
            socket2 = accept(socket1, (struct sockaddr *) &serverStorage, &addr_size);
            //cout << socket2 << " Connected" << endl;
            //send(socket2, msg, strlen(msg), 0 );
            int res = read(socket2, buffer, 1024);
            cout << buffer << endl;
            int result = parseExpressionAndCalculate(buffer);

            cout << "Result of " << buffer << "=" << result << endl;
            stringstream temp("");
            temp << result;
            const char *msg = temp.str().c_str();
            send(socket2, msg, strlen(msg), 0);
            close(socket2);
            cout << "Next mode (y/n)?" << endl;
            cin >> input;
            if (input == "y") kill == true;

        }

    }

    int parseExpressionAndCalculate(char data[]) {
        int i = 0;
        int num1, num2;
        string numStr1, numStr2;
        char operand;
        while (data[i] != '' && data[i]) {
            //cout << data[i] << endl;
            if (operand != '+' && operand != '-') { numStr1.push_back(data[i]); }
            if (operand == '+' || operand == '-') { numStr2.push_back(data[i]); }
            if (data[i] == '+' || data[i] == '-') { operand = data[i]; }
            i++;
        }
        stringstream temp(numStr1);
        temp >> num1;
        stringstream temp2(numStr2);
        temp2 >> num2;

        if (operand == '+') {
            return num1 + num2;
        } else {
            return num1 - num2;
        }
    }

    void multipleClients(int clinumber) {
        vector<int> clientSockets;
        Workers clientHandler(clinumber);
        string input;
        clientHandler.start();
        while (!kill) {
            if (listen(socket1, clients) == 0)
                printf("\nListening\n");
            else
                printf("Error\n");
            addr_size = sizeof serverStorage;
            int clientSocket = accept(socket1, (struct sockaddr *) &serverStorage, &addr_size);
            cout << "A new client connected" << endl;
            cout << clientSocket << endl;

            clientHandler.post([clientSocket, this] {
                char tempBuffer[1024];
                int res = read(clientSocket, tempBuffer, 1024);
                // cout << buffer << endl;
                int result = parseExpressionAndCalculate(tempBuffer);
                cout << "----THREAD RESULT----" << endl;
                cout << this_thread::get_id() << endl;
                cout << tempBuffer << endl;
                cout << result << endl;
                stringstream temp("");
                temp << result;
                const char *msg = temp.str().c_str();
                send(clientSocket, msg, strlen(msg), 0);
                close(clientSocket);
            });
            //cout << socket2 << " Connected" << endl;
            //send(socket2, msg, strlen(msg), 0 );
            // send(socket2, msg, strlen(msg), 0 );
        }
        clientHandler.stop();
        kill = true;
    }

    void httpResponse() {
        cout << "Servermode: httpResponse" << endl;
        kill = false;
        string input;
        while (!kill) {
            if (listen(socket1, clients) == 0)
                printf("Listening\n");
            else
                printf("Error\n");
            addr_size = sizeof serverStorage;
            socket2 = accept(socket1, (struct sockaddr *) &serverStorage, &addr_size);
            //cout << socket2 << " Connected" << endl;
            int res = read(socket2, buffer, 1024);
            string msg = "HTTP/1.0 200 OK \n"
                         "Content-Type: text/html; charset=utf-8 \n"
                         "\n"
                         "<HTML><BODY>\n"
                         "<H1> Hilsen. Du har koblet deg opp til min enkle web-tjener </h1>\n"
                         "Header fra klient er:\n"
                         "<UL>\n"
                         "<LI> " + string(buffer, strlen(buffer)) + "</LI>\n"
                                                                    "</UL>\n"
                                                                    "</BODY></HTML>";
            cout << "Error here" << endl;
            const char *msgChar = msg.c_str();
            cout << "Error here" << endl;
            send(socket2, msgChar, strlen(msgChar), 0);
            close(socket2);
            cout << buffer << endl;
            cout << "Next mode (y/n)?" << endl;
            cin >> input;

            if (input == "y") kill == true;
        }

    }

    void stop() {

    }

};


int main() {
    Server test(1);
    //test.basicCalculation();
    test.multipleClients(2);
    // test.httpResponse();
    return 0;
}
