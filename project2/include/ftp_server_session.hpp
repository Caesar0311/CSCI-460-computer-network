/**
 * @file ftp_server_session.hpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 * 
 */

#ifndef __FTP_SERVER_SESSION_HEADER__
#define __FTP_SERVER_SESSION_HEADER__

#define FTP_CLIENT_SESSION_TIMEOUT_SEC 600 //10 minutes
#define FTP_CLIENT_SESSION_TIMEOUT_USEC 0

struct ClientFtpSession {
	int controlConnection = -1;
	int dataConnection = -1;
	int dataListener = -1;
	bool isControlConnected = false;
	bool isDataConnected = false;
	bool isDataListener = false;
	bool isValidUser = false;
	bool isLoggedIn = false;
	char* rootDir = nullptr;
};


void startClientFTPSession(ClientFtpSession& clientFtpSession);
//Send a connected response to the client using 'controlSockDescriptor'.
//Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send response to the client.
//Start a client FTP session against a client control connection represented by 'controlSockDescriptor' by taking following actions.
//	Keep track of the state of the client session using a local variable, say 'clientFtpSession'  of type 'ClientFtpSession'.
//	Set 'controlSockDescriptor' to 'clientFtpSession.controlConnection' field.
//	Set 'true' to 'clientFtpSession.isControlConnected' field.
//	Determine the root directory of the ftp server application and assing it to 'clientFtpSession.rootDir' field.
//	Do the followings as long as the client is connected.
//		Wait for client's FTP request for FTP_CLIENT_SESSION_TIMEOUT_SEC + 0.000001xFTP_CLIENT_SESSION_TIMEOUT_USEC time
//		calling 'isConnectionReadyToRead()' function from 'ftp_server_connection.hpp'.
//		If a request comes before the timeout, read the request.
//		Interpret the request, take appropriate action, and sends appropriate response to the client by calling
//		'interpreteFtpRequest()' function from 'ftp_server.request.hpp'.
//		If timeout or error happens before a FTP request comes, send an appropriate response to the client
//		then stop client FTP session calling 'stopClientFTPSession()' function.


void stopClientFTPSession(ClientFtpSession& clientFtpSession);
//Close both the control and data connections of 'clientFtpSession' using 'closeConnection()' function from 'ftp_server_connection'.
//Close data listener of 'clientFtpSession' similarly.
//Set '-1' to all the fields related to the connection or the listener of client FTP session'.
//Set 'false' to all the boolean flags of 'clientFtpSession'.
//Set 'NULL' to 'rootDir' of 'clientFtpSession'.



#endif
