
#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <thread>
#include <unistd.h>

#include "commands.h"
#include "CLI.h"

using namespace std;

// edit your ClientHandler interface here:
class ClientHandler {
public:
    virtual void handle(int clientID) = 0;
};


// you can add helper classes here and implement on the cpp file


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler : public ClientHandler {
public:
    virtual void handle(int clientID){
        SocketIO sio(clientID);
        CLI cli(&sio);
        cli.start();
    }
};


// implement on Server.cpp
class Server {
    thread *t; // the thread to run the start() method in
    sockaddr_in clientSocket;
    sockaddr_in serverSocket;
    bool shouldQuit = false;
    int fileDescriptor;

public:
    Server(int port) throw (const char *);
    virtual ~Server();
    void start(ClientHandler& ch) throw (const char *);
    void stop();
};

#endif /* SERVER_H_ */
