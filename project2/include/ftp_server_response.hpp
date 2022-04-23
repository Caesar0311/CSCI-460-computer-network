/**
 * @file ftp_server_response.hpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 * 
 */

#ifndef __FTP_SERVER_RESPONSE_HEADER__
#define __FTP_SERVER_RESPONSE_HEADER__

#define FTP_RESPONSE_MAX_LENGTH 1024

//Success responses
#define CONNECTED_RESPONSE "220 Service ready.\n"
#define LOGIN_RESPONSE "230 User logged in, proceed.\n"
#define CHANGE_DIRECTORY_RESPONSE "200 Directory successfully changed.\n"
#define CHANGE_TO_PARENT_DIRECTORY_RESPONSE "200 Successfully changed to parent directory.\n"
#define PASSIVE_SUCCESS_RESPONSE "227 Entering Passive Mode (%s,%d,%d).\n"
#define DATA_CONNECTION_SUCCESS_RESPONSE "200 Successfully opened data connection."
#define NLST_CONNECTION_CLOSE_RESPONSE "226 Directory listing (%d) entries. Closing data connection..\n"
#define RETR_CONNECTION_CLOSE_RESPONSE "250 Requested file action with file size (%d) is OK, closing data connection..\n"
#define QUIT_RESPONSE "226 Closing connection..\n"
#define USERNAME_OK_RESPONSE "331 Username okay, need password.\n"

//Unsuccess responses
#define INTERNAL_ERROR_RESPONSE "400 Internal error, service is temporarily unavailable .\n"
#define INVALID_USERNAME_RESPONSE "530 Invalid user name.\n"
#define NOT_LOGGED_IN_RESPONSE "530 Not logged in.\n"
#define INVALID_PATH_RESPONSE "550 Invalid path.\n"
#define CWD_FAIL_RESPONSE "550 Changing directory failed.\n"
#define CDUP_FAIL_RESPONSE "550 Failed to change working directory to parent.\n"
#define PASSIVE_ERROR_TIMEOUT_RESPONSE "500 Timed out, no connection established within 20s.\n"
#define PASSIVE_ERROR_RESPONSE "500 Error, Failed to open data connection.\n"
#define DATA_OPEN_CONNECTION_ERROR_RESPONSE "425 Can't open data connection. Use PASV first.\n"
#define DATA_LOCAL_ERROR_RESPONSE "451 Requested action aborted, local error in processing.\n"
#define NLST_INVALID_OPTION_ERROR_RESPONSE "501 Syntax error in parameters or arguments.\n"
#define NLST_UNAVAILABLE_ERROR_RERSPONSE "450 Requested action not taken.\n"
#define RETR_UNAVAILABLE_ERROR_RESPONSE "550 Requested action not taken. File unavailable.\n"
#define UNSUPPORTED_COMMAND_RESPONSE "500 Unsupported command.\n"
#define INVALID_NUMBER_OF_ARGUMENTS_RESPONSE "501 Syntax error in parameters or arguments.\n"
#define CONNECTION_RESET_BY_PEER "10054 The connection was forcibly closed by the remote host.\n"

#endif
