/**
 * @file ftp_server_connection_listener.hpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 * 
 */


#ifndef __FTP_SERVER_CONNECTION_LISTENER_HEADER__
#define __FTP_SERVER_CONNECTION_LISTENER_HEADER__

#define MAX_CLIENT_CONNECTIONS 10

void startListenerSocket(char* port, int& listenerSockDescriptor, bool& succeded);
//Get the hostname of the machine where this code is executing.
//Get the IP Address, Address Family, Socket Type, and Protocol from the hostname and the port number.
//Open a socket using the Address Family, Socket Type, and port number and assign the opened  socket to 'listenerSocketDescriptor'.
//Set the socket option to re-use address and bind the IP address of the machine to the listener socket.
//Make the listener socket to listen connection request.
//Set true to 'succeded' once everything above is done successfully.


bool isListenerSocketReady(const int listenerSockDescriptor, const int timeoutSec, const int timeoutUSec, bool& isError, bool&isTimedout);
//Return true if there is any remote connection request on the listener socket represented by 'listenerSockDescriptor'.
//Wait for a connection request until 'timeoutSec' + 'timeoutUsec' time.
//If no connection request has been received before the time out, set 'isTimedout' value to 'true'.
//If any error occurs, set 'isError' value to 'true'.
//Call 'isSocketReadyToRead()' function from 'ftp_server_net_util.hpp' to do all the above.

void acceptClientConnection(const int listenerSockDescriptor, int& clientSockDescriptor);
//Accept a connection request on the listener socket represented by 'listenerSockDescriptor'.
//and assign the client socket to 'clientSockDescriptor'.

void closeListenerSocket(int& listenerSockDescriptor);
//Close the listener socket represented by 'listenerSockDescriptor'.

#endif
