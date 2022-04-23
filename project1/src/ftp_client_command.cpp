/**
 * @file ftp_client_command.cpp
 * @author Your Name, Student Number, Section, CSCI 460, VIU
 * @version 1.0.0
 * @date Date you have last modified your code in this file, e.g., August 05, 2021
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */

//Include required library and custom header files.
#include "../include/ftp_client_command.hpp"
#include "ftp_server_response.hpp"
#include <iostream>
#include <string>
#include "ftp_client_connection.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
//Implement all the functions prototyped in the header file ftp_client_command.hpp

void interpretAndHandleUserCommand(string command, ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    //get the command and the params
    std::string delimiter = " ";
    std::string cmd, param;
    size_t pos = 0;
    pos = command.find(delimiter);
    cmd = command.substr(0, pos);
    if (pos > 0) {
        param = command.substr(pos + 1, command.length() - pos);
    }
    if (cmd == FTP_CLIENT_USER_COMMAND_HELP) {
        handleCommandHelp();
    }
    if (cmd == FTP_CLIENT_USER_COMMAND_CHANGE_DIRECTORY) {
        handleCommandChangeDirectory(param, clientFtpSession, serverResponse);
    }

    if (cmd == FTP_CLIENT_USER_COMMAND_USER) {
        handleCommandUser(param, clientFtpSession, serverResponse);
    }

    if (cmd == FTP_CLIENT_USER_COMMAND_PASSWORD) {
        handleCommandPassword(param, clientFtpSession, serverResponse);
    }

    if (cmd == FTP_CLIENT_USER_COMMAND_PRINT_DIRECTORY) {
        handleCommandPrintDirectory(clientFtpSession, serverResponse);
    }

    if (cmd == FTP_CLIENT_USER_COMMAND_DIRECTORY) {
        handleCommandDirectory(clientFtpSession, serverResponse);
    }

    if (cmd == FTP_CLIENT_USER_COMMAND_GET) {
        handleCommandGetFile(param, clientFtpSession, serverResponse);
    }

    if (cmd == FTP_CLIENT_USER_COMMAND_CHANGE_DIRECTORY_UP) {
        handleCommandChangeDirectoryUp(clientFtpSession, serverResponse);
    }

    if (cmd == FTP_CLIENT_USER_COMMAND_QUIT) {
        handleCommandQuit(clientFtpSession, serverResponse);
    }
}

//replace a substring in a string with a newstring,
//here I used it to replace ',' with '.'
string subreplace(string resource_str, string sub_str, string new_str) {
    string::size_type pos = 0;
    while ((pos = resource_str.find(sub_str)) != string::npos) {
        resource_str.replace(pos, sub_str.length(), new_str);
    }
    return resource_str;
}

//from response string get the response code
string getRespCode(string recvMessage) {
    string respCode;
    for (char i : recvMessage) {
        if (i >= '0' && i <= '9') {
            respCode += i;
        } else {
            break;
        }
    }
    return respCode;
}

//print help message to screen
void handleCommandHelp() {
    std::cout << "Usage: csci460Ftp>> [ help | user | pass | pwd | dir | cwd | cdup | get | quit ]" << std::endl;
    std::cout << "help                    Gives the list of FTP commands available and how to use them." << std::endl;
    std::cout << "user    <username>      Sumbits the <username> to FTP server for authentication." << std::endl;
    std::cout << "pass    <password>      Sumbits the <password> to FTP server for authentication." << std::endl;
    std::cout << "pwd                     Requests FTP server to print current directory." << std::endl;
    std::cout << "dir                     Requests FTP server to list the entries of the current directory."
              << std::endl;
    std::cout << "cwd     <dirname>       Requests FTP server to change current working directory." << std::endl;
    std::cout << "cdup                    Requests FTP server to change current directory to parent directory."
              << std::endl;
    std::cout << "get     <filename>      Requests FTP server to send the file with <filename>." << std::endl;
    std::cout << "quit                    Requests to end FTP session and quit." << std::endl;
}

//for simple command, just send the ftpCommand to server and read the message send by server
//from the response get the response code, which is used to find unsuccessful requests,
//them judge whether the application needs to qiut
void handleSimpleCommand(string ftpCommand, bool checkAuthentication, ClientFtpSession &clientFtpSession,
                         ServerResponse &serverResponse) {

    sendToServer(clientFtpSession.controlConnection, ftpCommand.c_str(), ftpCommand.length());

    char recvMessage[FTP_RESPONSE_MAX_LENGTH];
    int recvSize = receiveFromServer(clientFtpSession.controlConnection, recvMessage, sizeof(recvMessage));
    recvMessage[recvSize] = '\0';
    serverResponse.responses[serverResponse.count++] = recvMessage;

    string respCode = getRespCode(serverResponse.responses[serverResponse.count - 1]);

    if (respCode.length() > 0) {
        if ((checkAuthentication &&
             (stoi(respCode) == 400 |
              stoi(respCode) == 530 |
              stoi(respCode) == 550 |
              stoi(respCode) == 500 |
              stoi(respCode) == 425 |
              stoi(respCode) == 451 |
              stoi(respCode) == 501 |
              stoi(respCode) == 450 |
              stoi(respCode) == 10054
             ))) {
            stopClientFTPSession(clientFtpSession);
        }
        if (stoi(respCode) == 226) {
            stopClientFTPSession(clientFtpSession);
        }
    }
}

