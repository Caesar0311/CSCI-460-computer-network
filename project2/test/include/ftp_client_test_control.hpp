/**
 * @file ftp_client_control.cpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 24, 2020
 *
 */


#ifndef FTP_CLIENT_CONTROL_H
#define FTP_CLIENT_CONTROL_H

#define FTP_CLIENT_DEAFULT_SERVER_IP "127.0.0.1"
#define FTP_CLIENT_DEFAULT_SERVER_PORT 21
#define BUFFER_SIZE 512

/**
 * Implemention of this header file will require to declare some variables
 * as follows:
 */
// int controlSocket = -1;
// int dataSocket = -1;
// bool isControlConnected = false; 
// bool isDataConnected = false;

void connectToServerControl(const char* serverIP, int serverPort);
// Sends a control connection request on severPort of a sever whose IP address is equal to serverIP.
// If the control connection is successful sets its value to controlSocket and 
// sets isControlConnected to true.

void connectToServerData(const char* serverIP, int serverPort);
// Sends a data connection request on severPort of a sever whose IP address is equal to serverIP.
// If the data connection is successful sets its value to dataSocket and 
// sets isDataConnected to true.

int sendOnControl(const char* buffer, int length);
// Sends the info in the buffer of length equal to 'length' on control connection.
// Returns the actual number of bytes sent.

int sendOnData(const char* buffer, int length);
// Sends the info in the buffer of length equal to 'length' on data connection.
// Returns the actual number of bytes sent.

int receiveOnControl(char* buffer, int length);
// Receives the info in the buffer of length equal to 'length' on control connection.
// Returns the actual number of bytes received.

int receiveOnData(char* buffer, int length);
// Receives the info in the buffer of length equal to 'length' on data connection.
// Returns the actual number of bytes received.

void disconnectControlConnection();
// Closes the opened control connection and sets isControlConnected to false;

void disconnectDataConnection();
// Closes the opened data connection and sets isDataConnected to false;

bool isControlConnectionAvailable();
// Returns the value of isControlConnected.

bool isDataConnectionAvailable();
// Returns the value of isDataConnected.

#endif
