/**
 *  @file ftp_client_ui.hpp
 *  @author Humayun Kabir, Instructor, CSCI 460, VIU
 *  @version 1.0.0
 *  @date June 22, 2020
 */

#ifndef __FTP_CLIENT_UI_HEADER__
#define __FTP_CLIENT_UI_HEADER__

#include <string>

#define FTP_CLIENT_PROMT "CSCI460FTP>>"


void getUserCommand(std::string& userCommand);
// Displays a command line prompt as follows:
//      CSCI460FTP>>
// Lets user to type command and reads the user command from the prompt.


void showFtpResponse(std::string response); 
// Displays server response to the user.
// For example, success response of user command
//      CSCI460FTP>>user csci460
// should be displayes as 
//      331 Username okay, need password.
//
// Success response of user command
//      CSCI460FTP>>dir
// should be  displayed as 
//      226 Directory listing (11) entries. Closing data connection.
//
//      D       .
//      D       ..
//      D       bin
//      F       makefile                2631
//      D       include
//      F       README                  0
//      D       build
//      D       resource
//      D       src

#endif
