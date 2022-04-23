/**
 * @file ftp_server_session.cpp
 * @author Your Name, Student Number, Section, CSCI 460, VIU
 * @version 1.0.0
 * @date Date you have last modified your code in this file, e.g., August 05, 2021
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */



//Include required library and custom header files.
#include <string>
#include <cstring>
#include "ftp_server_session.hpp"
#include "ftp_server_connection.hpp"
#include "ftp_server_request.hpp"
#include "ftp_server_response.hpp"
#include "ftp_server_net_util.hpp"
#include "ftp_server_nlist.hpp"
//#includ
using namespace std;
//Implement all the functions prototyped in the header file ftp_server_session.hpp


void startClientFTPSession(ClientFtpSession& clientFtpSession)
{
    sendToRemote(clientFtpSession.controlConnection, CONNECTED_RESPONSE, strlen(CONNECTED_RESPONSE));
    clientFtpSession.isControlConnected = true;

    while(clientFtpSession.isControlConnected) {
        bool isError = false,isTimedout = false;
        if (isConnectionReadyToRead(clientFtpSession.controlConnection, FTP_CLIENT_SESSION_TIMEOUT_SEC, FTP_CLIENT_SESSION_TIMEOUT_USEC,
                                     isError, isTimedout)) {
            char message[FTP_REQUEST_BUFFER_SIZE];
            int size = read(clientFtpSession.controlConnection, message, 1024);
            if (size > 0) {
                message[size] = '\0';
            }
            interpretFtpRequest(message, clientFtpSession);
        }
        else
        {
            sendToRemote(clientFtpSession.controlConnection, CONNECTION_RESET_BY_PEER, strlen(CONNECTION_RESET_BY_PEER));
            stopClientFTPSession(clientFtpSession);
        }
    }
}
//Send a connected response to the client using 'controlSockDescriptor'.
//Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send response to the client.

//Start a client FTP session against a client control connection represented by 'controlSockDescriptor' by taking following actions.
//	Keep track of the state of the client session using a local variable, say 'clientFtpSession'  of type 'ClientFtpSession'.
//	Set 'controlSockDescriptor' to 'clientFtpSession.controlConnection' field.
//	Set 'true' to 'clientFtpSession.isControlConnected' field.
//	Determine the root directory of the ftp server application and assing it to 'clientFtpSession.rootDir' field.

//	Do the followings as long as the client is connected.
//		Wait for client's FTP request for FTP_CLIENT_SESSION_TIMEOUT_SEC + 0.000001xFTP_CLIENT_SESSION_TIMEOUT_USEC time by
//		calling 'isConnectionReadyToRead()' function from 'ftp_server_connection.hpp'.

//		If a request comes before the timeout, read the request.

//		Interpret the request, take appropriate action, and sends appropriate response to the client by calling
//		'interpreteFtpRequest()' function from 'ftp_server.request.hpp'.

//		If timeout or error happens before a FTP request comes, send an appropriate response to the client
//		then stop client FTP session calling 'stopClientFTPSession()' function.


void stopClientFTPSession(ClientFtpSession& clientFtpSession)
{
    closeConnection(clientFtpSession.controlConnection);
    closeConnection(clientFtpSession.dataConnection);
    clientFtpSession.dataListener = -1;
    clientFtpSession.isControlConnected = false;
    clientFtpSession.isDataConnected = false;
    clientFtpSession.isDataListener = false;
    clientFtpSession.isLoggedIn = false;
    clientFtpSession.isValidUser = false;
    clientFtpSession.rootDir = nullptr;

}
//Close both the control and data connections of 'clientFtpSession' using 'closeConnection()' function from 'ftp_server_connection'.
//Close data listener of 'clientFtpSession' similarly.
//Set '-1' to all the fields related to the connection or the listener of client FTP session'.
//Set 'false' to all the boolean flags of 'clientFtpSession'.
//Set 'NULL' to 'rootDir' of 'clientFtpSession'.