//send "QUIT" to server via handleSimpleCommand
void handleCommandQuit(ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    string command = "QUIT";
    handleSimpleCommand(command, false, clientFtpSession, serverResponse);
}

//send "USER " + username to server via handleSimpleCommand
void handleCommandUser(string username, ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    string command = "USER " + username;
    handleSimpleCommand(command, true, clientFtpSession, serverResponse);
}

//send "PASS " + password to server via handleSimpleCommand
void handleCommandPassword(string password, ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    string command = "PASS " + password;
    handleSimpleCommand(command, true, clientFtpSession, serverResponse);
}

//1. send "PASV" to server, if success, get the data connection port and ipaddress
//2. connect to the new ip address and port, if success, read data from the new socket using handleNLIST
//3. close the connection and reset the clientFtpSession's dataConnection and isDataConnected
void handleCommandDirectory(ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    handlePassive(clientFtpSession, serverResponse);
    int dataPort;
    char dataHost[MAX_IP_ADDRESS_LENGTH];
    char responseStr[FTP_RESPONSE_MAX_LENGTH] = {'\0'};
    std::copy(serverResponse.responses[serverResponse.count - 1].begin(),
              serverResponse.responses[serverResponse.count - 1].end(), responseStr);
    getHostIPAndPortFromPassiveSuccessResponse(responseStr, dataHost, dataPort);
    string respCode = getRespCode(serverResponse.responses[serverResponse.count - 1]);

    if (respCode == "227") {
        int fd;
        bool isConnected = false;
        {
            struct sockaddr_in addr
                    {
                    };
            memset(&addr, 0, sizeof(struct sockaddr_in));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(dataPort);
            addr.sin_addr.s_addr = inet_addr(dataHost);
            bzero(&(addr.sin_zero), 8);
            fd = socket(AF_INET, SOCK_STREAM, 0);
            auto res = connect(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr));
            if (res == 0) {
                isConnected = true;
                clientFtpSession.dataConnection = fd;
                clientFtpSession.isDataConnected = true;
            }
        }
        if (isConnected) {
            handleNLIST(clientFtpSession, serverResponse);
            char recvMessage[512];
            int recsSize = receiveFromServer(clientFtpSession.controlConnection, recvMessage, sizeof(recvMessage));
            recvMessage[recsSize] = '\0';
            serverResponse.responses[serverResponse.count++] = recvMessage;
        }
    }
}

//send "PWD" + username to server via handleSimpleCommand
void handleCommandPrintDirectory(ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    string command = "PWD";
    handleSimpleCommand(command, false, clientFtpSession, serverResponse);
}

//send "CWD" + username to server via handleSimpleCommand
void handleCommandChangeDirectory(string path, ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    string command = "CWD " + path;
    handleSimpleCommand(command, false, clientFtpSession, serverResponse);
}

//send "CDUP" + username to server via handleSimpleCommand
void handleCommandChangeDirectoryUp(ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    string command = "CDUP";
    handleSimpleCommand(command, false, clientFtpSession, serverResponse);
}

//1. send "PASV" to server, if success, get the data connection port and ipaddress
//2. connect to the new ip address and port, if success, read data and write to local file from the new socket using handleRETR
//3. close the connection and reset the clientFtpSession's dataConnection and isDataConnected
void handleCommandGetFile(string filename, ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    if (filename.length() == 0) {
        return;
    }
    handlePassive(clientFtpSession, serverResponse);
    int dataPort;
    char dataHost[MAX_IP_ADDRESS_LENGTH];
    char responseStr[FTP_RESPONSE_MAX_LENGTH] = {'\0'};
    std::copy(serverResponse.responses[serverResponse.count - 1].begin(),
              serverResponse.responses[serverResponse.count - 1].end(), responseStr);
    getHostIPAndPortFromPassiveSuccessResponse(responseStr, dataHost, dataPort);
    string respCode = getRespCode(serverResponse.responses[serverResponse.count - 1]);
    if (respCode == "227") {
        int fd;
        bool isConnected = false;
        {
            struct sockaddr_in addr{};
            memset(&addr, 0, sizeof(struct sockaddr_in));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(dataPort);
            addr.sin_addr.s_addr = inet_addr(dataHost);
            bzero(&(addr.sin_zero), 8);
            fd = socket(AF_INET, SOCK_STREAM, 0);
            auto res = connect(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr));
            if (res == 0) {
                isConnected = true;
                clientFtpSession.dataConnection = fd;
                clientFtpSession.isDataConnected = true;
            }
        }
        if (isConnected) {
            handleRETR(filename, clientFtpSession, serverResponse);

            char recvMessage[512];
            int recsSize = receiveFromServer(clientFtpSession.controlConnection, recvMessage, sizeof(recvMessage));
            recvMessage[recsSize] = '\0';
            serverResponse.responses[serverResponse.count++] = recvMessage;
        }
    }
}

