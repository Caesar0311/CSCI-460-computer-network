/**
 *  @file ftp_client_connection.hpp
 *  @author Humayun Kabir, Instructor, CSCI 460, VIU
 *  @version 1.0.0
 *  @date June 22, 2020
 */

#ifndef __FTP_CLIENT_CONNECTION_HEADER__
#define __FTP_CLIENT_CONNECTION_HEADER__

void connectToServer(int& socketDescriptor, bool& isConnected, const char* serverIP, int serverPort);
// Sends a connection request on severPort of a sever whose IP address is equal to serverIP.
// If a connection has been established as a result of the request sets the connection descriptor value 
// to reference 'socketDescriptor' and sets reference 'isConnected' to 'true'.

void disconnectFromServer(int& socketDescriptor, bool& isConnected);
// Closes network connection represented by reference 'socketDescriptor' and
// sets reference 'isConnected' to 'false'.

int sendToServer(int sockDescriptor, const char* message, int messageLength);
// Sends 'message' of length 'messageLength' bytes to the server 
// on the network connection represented by 'sockDescriptor'.

int receiveFromServer(int sockDescriptor, char* message, int messageLength);
// Receives 'message' of length 'messageLength' bytes from the server 
// on the network connection represented by 'sockDescriptor'.

#endif
