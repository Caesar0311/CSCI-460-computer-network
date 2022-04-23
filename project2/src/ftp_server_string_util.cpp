/**
 * @file ftp_server_string_util.cpp
 * @author Your Name, Student Number, Section, CSCI 460, VIU
 * @version 1.0.0
 * @date Date you have last modified your code in this file, e.g., August 05, 2021
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */



//Include required library and custom header files.

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
using namespace std;
//Implement all the functions prototyped in the header file ftp_server_string_util.hpp


void replaceAll(char* str, char find, char replace)
{
    string s = str;
    std::replace( s.begin(), s.end(), find, replace);
    strcpy(str,s.c_str());
}
//Replace all the occurrences of 'find' character in 'str' with 'replace' character.

bool startsWith(const char* str, const char* prefix)
{
    if (str == nullptr || prefix == nullptr) {
        cout << "nullptr";
    }
    return (*str == *prefix);
}
//Return true if 'str' starts with 'prefix'.

bool contains(const char* str, const char* substr)
{
    string sstr = str;
    string ssubstr = substr;
    if (sstr.find(ssubstr) != string::npos) {
        return true;
    }
    else
    {
        return false;
    }
}
//Return true if 'str' contains 'substr'

void toUpper(char* str){
    string s = str;
    transform(s.begin(),s.end(),s.begin(),::toupper);
    strcpy(str, s.c_str());
}
//Change all characters of 'str' to upper case.

void toLower(char* str)
{
    string s = str;
    transform(s.begin(),s.end(),s.begin(),::tolower);
    strcpy(str, s.c_str());
}
//Change all characters of 'str' to lower case.

void stripLeadingAndTrailingSpaces(char* str)
{
    string s = str;
    s.erase(0, s.find_first_not_of(' '));
    s.erase(s.find_last_not_of(' ') + 1);

    strcpy(str, s.c_str());
}
//Remove all the spaces, if there is any, from the beginning and the ending of 'str'.

void stripNewlineAtEnd(char* str)
{
    string s = str;
    s.erase(s.find_last_not_of('\n') + 1);
    strcpy(str, s.c_str());
}