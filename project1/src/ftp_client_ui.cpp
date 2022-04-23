/**
 *  @file ftp_client_ui.hpp
 * @author Your Name, Student Number, Section, CSCI 460, VIU
 * @version 1.0.0
 * @date Date you have last modified your code in this file, e.g., August 05, 2021
 *
 * Describe the major functionalities that are performed by the code in this file.
 *
 */

//Include required library and custom header files.
#include "../include/ftp_client_ui.hpp"
#include <iostream>

//Implement all the functions prototyped in the header file ftp_client_ui.hpp

//prints the prompt and gets user's input
void getUserCommand(std::string &userCommand) {
    std::cout << "CSCI460FTP>>";
    std::getline(std::cin, userCommand);
}

//print the response
void showFtpResponse(std::string response) {
    std::cout << response << std::endl;
}