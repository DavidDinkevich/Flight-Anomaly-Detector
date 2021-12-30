#include "Server.h"

Server::~Server() {}

Server::Server(int port) throw (const char *) {
    fileDescriptor = socket(AF_INET,SOCK_STREAM,0);

    if (0 > fileDescriptor) {
        throw "Oh nooo!!!! Failed to create socket :(";
    }
    serverSocket.sin_port = htons(port);
    serverSocket.sin_addr.s_addr = INADDR_ANY;
    serverSocket.sin_family = AF_INET;

    int succStatus = bind(fileDescriptor,(struct sockaddr *) &serverSocket, sizeof(serverSocket));
    if (0 > succStatus)
        throw "Oh nooo!!!! Failed to bind :(";
    succStatus = listen(fileDescriptor, 6);
    if (0 > succStatus) {
        throw "Oh nooo!!!! Failed to listen :(";
    }

}

void Server::start(ClientHandler& ch) throw (const char *) {
    t = new thread([&]() {
        while (!shouldQuit) {
            socklen_t clientSocketSize = sizeof(clientSocket);
            alarm(1); // Schedule alarm for 1 second
            int newSocketFD = accept(fileDescriptor, (struct sockaddr *) &clientSocket, &clientSocketSize);
            alarm(0); // Cancel current alarm
            if (newSocketFD < 0)
                throw "Oh nooo!!! Failed to accept client :(";
            // Handle new client
            ch.handle(newSocketFD);
            close(newSocketFD); // Close socket after handle
        }
        close(fileDescriptor); // Close server socket
    });
}

void Server::stop(){
    shouldQuit = true;
    t->join();
}

