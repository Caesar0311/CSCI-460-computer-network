/**
 * @file ftp_server_test.cpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 24, 2020
 *
 */

#ifndef __FTP_SERVER_TEST_NET_UTIL_HEADER__
#define __FTP_SERVER_TEST_NET_UTIL_HEADER__

#define IP_ADDRESS_LENGTH 16

char*  getTestHostIP();

void getTestListenerSocket(char* port, int& socketDescriptor, struct sockaddr_in* socketAddress, bool& isSuccessful);

void connectToTestRemoteAddress(struct sockaddr_in* remoteAddress, int& socketDescriptor);

void connectToTestRemote(char* remoteIP, int remotePort, int& socketDescriptor);

void acceptTestRemoteConnection(const int listenerSocket, int& clientSocket, struct sockaddr_in* peerAddress);

bool isTestSocketReadyToRead(const int socketDescriptor, struct timeval* timeout, bool& isError, bool& isTimeout);

int sendMessageToTestRemote(const int socketDescriptor, const char* message, const int length);

int receiveMessageFromTestRemote(const int socketDescriptor, char* message, int& length);

void closeTestSocket(int& socketDescriptor);

bool isTestSocketClosed(const int socketDescriptor);

void testPassiveSuccessResponse(char* response, const int passiveListenerSockDescriptor);

void getTestSocketPort(const int socketDescriptor, int& port); 

void getHostIPAndPortFromTestPassiveSuccessResponse(char* response, char* hostIP, int& hostPort);

#endif
