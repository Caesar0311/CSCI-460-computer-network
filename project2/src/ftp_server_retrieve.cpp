/**
 * @file ftp_server_retrieve.cpp
 * @author Your Name, Student Number, Section, CSCI 460, VIU
 * @version 1.0.0
 * @date Date you have last modified your code in this file, e.g., August 05, 2021
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */



//Include required library and custom header files.

#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <iostream>
#include <sys/socket.h>

#include "ftp_server_retrieve.hpp"
#include "ftp_server_connection.hpp"
#include "ftp_server_response.hpp"
using namespace std;
//Implement all the functions prototyped in the header file ftp_server_retreive.hpp


int sendFile(const char* filename, int& dataSockDescriptor)
{
//    一些定义
    char  buffer[DATA_SOCKET_SEND_BUFFER_SIZE];
    FILE *fp;

//    检测文件行不行
    if( ( fp = fopen(filename,FILE_OPEN_MODE) ) == nullptr ){
        sendToRemote(dataSockDescriptor, RETR_UNAVAILABLE_ERROR_RESPONSE, strlen(RETR_UNAVAILABLE_ERROR_RESPONSE));
        return -1;
    }
//    构造size数组
    struct stat info{};
    stat(filename, &info);
    size_t fileSize = info.st_size;
    size_t sentSize = 0;
    vector<int> sendToByCount{};
    cout << "file size: " << fileSize << endl;

//    发射！
    bzero(buffer,DATA_SOCKET_SEND_BUFFER_SIZE);
    int nCount;
    while( (nCount = fread(buffer, 1, sizeof(buffer), fp)) > 0 ){
        write(dataSockDescriptor, buffer, nCount);
    }

    fclose(fp);
    return fileSize;
}
//Check whether the file with the 'filename' is accessible or not.
//If the file is accessible, open the file in FILE_OPEN_MODE.
//Determine the size of the file and initialize a send-to-byte-count to the size of the file.
//Do followings until send-to-byte-count is zero:
//	If send-to-byte-count is greater than or equal to DATA_SOCKET_SEND_BUFFER_SIZE
//		Read DATA_SOCKET_SEND_BUFFER_SIZE bytes from the file in a buffer.
//		Send the buffer content to the client using data connection represented by 'dataSockDescriptor'.
//		Update send-to-byte-count.
//	If send-to-byte-count is less than DATA_SOCKET_SEND_BUFFER_SIZE
//		Read send-to-byte-count bytes from the file in a buffer.
//		Send the buffer content to the client using data connection represented by 'dataSockDescriptor'.
//		Update send-to-byte-count.
//
//
//Return the size of the file.