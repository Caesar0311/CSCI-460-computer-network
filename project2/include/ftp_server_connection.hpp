/**
 * @file ftp_server_connection.hpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 * 
 */


#ifndef __FTP_SERVER_CONNECTION_HEADER__
#define __FTP_SERVER_CONNECTION_HEADER__

int sendToRemote(const int sockDescriptor, const char* message, const int messageLength);
//Send the 'message' of length 'messageLength' to the remote computer.
//Use the stream socket, represented by 'sockDescriptor', to send the message.

bool isConnectionReadyToRead(const int sockDescriptor, const int timeoutSec, const int timeoutUSec, bool& isError, bool&isTimedout);
//Return true if there is any data sent by the remote computer on the stream socket represented by 'sockDescriptor'.
//Wait for the data until 'timeoutSec' + 0.000001x'timeoutUsec' time.
//If no data has been received before the time out, set 'isTimedout' value to 'true'.
//If any error occurs, set 'isError' value to 'true'.
//Call 'isSocketReadyToRead()' function from 'ftp_server_net_util.hpp' to do all of the above.

int receiveFromRemote(const int sockDescriptor, char* message, int messageLength);
//Receive data from the remote computer into a buffer 'message'.
//Set the length of the received data into 'messageLength'.
//Use the stream socket, represented by 'sockDescriptor', to receive the data.

void closeConnection(int& sockDescriptor);
//Close the stream socket, represented by 'sockDescriptor'.

#endif
