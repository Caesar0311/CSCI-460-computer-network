/**
 * @file ftp_server_retrieve.hpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 * 
 */

#ifndef __FTP_SERVER_RETRIEVE_HEADER__
#define __FTP_SERVER_RETRIEVE_HEADER__

#define FILE_OPEN_MODE "r"
#define DATA_SOCKET_SEND_BUFFER_SIZE 65536 //64K

int sendFile(const char* filename, int& dataSockDescriptor);
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

#endif
