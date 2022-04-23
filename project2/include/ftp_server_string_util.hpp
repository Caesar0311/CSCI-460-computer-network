/**
 * @file ftp_server_str_util.hpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 * 
 */

#ifndef __FTP_SERVER_STR_UTIL_HEADER__
#define __FTP_SERVER_STR_UTIL_HEADER__

void replaceAll(char* str, char find, char replace);
//Replace all the occurrences of 'find' character in 'str' with 'replace' character.

bool startsWith(const char* str, const char* prefix);
//Return true if 'str' starts with 'prefix'.

bool contains(const char* str, const char* substr);
//Return true if 'str' contains 'substr'

void toUpper(char* str);
//Change all characters of 'str' to upper case.

void toLower(char* str);
//Change all characters of 'str' to lower case.

void stripLeadingAndTrailingSpaces(char* str);
//Remove all the spaces, if there is any, from the beginning and the ending of 'str'.

void stripNewlineAtEnd(char* str);
//Remove new line character ('\n'), if there is any, from the end of 'str'.

#endif
