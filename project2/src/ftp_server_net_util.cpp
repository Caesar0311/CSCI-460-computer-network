/**
 * @file ftp_server_net_util.cpp
 * @author Your Name, Student Number, Section, CSCI 460, VIU
 * @version 1.0.0
 * @date Date you have last modified your code in this file, e.g., August 05, 2021
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */



//Include required library and custom header files.
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

//Implement all the functions prototyped in the header file ftp_server_net_util.hpp


void closeSocket(int& sockDescriptor){
    close(sockDescriptor);
}
//Close the socket n represented by 'sockDescriptor'.
//todo:这里可能返回0，因为scokfd没有经过连接
int getPortFromSocketDescriptor(const int sockDescriptor)
{
    unsigned int myPort;
    struct sockaddr_in my_addr{};

    bzero(&my_addr, sizeof(my_addr));
    socklen_t len = sizeof(my_addr);
    getsockname(sockDescriptor, (struct sockaddr *) &my_addr, &len);
    myPort = ntohs(my_addr.sin_port);
    return myPort;
}
//Determine associated port number from a given socket descriptor and return the port number.

/*
//bool isSocketReadyToRead(const int sockDescriptor, const int timeoutSec, const int timeoutUSec, bool& isError, bool& isTimedout)
//{
////    这里设置为非阻塞，方便计时
//    int b_on = 1;
//    ioctl(sockDescriptor, FIONBIO, &b_on);
//    int messageLength = 1024;
//    char* message = (char*)malloc(1024);
//    auto start = system_clock::now();
//
//    while(true){
//        auto end   = system_clock::now();
//        auto duration = duration_cast<microseconds>(end - start);
//        if (double(duration.count()) > 1000000*(timeoutSec + 0.000001 * timeoutUSec)) {
//            isTimedout = true;
//            break;
//        }
//        int rc, max_sock;
//        struct timeval timeout{};
//        memset( &timeout, 0, sizeof(timeout) );
//        fd_set reading;
//        FD_SET( sockDescriptor, &reading );
//        max_sock = 2;
//        rc = select( sockDescriptor, &reading, nullptr, nullptr, &timeout );
//        //todo: 判断可读这里有问题，其他的暂时ok
//        if (rc < 0) {
//            cout << "select" << endl;
//        }
//        else if (rc == 0) {
////            cout << "no available" << endl;
//        }
//        else {
//            cout << "you available" << endl;
//            return true;
//        }
//    }
//    return false;
//}
 */
/*
//Return true if there is something sent by the remote computer on the socket represented by 'sockDescriptor'.
//Wait until 'timeoutSec' + 0.000001x'timeoutUsec' time.
//If nothing  has been sent before the time out, set 'isTimedout' value to 'true'.
//If any error occurs, set 'isError' value to 'true'.
*/
bool isSocketReadyToRead(const int sockDescriptor, const int timeoutSec, const int timeoutUSec, bool& isError, bool& isTimedout)
{
    int rc;
    struct timeval timeout{};
    timeout.tv_sec = timeoutSec;
    timeout.tv_usec = 0.000001 * timeoutUSec;
    fd_set reading;
    FD_ZERO(&reading);
    FD_SET( sockDescriptor, &reading );
    rc = select( sockDescriptor + 1, &reading, nullptr, nullptr, &timeout );

    //todo: 判断可读这里有问题，其他的暂时ok
    if (rc < 0) {
        isError = true;
        return false;
    }
    else if (rc == 0) {
        isTimedout = true;
        return false;
    }
    else {
        return true;
    }
}

char* getHostIPAddress(){
    char host_name[MAX_IP_ADDRESS_LENGTH]="";
    struct hostent *host_ent;
    gethostname(host_name, sizeof(host_name));
    host_ent = gethostbyname(host_name);
    char* first_ip = inet_ntoa(*(struct in_addr*)(host_ent->h_addr_list[0]));
    return first_ip;
}
//Determine the IP address of the local host and return it.


