//send "PASV" to server, if success, write the response to serverResponse
void handlePassive(ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    string sendMessage = "PASV";
    sendToServer(clientFtpSession.controlConnection, sendMessage.c_str(), sendMessage.length());
    char recvMessage[FTP_RESPONSE_MAX_LENGTH];
    int recsSize = receiveFromServer(clientFtpSession.controlConnection, recvMessage, sizeof(recvMessage));
    recvMessage[recsSize] = '\0';
    serverResponse.responses[serverResponse.count++] = recvMessage;
}

//send "NLST" to server, if success, write the response to serverResponse,
//then close the data connection and reset clientFtpSession.dataConnection and clientFtpSession.isDataConnected
void handleNLIST(ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    string sendMessage = "NLST";
    sendToServer(clientFtpSession.controlConnection, sendMessage.c_str(), sendMessage.length());
    char recvMessage[FTP_RESPONSE_MAX_LENGTH];
    int recsSize = receiveFromServer(clientFtpSession.controlConnection, recvMessage, sizeof(recvMessage));
    recvMessage[recsSize] = '\0';
    serverResponse.responses[serverResponse.count++] = recvMessage;
    std::string filelist;
    while (true) {
        int recvSize = receiveFromServer(clientFtpSession.dataConnection, recvMessage, sizeof(recvMessage));
        if (recvSize <= 0) {
            break;
        }
        recvMessage[recvSize] = '\0';
        filelist += recvMessage;
    }
    serverResponse.responses[serverResponse.count++] = filelist;
    close(clientFtpSession.dataConnection);
    clientFtpSession.dataConnection = -1;
    clientFtpSession.isDataConnected = false;
}

//send "RETR" to server, if success, write the response to serverResponse,
//then close the data connection and reset clientFtpSession.dataConnection and clientFtpSession.isDataConnected
void handleRETR(string filename, ClientFtpSession &clientFtpSession, ServerResponse &serverResponse) {
    string sendMessage = "RETR " + filename;
    sendToServer(clientFtpSession.controlConnection, sendMessage.c_str(), sendMessage.length());
    char recvMessage[FTP_RESPONSE_MAX_LENGTH];
    int recsSize = receiveFromServer(clientFtpSession.controlConnection, recvMessage, sizeof(recvMessage));
    recvMessage[recsSize] = '\0';
    serverResponse.responses[serverResponse.count++] = recvMessage;

    FILE *fp = fopen(filename.c_str(), FILE_OPEN_MODE);

    char recvBuffer[DATA_SOCKET_RECEIVE_BUFFER_SIZE];
    bzero(recvBuffer, sizeof(recvBuffer));
    while (true) {
        int recvSize = receiveFromServer(clientFtpSession.dataConnection, recvBuffer, sizeof(recvBuffer));

        if (recvSize <= 0) {
            break;
        }
        if (fwrite(recvBuffer, sizeof(char), recvSize, fp) < recvSize) {
            break;
        }
        bzero(recvBuffer, sizeof(recvBuffer));
    }
    close(clientFtpSession.dataConnection);
    clientFtpSession.dataConnection = -1;
    clientFtpSession.isDataConnected = false;
}

//from the response extract hostip and hostport
//write them into the references of hostip and hostport
void getHostIPAndPortFromPassiveSuccessResponse(char *response, char *hostIP, int &hostPort) {
    std::string respStr = response;
    std::string delimiter = " ";
    std::string respCode, ipAndPort;
    int last1, last2;
    size_t pos = 0;
    pos = respStr.find(delimiter);
    respCode = respStr.substr(0, pos);
    pos = respStr.find_last_of(delimiter);
    ipAndPort = respStr.substr(pos + 2, respStr.length() - pos - 4);

    pos = 0;
    int pivot = 0;
    for (pos = 0; pos < ipAndPort.length(); ++pos) {
        if (ipAndPort[pos] == ',') {
            pivot++;
        }
        if (pivot == 4) {
            break;
        }
    }
    std::string addr = ipAndPort.substr(0, pos);
    addr = subreplace(addr, ",", ".");
    std::string port = ipAndPort.substr(pos + 1, ipAndPort.length());

    pos = port.find(',');
    last1 = stoi(port.substr(0, pos));
    last2 = stoi(port.substr(pos + 1, port.length()));
    last1 = last1 * 256 + last2;
    std::copy(addr.begin(), addr.end(), hostIP);
    hostIP[addr.length()] = '\0';

    hostPort = last1;
}