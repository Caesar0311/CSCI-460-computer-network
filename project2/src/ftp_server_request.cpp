/**
 * @file ftp_server_request.cpp
 * @author Caesar, 578751737, N02, CSCI 460, VIU
 * @version 1.0.0
 * @date Nov 20,2020

 *
 */



//Include required library and custom header files.
#include <string>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include "ftp_server_request.hpp"
#include "ftp_server_connection.hpp"
#include "ftp_server_passive.hpp"
#include "ftp_server_nlist.hpp"
#include "ftp_server_retrieve.hpp"
#include "ftp_server_response.hpp"
#include "ftp_server_string_util.hpp"
using namespace std;
//Implement all the functions prototyped in the header file ftp_server_request.hpp

void interpretFtpRequest(const char* ftpRequest, ClientFtpSession& clientFtpSession)
{
    char requestName[100], requestArgument[100];
    parseFtpRequest(ftpRequest, requestName, requestArgument);


    if (strcmp(requestName, "USER") == 0) {
        handleFtpRequestUSER(requestArgument, clientFtpSession);
    } else if (strcmp(requestName, "PASS") == 0) {
        if (requestArgument == nullptr) {
            sendToRemote(clientFtpSession.controlConnection, INVALID_NUMBER_OF_ARGUMENTS_RESPONSE, strlen(INVALID_NUMBER_OF_ARGUMENTS_RESPONSE));
        }
        handleFtpRequestPASS(requestArgument, clientFtpSession);
    } else if (strcmp(requestName, "PWD") == 0) {
        handleFtpRequestPWD(clientFtpSession);
    } else if (strcmp(requestName, "CWD") == 0) {
        if (requestArgument == nullptr) {
            sendToRemote(clientFtpSession.controlConnection, INVALID_NUMBER_OF_ARGUMENTS_RESPONSE, strlen(INVALID_NUMBER_OF_ARGUMENTS_RESPONSE));
        }
        handleFtpRequestCWD(requestArgument, clientFtpSession);
    } else if (strcmp(requestName, "CDUP") == 0) {
        handleFtpRequestCDUP(clientFtpSession);
    } else if (strcmp(requestName, "PASV") == 0) {
        handleFtpRequestPASV(clientFtpSession);
    }else if (strcmp(requestName, "NLST") == 0) {
        handleFtpRequestNLST(clientFtpSession);
    } else if (strcmp(requestName, "RETR") == 0) {
        if (requestArgument == nullptr) {
            sendToRemote(clientFtpSession.controlConnection, INVALID_NUMBER_OF_ARGUMENTS_RESPONSE, strlen(INVALID_NUMBER_OF_ARGUMENTS_RESPONSE));
        }
        handleFtpRequestRETR(requestArgument, clientFtpSession);
    } else if (strcmp(requestName, "QUIT") == 0) {
        handleFtpRequestQUIT(clientFtpSession);
    } else {
        handleFtpRequestUnSupported(clientFtpSession);
    }
}
//Separate the command and its argument calling 'parseFtpRequest()' function. Make sure your 'requestName' and 'requestArgument'
//parameters are valid pointers with appropriate size of memory allocated.
//Determine, which valid FTP request has been sent, only the requests defined in this header file are valid for this FTP server.
//Call appropriate 'handleFtpRequestXXXX()' function to handle a valid request.
//Call 'handleFtpRequestUnSupported()' if an invalid request has been received.


void parseFtpRequest(const char* ftpRequest, char* requestName, char* requestArgument)
{
    string tmp = ftpRequest;
    char* tmpCharP = (char*)tmp.c_str();
    stripNewlineAtEnd(tmpCharP);
    stripLeadingAndTrailingSpaces(tmpCharP);
    tmp = tmpCharP;
    auto firstSpacePos = tmp.find_first_of(' ');
    auto lastSpacePos = tmp.find_last_of(' ');
    if (firstSpacePos == -1) {
        auto name = tmp.substr(0, firstSpacePos);
        strcpy(requestName, name.c_str());
        toUpper(requestName);
        requestArgument = nullptr;
    } else {
        auto name = tmp.substr(0, firstSpacePos);
        auto argument = tmp.substr(lastSpacePos + 1);
        strcpy(requestName, name.c_str());
        strcpy(requestArgument, argument.c_str());
        toUpper(requestName);
    }
}
//Break the 'ftpRequest' string into its parts: request name and request argument.
//Copy the request name part to reference parameter 'requestName' and the request argument part to reference parameter 'requestArgument'.
//Caller of this function will retrieve the request parts through these reference parameters.


