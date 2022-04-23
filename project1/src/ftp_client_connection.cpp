/**
 * @file ftp_client_connection.cpp
 * @author Your Name, Student Number, Section, CSCI 460, VIU
 * @version 1.0.0
 * @date Date you have last modified your code in this file, e.g., August 05, 2021
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */



//Include required library and custom header files.

#include "../include/ftp_client_session.hpp"
#include "ftp_client_connection.hpp"
#include "ftp_server_response.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>


//Implement all the functions prototyped in the header file ftp_client_connection.hpp

char *getHostIPAddress() {
    return nullptr;
}

//connect to server using specified serverIP and serverPort
//if successfully connected, set isConnected flag to true
//here printed teh received message
void connectToServer(int &socketDescriptor, bool &isConnected, const char *serverIP, int serverPort) {
    struct sockaddr_in addr{};
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(serverPort);
    addr.sin_addr.s_addr = inet_addr(serverIP);
    bzero(&(addr.sin_zero), 8);
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    auto res = connect(socketDescriptor, (struct sockaddr *) &addr, sizeof(struct sockaddr));
    if (res != -1) {
        char recvMessage[FTP_RESPONSE_MAX_LENGTH];
        isConnected = true;
        int recvSize = receiveFromServer(socketDescriptor, recvMessage, sizeof(recvMessage));
        recvMessage[recvSize] = '\0';
        if (recvSize > 0) {
            std::cout << recvMessage << std::endl;
        }
    }
}

// Closes network connection represented by reference 'socketDescriptor' and
// sets reference 'isConnected' to 'false'.
void disconnectFromServer(int &socketDescriptor, bool &isConnected) {
    close(socketDescriptor);
    isConnected = false;
}

// Sends 'message' of length 'messageLength' bytes to the server
// on the network connection represented by 'sockDescriptor'.
int sendToServer(int sockDescriptor, const char *message, int messageLength) {
    int res = write(sockDescriptor, message, messageLength);
    if (res < 0) {
        std::cout << "error" << std::endl;
        return -1;
    }
    return res;
}

// Receives 'message' of length 'messageLength' bytes from the server
// on the network connection represented by 'sockDescriptor'.
int receiveFromServer(int sockDescriptor, char *message, int messageLength) {
    int res = read(sockDescriptor, message, messageLength);
    if (res <= 0) {
        return -1;
    }
    return res;
}

