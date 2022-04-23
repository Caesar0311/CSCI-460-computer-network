/**
 * @file ftp_server_connection.cpp
 * @author Your Name, Student Number, Section, CSCI 460, VIU
 * @version 1.0.0
 * @date Date you have last modified your code in this file, e.g., August 05, 2021
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */



//Include required library and custom header files.

#include "ftp_server_connection.hpp"
#include "ftp_server_net_util.hpp"
//Implement all the functions prototyped in the header file ftp_server_connection.hpp


int sendToRemote(const int sockDescriptor, const char* message, const int messageLength)
{
    int res = write(sockDescriptor, message, messageLength);
    if (res < 0) {
        return -1;
    }
    return res;
}
//Send the 'message' of length 'messageLength' to the remote computer.
//Use the stream socket, represented by 'sockDescriptor', to send the message.

bool isConnectionReadyToRead(const int sockDescriptor, const int timeoutSec, const int timeoutUSec, bool& isError, bool& isTimedout)
{
    return isSocketReadyToRead(sockDescriptor, timeoutSec, timeoutUSec, isError, isTimedout);
}
//Return true if there is any data sent by the remote computer on the stream socket represented by 'sockDescriptor'.
//Wait for the data until 'timeoutSec' + 0.000001x'timeoutUsec' time.
//If no data has been received before the time out, set 'isTimedout' value to 'true'.
//If any error occurs, set 'isError' value to 'true'.
//Call 'isSocketReadyToRead()' function from 'ftp_server_net_util.hpp' to do all of the above.

int receiveFromRemote(const int sockDescriptor, char* message, int messageLength)
{
    int res = read(sockDescriptor, message, messageLength);
    if (res <= 0) {
        return -1;
    }
    return res;
}
//Receive data from the remote computer into a buffer 'message'.
//Set the length of the received data into 'messageLength'.
//Use the stream socket, represented by 'sockDescriptor', to receive the data.

void closeConnection(int& sockDescriptor)
{
    close(sockDescriptor);
}
//Close the stream socket, represented by 'sockDescriptor'.