void handleFtpRequestUSER(const char* username, ClientFtpSession& clientFtpSession)
{
    if (strcmp(username, DEFAULT_USERNAME) == 0) {
        clientFtpSession.isValidUser = true;
        sendToRemote(clientFtpSession.controlConnection, USERNAME_OK_RESPONSE, strlen(USERNAME_OK_RESPONSE));
    }
    else {
        cout << username << endl;
        sendToRemote(clientFtpSession.controlConnection, INVALID_USERNAME_RESPONSE, strlen(INVALID_USERNAME_RESPONSE));
        closeConnection(clientFtpSession.controlConnection);
        clientFtpSession.isControlConnected = false;
    }
}
//Handle USER command by comparing 'username' with the DEFAULT_USERNAME.
//If the 'username' matches, set 'true' to 'isUser' and send an appropriate response to the client using the
//control connection represented by 'controlSockDescriptor'.
//Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.
//If the 'username' does not match, send an appropriate response to the client and closes all connections
//calling 'closeAllConnections()' function from 'ftp_server.connection.hpp'.

void handleFtpRequestPASS(const char* password, ClientFtpSession& clientFtpSession)
{
    if (clientFtpSession.isValidUser) {
        if (strcmp(password, DEFAULT_PASSWORD) == 0) {
            clientFtpSession.isLoggedIn = true;
            sendToRemote(clientFtpSession.controlConnection, LOGIN_RESPONSE, strlen(LOGIN_RESPONSE));
            return;
        }
    }
    handleNotLoggedIn(clientFtpSession);
}
//Check whether the USER command was successful or not before this PASS command by checking the value in 'isUser'.
//A 'false' value in 'isUser' means USER command has not been successful before this PASS command,
//send appropriate response to the client and closes all connections by calling 'handleNotLoggedIn()' function.
//Compare 'password' with the DEFAULT_PASSWORD.
//If the password does not match, send appropriate response to the client and closes all connections
//by calling ''handleNotLoggedIn()' function.
//If the password matches, sets 'true' to 'isLoogedIn' and sends appropriate response to the client on the control connection
//represented by 'controlSockDescriptor'..
//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleFtpRequestPWD(ClientFtpSession& clientFtpSession)
{
    if (clientFtpSession.isLoggedIn) {
        char *path = get_current_dir_name();
        sendToRemote(clientFtpSession.controlConnection, path, strlen(path));
    } else {
        handleNotLoggedIn(clientFtpSession);
    }
}
//Check whether the client is logged in or not by checking the value in 'isLoggedIn'.
//If the client is not logged in, send an appropriate response to the client and closes all the connections
//by calling 'handleNotLoggedIn()' function.
//If the client is logged in, determines the current working directory sends it to the client in an appropriate response.
//Calls 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

bool isValidDirectory(const char* directory, ClientFtpSession& clientFtpSession) {
    if (startsWith(directory, "./")
        || startsWith(directory, "../")
        || contains(directory, "/.")
        || contains(directory, "/..")
        || contains(directory, "*")) {
        return false;
    }
    DIR* dir = opendir(directory);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
        return true;
    } else {
        return false;
    }
}
void handleFtpRequestCWD(const char* directory, ClientFtpSession& clientFtpSession)
{
    if (clientFtpSession.isLoggedIn) {
        while (startsWith(directory, "/")) {
            directory++;
        }
        char* currentPath = get_current_dir_name();
        auto absolutePath = string(currentPath) + "/" + string(directory);
        cout << absolutePath << endl;
        if (isValidDirectory(absolutePath.c_str(), clientFtpSession)) {
//            strcpy(clientFtpSession.rootDir, absolutePath.c_str());
            cout << absolutePath << endl;
            if (chdir(absolutePath.c_str()) == -1) {
                sendToRemote(clientFtpSession.controlConnection, CWD_FAIL_RESPONSE, strlen(CWD_FAIL_RESPONSE));
            }
            sendToRemote(clientFtpSession.controlConnection, CHANGE_DIRECTORY_RESPONSE, strlen(CHANGE_DIRECTORY_RESPONSE));
        } else {
            sendToRemote(clientFtpSession.controlConnection, INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE));
        }
    } else {
        handleNotLoggedIn(clientFtpSession);
    }
}
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

void handleFtpRequestCDUP(ClientFtpSession& clientFtpSession)
{
    if (clientFtpSession.isLoggedIn) {
        char* currentPath = get_current_dir_name();
        if (string(currentPath).length() <= string(clientFtpSession.rootDir).length()) {
            sendToRemote(clientFtpSession.controlConnection, CDUP_FAIL_RESPONSE, strlen(CDUP_FAIL_RESPONSE));
        } else {
            char parent[256];
            strcpy(parent, string(currentPath).substr(0, string(currentPath).find_last_of("/")).c_str());
            chdir(parent);
            sendToRemote(clientFtpSession.controlConnection, CHANGE_TO_PARENT_DIRECTORY_RESPONSE, strlen(CHANGE_TO_PARENT_DIRECTORY_RESPONSE));
        }
    } else {
        handleNotLoggedIn(clientFtpSession);
    }
}
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

