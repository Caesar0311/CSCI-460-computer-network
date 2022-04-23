/**
 * @file ftp_server_net_util.hpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 * 
 */

#ifndef __FTP_SERVER_NET_UTIL_HEADER__

#define __FTP_SERVER_NET_UTIL_HEADER__
#include <unistd.h>
#include <netinet/in.h>
#include "ftp_server_net_util.hpp"
#include <asm-generic/ioctls.h>
#include <sys/ioctl.h>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <asm-generic/ioctls.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <iostream>
using namespace std;
using namespace chrono;

#define MAX_IP_ADDRESS_LENGTH 1024

void closeSocket(int& sockDescriptor);
//Close the socket n represented by 'sockDescriptor'.

int getPortFromSocketDescriptor(const int sockDescriptor);
//Determine associated port number from a given socket descriptor and return the port number.

bool isSocketReadyToRead(const int sockDescriptor, const int timeoutSec, const int timeoutUSec, bool& isError, bool& isTimedout); 
//Return true if there is something sent by the remote computer on the socket represented by 'sockDescriptor'.
//Wait until 'timeoutSec' + 0.000001x'timeoutUsec' time.
//If nothing  has been sent before the time out, set 'isTimedout' value to 'true'.
//If any error occurs, set 'isError' value to 'true'.

char* getHostIPAddress();
//Determine the IP address of the local host and return it.

#endif
