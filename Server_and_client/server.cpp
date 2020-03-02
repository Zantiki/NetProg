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

using namespace std;

//TODO: Hva gjør vi med mtx????

class Server {
public:
    Workers taskHandlers = Workers(10);
    Workers eventListener = Workers(1);
    int socket1 = socket(AF_INET, SOCK_STREAM, 0);
    int socket2;
    int clients;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    bool kill = false;

    Server(int threads) {
        clients = threads;
        /*taskHandlers(threads);
        eventListener(1);*/
        try {
            // Address family = Internet
            serverAddr.sin_family = AF_INET;
            //Set port number, using htons function to use proper byte order
            serverAddr.sin_port = htons(7799);
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

    void start() {
        eventListener.start();
        eventListener.post([this] {
            cout << "kill server?"
            while (!this.kill) {
                cin >> input;
                if (input == "y" || input == "yes") {
                    this.kill = true;
                    this.stop();
                }
            }
        })
        taskHandlers.start();

        if (listen(socket1, clients) == 0)
            printf("Listening\n");
        else
            printf("Error\n");
        int i = 0;
        while (!kill) {

            addr_size = sizeof serverStorage;
            socket2 = accept(socket1, (struct sockaddr *) &serverStorage, &addr_size);

            taskHandlers.post([&socket2] {
                cout << "A new client was added" << endl;
            })
        }

        return 0;
    }


    void stop() {
        taskHandlers.stop()
        eventListener.stop();
    }

};


int main() {
    Server test();
    return 0;
}
