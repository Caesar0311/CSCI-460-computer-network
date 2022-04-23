/**
 * @file ftp_client_command.cpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 24, 2020
 *
 */

#ifndef FTP_CLIENT_COMMAND_H
#define FTP_CLIENT_COMMAND_H

#define FTP_COMMAND_MAX_LENGTH 512

#define FTP_CLIENT_USER_COMMAND_HELP "help"
#define FTP_CLIENT_USER_COMMAND_USER "user"
#define FTP_CLIENT_USER_COMMAND_PASSWORD "pass"
#define FTP_CLIENT_USER_COMMAND_DIRECTORY "dir"
#define FTP_CLIENT_USER_COMMAND_PRINT_DIRECTORY "pwd"
#define FTP_CLIENT_USER_COMMAND_CHANGE_DIRECTORY "cwd"
#define FTP_CLIENT_USER_COMMAND_CHANGE_DIRECTORY_UP "cdup"
#define FTP_CLIENT_USER_COMMAND_GET "get"
#define FTP_CLIENT_USER_COMMAND_QUIT "quit"


#define FILE_OPEN_MODE "a"
#define DATA_SOCKET_RECEIVE_BUFFER_SIZE 65536 //64K

#define MAX_NUMBER_OF_RESPONSES 5

using namespace std;


void handleCommandUser(string username, string serverResponses[], int& serverResponseCount);
// Sends a 'USER <username>' request meesage to the FTP Server.
// Receives response from FTP Server.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively. 
// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter true 
// to perform the activities mentioned above.

void handleCommandPassword(string password, string serverResponses[], int& serverResponseCount);
// Sends a 'PASS <password>' request message to the FTP Server.
// Receives response from FTP Server.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively. 
// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter true 
// to perform the activities mentioned above.

void handleCommandDirectory(string serverResponses[], int& serverResponseCount);
// First sends a 'PASV' request message to the FTP server.
// Receives the response against PASV request message from the server.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// If the response is a successful one, retrives data-connection listener port number form the response.
// Requests a data connection to the server on the listener port at the server.
// Calls function handlePassive() to perform the activities mentioned above. 
// If the data connection is successful, sends a 'NLST' request message to the server on the control connection.
// Receives the response against NLST request from the server on the control connection.
// If the response is successful, retrieves the list of entries in server's current directory on the data connection.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// Calls function handleNLIST() to perform the activities mentioned above. 

void handleCommandPrintDirectory(string serverResponses[], int& serverResponseCount);
// Sends a 'PWD' request message to FTP Server.
// Receives the response from the server.
// Successful server response contains the current FTP directory at the server.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter false 
// to perform the activities mentioned above.

void handleCommandChangeDirectory(string path, string serverResponses[], int& serverResponseCount);
// Sends a 'CWD <path>' request message to the FTP Server.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter false 
// to perform the activities mentioned above.

void handleCommandChangeDirectoryUp(string serverResponses[], int& serverResponseCount);
// Sends a 'CDUP' request message to FTP Server.
// Receives the response from the server.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter false 
// to perform the activities mentioned above.

void handleCommandGetFile(string filename, string serverResponses[], int& serverResponseCount);
// First sends a 'PASV' request message to the FTP server.
// Receives the response against PASV request message from the server.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// If the response is a successful one, retrives data-connection listener port number form the response.
// Requests a data connection to the server on the listener port at the server.
// Calls function handlePassive() to perform the activities mentioned above. 
// If the data connection is successful, sends a 'RETR <filename>' request message 
// to the server on the control connection.
// Receives the response against RETR request from the server on the control connection.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// If the response is successful, retrieves the content of the file on the data connection.
// Saves the file in the local directory at the client computer.
// Calls function handleRETR() to perform the activities mentioned above. 

void handleCommandQuit(string serverResponses[], int& serverResponseCount);
// Sends a 'QUIT' request message to FTP Server.
// Calls function handleSimpleCommandResponse() with "checkAuthentication" parameter false 
// to send 'QUIT' request message.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// Quits from the application.

void handlePassive( string serverResponses[], int& serverResponseCount);
// Sends a 'PASV' request message to the FTP server.
// Receives the response against PASV request message from the server.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// If the response is a successful one, retrives data-connection listener port number form the response.
// Retrives data-connection listener port number form the response 
// using function getHostIPAndPortFromPassiveSuccessResponse().
// Requests a data connection to the server on the listener port at the server.

void handleNLIST(string serverResponses[], int& serverResponseCount);
// Sends a 'NLST' request message to the server on the control connection.
// Receives the response against NLST request from the server on the control connection.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// If the response is successful, retrieves the list of entries in server's current directory 
// on the data connection.
// Returns the  list of directory entries as the server responses through 'serverResponses' and 'serverResponseCount' parameters.

void handleRETR(string filename, string serverResponses[], int& serverResponseCount);
// Sends a 'RETR <filename>' request message to the server on the control connection.
// Receives the response against RETR request from the server on the control connection.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// If the response is successful, retrieves the content of the file on the data connection.
// Saves the file in the local directory at the client computer.

void handleSimpleCommand(string ftpCommand, bool checkAuthentication, string serverResponses[], int& serverResponseCount);
// Sends 'ftpCommand' request message to FTP server on the control connection.
// Receives the response from the server against the request.
// Returns server responses and response count through 'serverResponses' and 'serverResponseCount' parameters respectively.
// If the response is unsuccessful and checkAuthentication parameter value is true, quits the application.

void getHostIPAndPortFromPassiveSuccessResponse(char* response, char* hostIP, int& hostPort);
// Retrieves IP address of FTP Server from the response string into reference 'hostIP'.
// Retrives data-connection listening port number of FTP server from the response string into reference 'hostPort'. 
// The example of a successful response message is "227 Entering Passive Mode (192,168,1,65,202,143)."
// From the above response message 'hostIP' will be 192.168.1.65 and 'hostPort' will be
// (202x256 + 143) or 51855.


#endif
