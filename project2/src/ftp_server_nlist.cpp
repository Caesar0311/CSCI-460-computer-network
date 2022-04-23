/**
 * @file ftp_server_nlist.cpp
 * @author Caesar, 578751737, N02, CSCI 460, VIU
 * @version 1.0.0
 * @date Nov 4, 2021
 *
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */



//Include required library and custom header files.
#include <unistd.h>
#include "ftp_server_nlist.hpp"
#include "ftp_server_connection.hpp"
#include "ftp_server_response.hpp"
#include <iostream>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <vector>
#include <algorithm>

using namespace std;
//Implement all the functions prototyped in the header file ftp_server_nlist.hpp

int listDirEntries(int dataSockDescriptor)
{
    char *path;
    path = get_current_dir_name();
    string dirname;
    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(path)) == nullptr) {
        sendToRemote(dataSockDescriptor, NLST_INVALID_OPTION_ERROR_RESPONSE, strlen(NLST_INVALID_OPTION_ERROR_RESPONSE));
    }
    vector<string> dirs;
    int count = 0;
    while((dirp = readdir(dp)) != nullptr)
    {
        char line[256];
        count ++;

        auto name = dirp->d_name;
        struct stat info{};
        stat(name, &info);
        char type;
        size_t size;

        if(S_ISDIR(info.st_mode)) {
            type = 'D';
            sprintf(line, "%-8c%-24s", type, name);
        } else {
            type = 'F';
            size = info.st_size;
            sprintf(line, "%-8c%-24s%d", type, name, size);
        }
        dirs.push_back(string(line));
    }
    sort(dirs.begin(), dirs.end());
    string tmp = {};
    for (auto s : dirs) {
        tmp += s;
        tmp += "\n";
    }
    char message[256 * dirs.size()];
    strcpy(message, tmp.c_str());
    sendToRemote(dataSockDescriptor, message, strlen(message));
    closedir(dp);
    return count;
}
//Determine the list of current directory entries and send the list of entried to the client using
//the data connection, represented by 'dataSockDescriptor'.
//Return the count of the entries.

