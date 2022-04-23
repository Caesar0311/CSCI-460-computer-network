/**
 * @file ftp_server_request.hpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 * 
 */


#ifndef __FTP_SERVER_FTP_REQUEST_HEADER__
#define __FTP_SERVER_FTP_REQUEST_HEADER__

#include "ftp_server_session.hpp"

#define FTP_REQUEST_BUFFER_SIZE 512
#define FTP_REQUEST_CODE_CHARACTER_COUNT 5
#define FTP_REQUEST_ARG_MAX_CHARACTER_COUNT 256
#define FTP_REQUEST_DELIMITER " "

#define DEFAULT_USERNAME "csci460"
#define DEFAULT_USERNAME_LENGTH 7
#define DEFAULT_PASSWORD "460pass"
#define DEFAULT_PASSWORD_LENGTH 7


#define FTP_REQUEST_USER "USER"
#define FTP_REQUEST_PASSWORD "PASS"
#define FTP_REQUEST_PWD "PWD"
#define FTP_REQUEST_CWD "CWD"
#define FTP_REQUEST_CDUP "CDUP"
#define FTP_REQUEST_PASV "PASV"
#define FTP_REQUEST_NLST "NLST"
#define FTP_REQUEST_RETR "RETR"
#define FTP_REQUEST_QUIT "QUIT"


#define PATH_NOT_ALLOWED_CURERENT_DIRECTORY "./"
#define PATH_NOT_ALLOWED_PARENT_DIRECTORY "../"


void interpretFtpRequest(const char* ftpRequest, ClientFtpSession& clientFtpSession);
//Separate the command and its argument calling 'parseFtpRequest()' function. Make sure your 'requestName' and 'requestArgument'
//parameters are valid pointers with appropriate size of memory allocated.
//Determine, which valid FTP request has been sent, only the requests defined in this header file are valid for this FTP server.
//Call appropriate 'handleFtpRequestXXXX()' function to handle a valid request.
//Call 'handleFtpRequestUnSupported()' if an invalid request has been received.


void parseFtpRequest(const char* ftpRequest, char* requestName, char* requestArgument);
//Break the 'ftpRequest' string into its parts: request name and request argument.
//Copy the request name part to reference parameter 'requestName' and the request argument part to reference parameter 'requestArgument'.
//Caller of this function will retrieve the request parts through these reference parameters.


void handleFtpRequestUSER(const char* username, ClientFtpSession& clientFtpSession);
//Handle USER command by comparing 'username' with the DEFAULT_USERNAME.
//If the 'username' matches, set 'true' to 'isUser' and send an appropriate response to the client using the
//control connection represented by 'controlSockDescriptor'.
///Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
//If the 'username' does not match, send an appropriate response to the client and closes all connections
//calling 'closeAllConnections()' function from 'ftp_server.connection.hpp'.

