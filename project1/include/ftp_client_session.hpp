/**
 *  @file ftp_client_control.hpp
 *  @author Humayun Kabir, Instructor, CSCI 460, VIU
 *  @version 1.0.0
 *  @date June 22, 2020
 */


#ifndef __FTP_CLIENT_CONTROL_HEADER__
#define __FTP_CLIENT_CONTROL_HEADER__

#define FTP_CLIENT_DEAFULT_SERVER_IP "127.0.0.1"
#define FTP_CLIENT_DEFAULT_SERVER_PORT 21
#define BUFFER_SIZE 512


struct ClientFtpSession {
	int controlConnection = -1;
	int dataConnection = -1;
	bool isControlConnected = false;
	bool isDataConnected = false;
};


void startClientFTPSession(const char* serverIP, int serverPort, ClientFtpSession& clientFtpSession);


void stopClientFTPSession(ClientFtpSession& clientFtpSession);


#endif
