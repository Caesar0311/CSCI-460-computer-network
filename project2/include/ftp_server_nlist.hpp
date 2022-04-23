/**
 * @file ftp_server_nlist.hpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 * 
 */

#ifndef __FTP_SERVER_NLIST_HEADER__
#define __FTP_SERVER_NLIST_HEADER__

#define DEFAULT_DIRECTORY_PATH "./"

int listDirEntries(int dataSockDescriptor);
//Determine the list of current directory entries and send the list of entried to the client using 
//the data connection, represented by 'dataSockDescriptor'.
//Return the count of the entries.

#endif
