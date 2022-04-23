/**
 * @file ftp_client_session.cpp
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
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <string>
#include <list>
#include <cassert>
#include <stdlib.h>
#include <cstring>

//Implement all the functions prototyped in the header file ftp_client_session.hpp

//connects to server using specified serverIP and serverPort, if success,
//the socket descriptor will be recorded in clientFtpSession.controlConnection,
//the clientFtpSession.isControlConnected will be set to true
void startClientFTPSession(const char *serverIP, int serverPort, ClientFtpSession &clientFtpSession) {
    connectToServer(clientFtpSession.controlConnection, clientFtpSession.isControlConnected,
                    serverIP, serverPort);

    if (!clientFtpSession.isControlConnected) {
        perror("not connected!\n");
    }
}

//disconnect from server and close socket descriptor and set isDataConnnect to false,
//the same to isControlConnected
void stopClientFTPSession(ClientFtpSession &clientFtpSession) {
    disconnectFromServer(clientFtpSession.dataConnection, clientFtpSession.isDataConnected);
    disconnectFromServer(clientFtpSession.controlConnection, clientFtpSession.isControlConnected);
}



































