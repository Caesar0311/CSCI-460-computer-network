/**
 * @file ftp_server_passive.hpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 * 
 */

#ifndef __FTP_SERVER_PASSIVE_HEADER__
#define __FTP_SERVER_PASSIVE_HEADER__

#include "ftp_server_session.hpp"


#define PASSIVE_DEFAULT_PORT "0"
#define DATA_CONNECTION_TIME_OUT_SEC 20
#define DATA_CONNECTION_TIME_OUT_USEC 0

void enteringIntoPassive(ClientFtpSession& clientFtpSession);
//Start a passive connection listener by calling 'startPassiveListener()' function.
//Once successful on the above function call, create a passive success response calling 'createPassiveSuccessResponse()'
//function and send the passive success response to the client on the control connection represented by 'controlSockDescriptor'.
//Use 'sendToRemote()' function from 'ftp_server_connection.hpp' to send the passive success response to the client.
//Wait for DATA_CONNECTION_TIME_OUT_SEC and DATA_CONNECTION_TIME_OUT_USEC time to get a client's connection request 
//on the listener by calling isListenerSocketReady() from 'ftp_server_connection_listener.hpp'.
//Accept client's connection request, if there is any, and opens a data connection with the client by calling
//'acceptClientConnetion() function from 'ftp_server_connection_listener.hpp'.
//Close the connection listener after opening the data connection by calling 'closeListenerSocket()'
//function from 'ftp_server_connection_listener.hpp'.
//Send an appropriate response to the client using control connection represented by 'controlSockDescriptor'
//using 'sendToRemote()' function from 'ftp_server_connection.hpp'.


void startPassiveListener(ClientFtpSession& clientFtpSession, bool& succeded);
//Start a passive listener socket that can listen connection requests from the remote computer.
//by calling 'startListenerSocket()' function from 'ftp_server_connection_listener.hpp'.


void stopPassiveListener(ClientFtpSession& clientFtpSession);


void createPassiveSuccessResponse(char* response, const int passiveListenerSockDescriptor);
//Determine the passive listener port number from 'passiveListenerSockDescriptor' by calling
//'getPortFromSocketDescriptor()' function from 'ftp_server_net_util.hpp'.
//Determine the local IP address by calling 'getHostIPAddress()' function from 'ftp_server_net_util.hpp'.
//Includes both the IP address and the port number into passive success response according to RFC.    

#endif
