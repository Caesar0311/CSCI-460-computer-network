/**
 * @file ftp_server_passive.cpp
 * @author Your Name, Student Number, Section, CSCI 460, VIU
 * @version 1.0.0
 * @date Date you have last modified your code in this file, e.g., August 05, 2021
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */



//Include required library and custom header files.
#include "ftp_server_passive.hpp"
#include "ftp_server_connection_listener.hpp"
#include "ftp_server_connection.hpp"
#include "ftp_server_net_util.hpp"
#include "ftp_server_request.hpp"
#include "ftp_server_string_util.hpp"
#include "ftp_server_response.hpp"
#include "ftp_server_string_util.hpp"

//Implement all the functions prototyped in the header file ftp_server_passive.hpp
void enteringIntoPassive(ClientFtpSession& clientFtpSession)
{
//    启动passive监听
    bool succeeded = false;
    startPassiveListener(clientFtpSession, succeeded);
    if (!succeeded) {
        sendToRemote(clientFtpSession.controlConnection, PASSIVE_ERROR_RESPONSE, strlen(PASSIVE_ERROR_RESPONSE));
        return;
    }
//    创建响应
    char response[FTP_RESPONSE_MAX_LENGTH];
    createPassiveSuccessResponse(response, clientFtpSession.dataListener);
    if (clientFtpSession.dataListener == -1) {
        sendToRemote(clientFtpSession.controlConnection, DATA_LOCAL_ERROR_RESPONSE, strlen(DATA_LOCAL_ERROR_RESPONSE));
    }
//    发送响应信息
    sendToRemote(clientFtpSession.controlConnection, response, strlen(response));
//    等待回复
    bool isError = false;
    bool isTimedOut = false;
    if (isListenerSocketReady(clientFtpSession.dataListener, DATA_CONNECTION_TIME_OUT_SEC,
                              DATA_CONNECTION_TIME_OUT_USEC, isError, isTimedOut)) {
//        若有进一步操作
        acceptClientConnection(clientFtpSession.dataListener, clientFtpSession.dataConnection);
        if (clientFtpSession.dataConnection == -1) {
            sendToRemote(clientFtpSession.controlConnection, PASSIVE_ERROR_RESPONSE, strlen(PASSIVE_ERROR_RESPONSE));
        }
        clientFtpSession.isDataConnected = true;
        closeListenerSocket(clientFtpSession.dataListener);
        sendToRemote(clientFtpSession.controlConnection, DATA_CONNECTION_SUCCESS_RESPONSE, strlen(DATA_CONNECTION_SUCCESS_RESPONSE));
    } else {
        sendToRemote(clientFtpSession.controlConnection, PASSIVE_ERROR_TIMEOUT_RESPONSE, strlen(PASSIVE_ERROR_TIMEOUT_RESPONSE));
    }
}
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


void startPassiveListener(ClientFtpSession& clientFtpSession, bool& succeded)
{
    char portStr[10] = "2020";
    startListenerSocket(portStr, clientFtpSession.dataListener, succeded);
}
//Start a passive listener socket that can listen connection requests from the remote computer.
//by calling 'startListenerSocket()' function from 'ftp_server_connection_listener.hpp'.


void stopPassiveListener(ClientFtpSession& clientFtpSession) {
    clientFtpSession.isDataConnected = false;
    clientFtpSession.isDataListener = false;
    clientFtpSession.isDataConnected = false;
    clientFtpSession.dataConnection = -1;
    clientFtpSession.dataListener = -1;
}


void createPassiveSuccessResponse(char* response, const int passiveListenerSockDescriptor)
{
    int listenerPort = getPortFromSocketDescriptor(passiveListenerSockDescriptor);
    auto ip = getHostIPAddress();
    replaceAll(ip, '.', ',');
    string tmp = ip;
    tmp.append(",");
    tmp.append(to_string(listenerPort / 256));
    tmp.append(to_string(listenerPort % 256));
    strcpy(response, tmp.c_str());
    sprintf(response, PASSIVE_SUCCESS_RESPONSE, ip, listenerPort / 256, listenerPort % 256);
}
//Determine the passive listener port number from 'passiveListenerSockDescriptor' by calling
//'getPortFromSocketDescriptor()' function from 'ftp_server_net_util.hpp'.
//Determine the local IP address by calling 'getHostIPAddress()' function from 'ftp_server_net_util.hpp'.
//Includes both the IP address and the port number into passive success response according to RFC.