void handleFtpRequestPASV(ClientFtpSession& clientFtpSession) {
    if (clientFtpSession.isLoggedIn) {
        enteringIntoPassive(clientFtpSession);
    } else {
        handleNotLoggedIn(clientFtpSession);
    }
}
//Check whether the client is logged in or not by checking the value in 'isLoggedIn'.
//If the client is not logged in, send an appropriate response to the client and closes all the connections
//by calling 'handleNotLoggedIn()' function.
//If the client is logged in, enter into passive mode by calling 'enteringIntoPassive()' function from 'ftp_server_passive.hpp'
//'enteringIntoPassive()' function starts a passive connection listener, sends an appropriate response to the client.
//'enteringIntoPassive()' function waits for a specific time interval for the client to send a connection request on the listener.
//'enteringIntoPassive()' function accepts client's connection request, if there is any, and opens a data connection with the client.
//'enteringIntoPassive()' function closes the connection listener after opening the data connection with the client.
//'enteringIntoPassive()' function also sends appropriate response to the client using control connection.

void handleFtpRequestNLST(ClientFtpSession& clientFtpSession) {
    if (!clientFtpSession.isLoggedIn) {
        handleNotLoggedIn(clientFtpSession);
    }
    if (!clientFtpSession.isDataConnected) {
        sendToRemote(clientFtpSession.controlConnection, DATA_OPEN_CONNECTION_ERROR_RESPONSE, strlen(DATA_OPEN_CONNECTION_ERROR_RESPONSE));
    }
    auto count = listDirEntries(clientFtpSession.dataConnection);
    if (count == -1) {
        sendToRemote(clientFtpSession.controlConnection, NLST_UNAVAILABLE_ERROR_RERSPONSE, strlen(NLST_UNAVAILABLE_ERROR_RERSPONSE));
    }
    char message[1024];
    sprintf(message, NLST_CONNECTION_CLOSE_RESPONSE, count);
    sendToRemote(clientFtpSession.controlConnection, message, strlen(message));
    closeConnection(clientFtpSession.dataConnection);
    clientFtpSession.isDataConnected = false;
}
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

void handleFtpRequestRETR(const char* file, ClientFtpSession& clientFtpSession) {
    if (!clientFtpSession.isLoggedIn) {
        handleNotLoggedIn(clientFtpSession);
    }
    if (!clientFtpSession.isDataConnected) {
        sendToRemote(clientFtpSession.controlConnection, DATA_OPEN_CONNECTION_ERROR_RESPONSE, strlen(DATA_OPEN_CONNECTION_ERROR_RESPONSE));
    }

    char absolutePath[1025];
    auto tmp = string(clientFtpSession.rootDir) + "/" + string(file);
    strcpy(absolutePath, tmp.c_str());
    auto count = sendFile(absolutePath, clientFtpSession.dataConnection);
    if (count == -1) {
        closeConnection(clientFtpSession.dataConnection);
        return;
    }
    char message[1024];
    sprintf(message, RETR_CONNECTION_CLOSE_RESPONSE, count);
    sendToRemote(clientFtpSession.controlConnection, message, strlen(message));
    closeConnection(clientFtpSession.dataConnection);
    clientFtpSession.isDataConnected = false;
}
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

void handleFtpRequestQUIT(ClientFtpSession& clientFtpSession)
{
    sendToRemote(clientFtpSession.controlConnection, QUIT_RESPONSE, strlen(QUIT_RESPONSE));
    closeConnection(clientFtpSession.controlConnection);
    clientFtpSession.isControlConnected = false;
}
//Handle QUIT command by sending appropriate response to the client.
//Close all connections by calling 'closeAllConnections()' function from 'ftp_server.connection.hpp'.
//Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleFtpRequestUnSupported(ClientFtpSession& clientFtpSession)
{
    sendToRemote(clientFtpSession.controlConnection, UNSUPPORTED_COMMAND_RESPONSE, strlen(UNSUPPORTED_COMMAND_RESPONSE));
}
//Send an appropriate response saying the requested command is not supported by this FTP server.
//Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

void handleNotLoggedIn(ClientFtpSession& clientFtpSession)
{
    sendToRemote(clientFtpSession.controlConnection, NOT_LOGGED_IN_RESPONSE, strlen(NOT_LOGGED_IN_RESPONSE));
    closeConnection(clientFtpSession.controlConnection);
    clientFtpSession.isControlConnected = false;
}
//Send an appropriate response saying the user is not logged in.
//Close all connections by calling 'closeAllConnections()' function from 'ftp_server.connection.hpp'.
//Call 'sendToRemote()' function from 'ftp_server_connection.hpp' to send a response to the client.

