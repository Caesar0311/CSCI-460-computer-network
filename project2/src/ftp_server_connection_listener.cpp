/**
 * @file ftp_server_connection_listener.cpp
 * @author Your Name, Student Number, Section, CSCI 460, VIU
 * @version 1.0.0
 * @date Date you have last modified your code in this file, e.g., August 05, 2021
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */



//Include required library and custom header files.

#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include "ftp_server_net_util.hpp"
//Implement all the functions prototyped in the header file ftp_server_connection_listener.hpp
using namespace std;

void startListenerSocket(char* port, int& listenerSockDescriptor, bool& succeded)
{
    int rc;
    char hostname[50];
    char port_str[16] = {};
    rc = gethostname(hostname,sizeof(hostname));
    //    if(rc == 0)
    //        printf("hostname = %s\n",hostname);

    struct addrinfo hints = {}, *addrs;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sprintf(port_str, "%s", port);
    //    得到的信息储存在addrs里面
    int err = getaddrinfo(hostname, port_str, &hints, &addrs);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s\n", hostname, gai_strerror(err));
    }
    //    新建socket
    listenerSockDescriptor  = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
    //    重用

    setsockopt(listenerSockDescriptor, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const void *>((int) {1}), sizeof(int));
    //    绑定
    bind(listenerSockDescriptor, addrs->ai_addr, addrs->ai_addrlen);
    //    监听
    listen(listenerSockDescriptor, 54);
    //    成功
    succeded = true;
}
//Get the hostname of the machine where this code is executing.
//Get the IP Address, Address Family, Socket Type, and Protocol from the hostname and the port number.
//Open a socket using the Address Family, Socket Type, and port number and assign the opened  socket to 'listenerSocketDescriptor'.
//Set the socket option to re-use address and bind the IP address of the machine to the listener socket.
//Make the listener socket to listen connection request.
//Set true to 'succeded' once everything above is done successfully.


bool isListenerSocketReady(const int listenerSockDescriptor, const int timeoutSec, const int timeoutUSec, bool& isError, bool& isTimedout){
    return isSocketReadyToRead(listenerSockDescriptor, timeoutSec, timeoutUSec, isError, isTimedout);

}
//Return true if there is any remote connection request on the listener socket represented by 'listenerSockDescriptor'.
//Wait for a connection request until 'timeoutSec' + 'timeoutUsec' time.
//If no connection request has been received before the time out, set 'isTimedout' value to 'true'.
//If any error occurs, set 'isError' value to 'true'.
//Call 'isSocketReadyToRead()' function from 'ftp_server_net_util.hpp' to do all the above.

void acceptClientConnection(const int listenerSockDescriptor, int& clientSockDescriptor)
{

    struct sockaddr addr;
    socklen_t len = sizeof(addr);
    int res = accept(listenerSockDescriptor, &addr, &len);
    if (res == -1) {

            cout << "accept error" << endl;
    }
    clientSockDescriptor = res;
}
//Accept a connection request on the listener socket represented by 'listenerSockDescriptor'.
//and assign the client socket to 'clientSockDescriptor'.

void closeListenerSocket(int& listenerSockDescriptor)
{
    close(listenerSockDescriptor);
}
//Close the listener socket represented by 'listenerSockDescriptor'.