void handleFtpRequestPASS(const char* password, ClientFtpSession& clientFtpSession);
//Check whether the USER command was successful or not before this PASS command by checking the value in 'isUser'.
//A 'false' value in 'isUser' means USER command has not been successful before this PASS command, 
//send appropriate response to the client and closes all connections by calling 'handleNotLoggedIn()' function.
//Compare 'password' with the DEFAULT_PASSWORD.
//If the password does not match, send appropriate response to the client and closes all connections 
//by calling ''handleNotLoggedIn()' function.
//If the password matches, sets 'true' to 'isLoogedIn' and sends appropriate response to the client on the control connection
//represented by 'controlSockDescriptor'..
//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleFtpRequestPWD(ClientFtpSession& clientFtpSession);
//Check whether the client is logged in or not by checking the value in 'isLoggedIn'.
//If the client is not logged in, send an appropriate response to the client and closes all the connections 
//by calling 'handleNotLoggedIn()' function.
//If the client is logged in, determines the current working directory sends it to the client in an appropriate response.
//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleFtpRequestCWD(const char* directory, ClientFtpSession& clientFtpSession);
//Check whether the client is logged in or not by checking the value in 'isLoggedIn'.
//If the client is not logged in, send an appropriate response to the client and closes all the connections 
//by calling 'handleNotLoggedIn()' function.
//If the client is logged in, determine whether the requested 'directory' is valid or not.
//A requested directory is not valid if any of the following is true
//	It is not a subdirectory of the current working directory
//	It starts with "./" or "../"
//	It contains "/.", "/..", or "*"
//If the requested directory is not a valid directory, send an appropriate response to the client.
//If the requested directory is valid, change the current directory to the requested directory and sends 
//an appropriate response to the client.
//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleFtpRequestCDUP(ClientFtpSession& clientFtpSession);
//Check whether the client is logged in or not by checking the value in 'isLoggedIn'.
//If the client is not logged in, send an appropriate response to the client and closes all the connections 
//by calling 'handleNotLoggedIn()' function.
//If the client is logged in, determine whether moving up to the parent directory is not falling beyond 
//the applications's root directory.
//Application's root directory is passed in the last parameter 'rootDir'.
//If the move does not go beyond application's root directory, change the current working directory 
//to the parent directory and send an appropriate response to the client.
//If the the move goes beyond application's root directory, send an appropriate response to the client.
//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleFtpRequestPASV(ClientFtpSession& clientFtpSession);
//Check whether the client is logged in or not by checking the value in 'isLoggedIn'.
//If the client is not logged in, send an appropriate response to the client and closes all the connections 
//by calling 'handleNotLoggedIn()' function.
//If the client is logged in, enter into passive mode by calling 'enteringIntoPassive()' function from 'ftp_server_passive.hpp'
//'enteringIntoPassive()' function starts a passive connection listener, sends an appropriate response to the client.
//'enteringIntoPassive()' function waits for a specific time interval for the client to send a connection request on the listener.
//'enteringIntoPassive()' function accepts client's connection request, if there is any, and opens a data connection with the client.
//'enteringIntoPassive()' function closes the connection listener after opening the data connection with the client.
//'enteringIntoPassive()' function also sends appropriate response to the client using control connection.

void handleFtpRequestNLST(ClientFtpSession& clientFtpSession);
//Check whether the client is logged in or not by checking the value in 'isLoggedIn'.
//If the client is not logged in, send an appropriate response to the client and closes all the connections 
//by calling 'handleNotLoggedIn()' function.
//If the client is logged in, it check whether the data connection has already been established by 
//a PASV command before this NLST command.
//If a data connection has already been established 'dataSockDescriptor' parameter should have a non-negative value.
//If a data connection has not been established, send an appropriate response to the user.
//If a data connection has already been established, prepare and sends the list of current 
//directory entries by calling 'listDirEntries()' function
//from 'ftp_server_nlist.hpp'. 
//'listDirEntries()' function sends the list of current directory entries using the data connection
//and returns the count of entries.
//Send the count of the entries in an appropriate response message to the client using the control connection.
//Close the data connection.  
//Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleFtpRequestRETR(const char* file, ClientFtpSession& clientFtpSession);
//Check whether the client is logged in or not by checking the value in 'isLoggedIn'.
//If the client is not logged in, send an appropriate response to the client and closes all the connections 
//by calling 'handleNotLoggedIn()' function.
//If the client is logged in, it check whether the data connection has already been established by a PASV 
//command before this RETR command.
//If a data connection has already been established 'dataSockDescriptor' parameter should have a non-negative value.
//If a data connection has not been established, send an appropriate response to the user.
//If a data connection has already been established, read from the file and send the content of the file by 
//calling 'sendFile()' function
//from 'ftp_server_retrieve.hpp'. 
//'sendFile()' function sends the content of the file using the data connection
//and returns the count of the bytes sent.
//Sends the count of the bytes sent in an appropriate response message to the client using the control connection.
//Closes the data connection.  
//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleFtpRequestQUIT(ClientFtpSession& clientFtpSession);
//Handle QUIT command by sending appropriate response to the client.
//Close all connections by calling 'closeAllConnections()' function from 'ftp_server.connection.hpp'.
//Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleFtpRequestUnSupported(ClientFtpSession& clientFtpSession);
//Send an appropriate response saying the requested command is not supported by this FTP server.
//Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleNotLoggedIn(ClientFtpSession& clientFtpSession);
//Send an appropriate response saying the user is not logged in.
//Close all connections by calling 'closeAllConnections()' function from 'ftp_server.connection.hpp'.
//Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

#endif
