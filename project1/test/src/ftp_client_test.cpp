/**
 * @file ftp_client_test.cpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 24, 2020
 *
 */


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ftp_client_test

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ftp_client_connection.hpp"
#include "ftp_client_session.hpp"
#include "ftp_client_command.hpp"
#include "ftp_client_ui.hpp"
#include "ftp_server_response.hpp"
#include "ftp_client_test_net_util.hpp"

using namespace std;

/**
 * FTP Client Test Fixture
 * At the beginning of each test case, fixture's constructor is invloked
 * to start ftp server as a child process of the test program.
 * At the end of each test case, fixture's destructor is invoked to
 * stop the ftp server by terminating the child process.
 */
class FTPClientTestFixture {
	private:
		pid_t serverPID; // Remembers ftpserver process ID
	public:
		char* testHostIP;
		FTPClientTestFixture() {
			char *args[] = {const_cast<char*>("ftpserver"), const_cast<char*>("3030"), 0};
			char *env[] = {0};
			testHostIP = getTestHostIPAddress();
			serverPID = -1;
			
			serverPID = fork(); // Creates a child process
			if(serverPID == 0) {
				// Inside the child process.
				execve("./example/bin/ftpserver", args, env); // Child process is set to run ftpserver code
				exit(1);
			}	
			else { // Parent process continues
				if (serverPID == -1) {
					exit(1);
				}
				cout<<"FTP Server PID: "<<serverPID<<endl;
				sleep(3);
			}
			
		}	
		~FTPClientTestFixture() {
			if (serverPID != -1) {
				// Valid FTP Server PID, needs to stop the server by
				// sending signals to the server process.
				cout<<"Stopping FTP Server (PID: "<<serverPID<<")"<<endl;
				kill(serverPID, SIGTERM); // Sends terminate signal to ftpserver process
				sleep(2);
				kill(serverPID, SIGKILL); // Sends kill signal to ftpserver process
			}
		}
};

BOOST_FIXTURE_TEST_SUITE(ftp_client_deliverable1, FTPClientTestFixture)

/**
 * Test Case to test the functions in ftp_client_connection.cpp file.
 */
BOOST_AUTO_TEST_CASE(ftp_client_connection) {

	/*
	 * Test getTestHostIPAddress() function.
	 */

	char* hostIP = getHostIPAddress();
	BOOST_CHECK_MESSAGE(strcmp(testHostIP, hostIP) == 0, "TEST: Host IP");


	/*
	 * Test connecToServer() function.
	 */
	int socketDescriptor = -1;
	bool isConnected = false;
	connectToServer(socketDescriptor, isConnected, testHostIP, 3030);

	BOOST_CHECK_MESSAGE(isConnected, "TEST: Connected");
	
	
	/*
	 * Test receiveFromServer() function.
	 */	
	char response[40];
	memset(response, 0, 40);
	int count = -1;
	count = receiveFromServer(socketDescriptor, response, 40);
	BOOST_CHECK_MESSAGE(count==19, "TEST: Received Byte Count 19");
	BOOST_CHECK_MESSAGE(strncmp(response, CONNECTED_RESPONSE, count-1)==0, "TEST: Connected Response");


	/*
	 * Test sendToServer() function.
	 */
	int sendCount= -1;
	sendCount = sendToServer(socketDescriptor, "Hello", 5 );
	BOOST_CHECK_MESSAGE(sendCount==5, "TEST: Send Hello");

	memset(response, 0, 40);
	count = -1;
	count = receiveFromServer(socketDescriptor, response, 40);
	BOOST_CHECK_MESSAGE(count==25, "TEST: Received Byte Count 25");
	BOOST_CHECK_MESSAGE(strncmp(response, UNSUPPORTED_COMMAND_RESPONSE, count-1)==0, "TEST: Unsupported Command Response");


	/*
	 * Test disconnectFromServer() function.
	 */
	disconnectFromServer(socketDescriptor, isConnected);
	BOOST_CHECK_MESSAGE(!isConnected, "TEST: Is Disconnected");
}

/**
 * Test Case to test functions in ftp_client_control.cpp.
 */
BOOST_AUTO_TEST_CASE(ftp_client_session) {
	/*
	 * Test startClientFTPSession() function.
	 */
	ClientFtpSession clientFtpSession;
	bool isFTPSession = false;
	
	startClientFTPSession(testHostIP, 3030, clientFtpSession);
	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_CHECK_MESSAGE(isFTPSession==true, "TEST: FTP Session started");

	/*
	 * Test send using started session.
	 */
	int sendCount = -1;
	sendCount = sendToServer(clientFtpSession.controlConnection, "USER csci460", 12);
	BOOST_CHECK_MESSAGE(sendCount==12, "TEST: Send User");
	
	/*
	 * Test receive using the started session.
	 */
	char response[40];
	memset(response, 0, 40);
        int count = -1;
       	count = receiveFromServer(clientFtpSession.controlConnection, response, 40);
	BOOST_CHECK_MESSAGE(count == 34, "TEST: Received Byte Count 34");
	BOOST_CHECK_MESSAGE(strncmp(response, USERNAME_OK_RESPONSE, 33)==0,
		"TEST: User Response");
	/*
	 * Test stopClientFTPSession() function.
	 */
	stopClientFTPSession(clientFtpSession);
	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_CHECK_MESSAGE(isFTPSession==false, "TEST: FTP Session stopped");

}

BOOST_AUTO_TEST_CASE(ftp_client_command) {
	
	ClientFtpSession clientFtpSession;
	bool isFTPSession = false;
	startClientFTPSession(testHostIP, 3030, clientFtpSession);

	/*
	 * Test handleSimpleCommand() function with invalid username.
	 */
	ServerResponse serverResponse;	
	handleSimpleCommand("USER none", true, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: Simple Command - Invalid User - Response Count 1.");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), INVALID_USERNAME_RESPONSE,22)==0, 
			"TEST: Simple Command - Invalid User - Response");
	
	/*
	 * Reconnect after loosing control connection due to sending an invalid username.
	 */ 

	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_CHECK_MESSAGE(isFTPSession==false, "TEST: FTP Session stopped");

	startClientFTPSession(testHostIP, 3030, clientFtpSession);
	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_CHECK_MESSAGE(isFTPSession==true, "TEST: FTP Session started");

	/*
	 * Test handleSimpleCommand() function with valid username.
	 */
	serverResponse.count = 0;
	handleSimpleCommand("USER csci460", true, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: Simple Command - Valid User - Response Count 1");
      	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), USERNAME_OK_RESPONSE, 33)==0,
		"TEST: Simple Command - Valid User - Response");
	
	/*
	 * Test handleSimpleCommand() function with invalid password.
	 */
	serverResponse.count = 0;
	handleSimpleCommand("PASS nopass", true, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: Simple Command - Invalid Password - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), NOT_LOGGED_IN_RESPONSE, 18) == 0,
		"TEST: Simple Command - Invalid Password - Response"); 	

	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_CHECK_MESSAGE(isFTPSession==false, "TEST: FTP Session stopped");

	startClientFTPSession(testHostIP, 3030, clientFtpSession);
	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_CHECK_MESSAGE(isFTPSession==true, "TEST: FTP Session started");


	/*
	 * Test handleCommandUser() function.
	 */
	string username= "csci460";
	serverResponse.count = 0;
	handleCommandUser(username, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: USER - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), USERNAME_OK_RESPONSE,33)==0, 
			"TEST: USER - Response");

	/*
	 * Test handleCommandPassword() fucntion.
	 */
	string password= "460pass";
	serverResponse.count = 0;
	handleCommandPassword(password, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: Password - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), LOGIN_RESPONSE, 27)==0, 
			"TEST: Password - Response");

	/*
	 * Test handleCommandQuit() function.
	 */
	serverResponse.count = 0;
	handleCommandQuit(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: Quit - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), QUIT_RESPONSE, 23) == 0,
			"TEST: Quit - Response");

	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_CHECK_MESSAGE(isFTPSession==false, "TEST: FTP Session stopped");

	startClientFTPSession(testHostIP, 3030, clientFtpSession);
	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_REQUIRE(isFTPSession == true);

	ServerResponse serverResponseUser;
	interpretAndHandleUserCommand("user csci460", clientFtpSession, serverResponseUser);
	BOOST_CHECK_MESSAGE(serverResponseUser.count == 1, "TEST: Interpret and Handle Command - USER Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseUser.responses[0].c_str(), USERNAME_OK_RESPONSE,33)==0, 
			"TEST: Interpret and Handle Command - USER Response");


	
	ServerResponse serverResponsePass;
	interpretAndHandleUserCommand("pass 460pass", clientFtpSession, serverResponsePass);
	BOOST_CHECK_MESSAGE(serverResponsePass.count == 1, "TEST: Interpret and Handle Command - Password  Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponsePass.responses[0].c_str(), LOGIN_RESPONSE, 27)==0, 
			"TEST: Interpret and Handle Command - Password Response");



	ServerResponse serverResponseQuit;
	interpretAndHandleUserCommand("quit", clientFtpSession, serverResponseQuit);
	BOOST_CHECK_MESSAGE(serverResponseQuit.count == 1, "TEST: Interpret and Handle Command - Quit Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseQuit.responses[0].c_str(), QUIT_RESPONSE, 23) == 0,
			"TEST: Interpret and Handle Command - Quit Response");


	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_REQUIRE(isFTPSession==false);


}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ftp_client_deliverable2, FTPClientTestFixture) 

BOOST_AUTO_TEST_CASE(ftp_client_command) {

	ClientFtpSession clientFtpSession;
	ServerResponse serverResponse;
	startClientFTPSession(testHostIP, 3030, clientFtpSession);
	
	bool isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_REQUIRE(isFTPSession == true);

	/*
	 * Test handleCommandUser() function.
	 */
	string username= "csci460";
	serverResponse.count = 0;
	handleCommandUser(username, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: USER - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), USERNAME_OK_RESPONSE,33)==0, 
			"TEST: USER - Response");

	/*
	 * Test handleCommandPassword() fucntion.
	 */
	string password= "460pass";
	serverResponse.count = 0;
	handleCommandPassword(password, clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: Password - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), LOGIN_RESPONSE, 27)==0, 
			"TEST: Password - Response");

	/*
	 * Test handleCommandPrintDirectory() function.
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD - Response Count 1");
	char currentDir[1024];
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD - Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));
	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD - Response skipped");
	}
	

	/*
	 * Test handleCommandChangeDirectoryUp() function, while CDUP is not permitted.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectoryUp(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: CDUP - Unsuccessful Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(),CDUP_FAIL_RESPONSE, strlen(CDUP_FAIL_RESPONSE)) == 0,
			"TEST: CDUP - Unsuccessful Response");

	
	/*
	 * Test PWD after unsuccessful CDUP
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD after unsuccessful CDUP - Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after unsuccessful CDUP - Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD after unsuccessful CDUP - Response skipped");
	}
	
	/*
	 * Test handleCommandChangeDirectory() function with invalid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("nodir", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: CWD Fail - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(),CWD_FAIL_RESPONSE, strlen(CWD_FAIL_RESPONSE)) == 0,
			"TEST: CWD Fail - Response");

	/*
	 * Test PWD after unsuccessful CWD
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD after unsuccessful CWD - Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after unsuccessful CWD - Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD after unsuccessful CWD - Response skipped");
	}



	/*
	 * Test handleCommandChangeDirectory() function with invalid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("./", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: CWD Invalid Path - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(),INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE)) == 0,
			"TEST: CWD Invalid Path - Response");

	/*
	 * Test PWD after unsuccessful CWD
	 */

	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD after unsuccessful CWD - Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after unsuccessful CWD - Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD after unsuccessful CWD - Response skipped");
	}


	
	/*
	 * Test handleCommandChangeDirectory() function with invalid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("../", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: CWD Invalid Path - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(),INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE)) == 0,
			"TEST: CWD Invalid Path - Response");
	/*
	 * Test PWD after unsuccessful CWD
	 */

	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD after unsuccessful CWD - Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after unsuccessful CWD - Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD after unsuccessful CWD - Response skipped");
	}


	/*
	 * Test handleCommandChangeDirectory() function with invalid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("resource/../..", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: CWD Invalid Path - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(),INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE)) == 0,
			"TEST: CWD Invalid Path -  Response");

	/*
	 * Test PWD after unsuccessful CWD
	 */

	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD after unsuccessful CWD - Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after unsuccessful CWD - Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD after unsuccessful CWD - Response skipped");
	}


	/*
	 * Test handleCommandChangeDirectory() function with a valid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("resource", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: CWD Successful - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(),CHANGE_DIRECTORY_RESPONSE, strlen(CHANGE_DIRECTORY_RESPONSE)) == 0,
			"TEST: CWD Successful - Response");

	/*
	 * Test PWD after successful CWD
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD after successful CWD - Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after successful CWD -  Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: FTP Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD after successful CWD - Response skipped");
	}

	/*
	 * Test handleCommandChangeDirectoryUp() function, while CDUP is permitted.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectoryUp(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: CDUP Successful - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(),CHANGE_TO_PARENT_DIRECTORY_RESPONSE, 
				strlen(CHANGE_TO_PARENT_DIRECTORY_RESPONSE)) == 0,
			"TEST: CDUP Successful - Response");

	/*
	 * Test PWD after CDUP
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD after CDUP - Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after CDUP - Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD after CDUP - Response skipped");
	}



	/*
	 * Test getHostIPAndPortFromPAssiveSuccessResponse() function.
	 */
	char hostIP[20];
	memset(hostIP, 0, 20);
	int  hostPort = -1;
	const int RESPONSE_SIZE = 50;
	char response[RESPONSE_SIZE];
	memset(response, 0, RESPONSE_SIZE);
	strcpy(response, "227 Entering Passive Mode (192,168,1,65,202,143).");
	getHostIPAndPortFromPassiveSuccessResponse(response, hostIP, hostPort);

	BOOST_CHECK_MESSAGE(strncmp(hostIP, "192.168.1.65", 12) == 0, "TEST: IP from Passive Response");
	BOOST_CHECK_MESSAGE(hostPort ==51855, "TEST: Port from Passive Response");


	BOOST_CHECK_MESSAGE(isFTPSession==true, "TEST: FTP Session started");



	/*
	 * Test handleCommandDirectory() function.
	 */
	serverResponse.count = 0;
	handleCommandDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 4, "TEST: DIR - Response Count 4");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25)==0,
			"TEST: DIR - Passive Success - Response");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
				strlen(DATA_CONNECTION_SUCCESS_RESPONSE))==0,
			"TEST: DIR - Data Connection Success - Response");
	BOOST_CHECK_MESSAGE(serverResponse.responses[3].empty() == false, 
			serverResponse.responses[3].insert(0, "TEST: DIR - Directory List\n"));

	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[2].c_str(), NLST_CONNECTION_CLOSE_RESPONSE,21)==0,
			"TEST: DIR - Data Connection Close - Response");
	
	/*
	 * Test handleCommandChangeDirectory() function with a valid path.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectory("resource", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: CWD Successful - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(),CHANGE_DIRECTORY_RESPONSE, strlen(CHANGE_DIRECTORY_RESPONSE)) == 0,
			"TEST: CWD Successful - Response");

	/*
	 * Test PWD after successful CWD
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD after successful CWD - Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after successful CWD -  Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: FTP Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD after successful CWD - Response skipped");
	}
	
	
	/*
	 * Test handleCommandGetFile() function with invalid filename.
	 */
	serverResponse.count = 0;
	handleCommandGetFile("nofile", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 3, "TEST: GET Unsuccessful - Response Count 3");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25)==0,
			"TEST: GET Passive Success - Response");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
				strlen(DATA_CONNECTION_SUCCESS_RESPONSE))==0,
			"TEST: GET Data Connection Success - Response");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[2].c_str(), RETR_UNAVAILABLE_ERROR_RESPONSE, 
				strlen(RETR_UNAVAILABLE_ERROR_RESPONSE))==0,
			"TEST: GET File Unavailable - Response");

	/*
	 * Test handleCommandFile() function with valid filename.
	 */
	serverResponse.count = 0;
	handleCommandGetFile("duck.jpeg", clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 3, "TEST: GET - Success Response Count 3");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25)==0,
			"TEST: GET - Passive Success Response");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
				strlen(DATA_CONNECTION_SUCCESS_RESPONSE))==0,
			"TEST: GET - Data Connection Success Response");
	BOOST_CHECK_MESSAGE(access("duck.jpeg", F_OK) != -1, "TEST: GET - File copied");
       	
	struct stat statbuf;
	stat("duck.jpeg", &statbuf);
	BOOST_CHECK_MESSAGE(statbuf.st_size == 5594, "TEST: GET - File Size");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[2].c_str(), RETR_CONNECTION_CLOSE_RESPONSE,40)==0,
			"TEST: GET - Data Connection Close Response");


	remove("duck.jpeg");	


	/*
	 * Test PWD before CDUP
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD before CDUP - Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD before CDUP - Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD before CDUP - Response skipped");
	}



	/*
	 * Test handleCommandChangeDirectoryUp() function, while CDUP is permitted.
	 */
	serverResponse.count = 0;
	handleCommandChangeDirectoryUp(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: CDUP Successful - Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(),CHANGE_TO_PARENT_DIRECTORY_RESPONSE, 
				strlen(CHANGE_TO_PARENT_DIRECTORY_RESPONSE)) == 0,
			"TEST: CDUP Successful - Response");

	/*
	 * Test PWD after CDUP
	 */
	serverResponse.count = 0;
	handleCommandPrintDirectory(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: PWD after CDUP - Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after CDUP - Response");
		BOOST_TEST_MESSAGE(serverResponse.responses[0].insert(0, "TEST: Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("TEST: PWD after CDUP - Response skipped");
	}


	/*
	 * Test handleCommandQuit() function.
	 */
	serverResponse.count = 0;
	handleCommandQuit(clientFtpSession, serverResponse);
	BOOST_CHECK_MESSAGE(serverResponse.count == 1, "TEST: Quit -  Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponse.responses[0].c_str(), QUIT_RESPONSE, 23) == 0,
			"TEST: Quit - Response");

	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_CHECK_MESSAGE(isFTPSession==false, "TEST: FTP Session stopped");


	
	startClientFTPSession(testHostIP, 3030, clientFtpSession);
	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_CHECK_MESSAGE(isFTPSession==true, "TEST: Interpret and Handle Command - FTP Session started");

	ServerResponse serverResponseUser;
	interpretAndHandleUserCommand("user csci460", clientFtpSession, serverResponseUser);
	BOOST_CHECK_MESSAGE(serverResponseUser.count == 1, "TEST: Interpret and Handle Command - USER Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseUser.responses[0].c_str(), USERNAME_OK_RESPONSE,33)==0, 
			"TEST: Interpret and Handle Command - USER Response");



	ServerResponse serverResponsePass;
	interpretAndHandleUserCommand("pass 460pass", clientFtpSession, serverResponsePass);
	BOOST_CHECK_MESSAGE(serverResponsePass.count == 1, "TEST: Interpret and Handle Command - PASS Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponsePass.responses[0].c_str(), LOGIN_RESPONSE,33)==0, 
			"TEST: Interpret and Handle Command - PASS Response");



	ServerResponse serverResponsePwd;
	interpretAndHandleUserCommand("pwd", clientFtpSession, serverResponsePwd);
	BOOST_CHECK_MESSAGE(serverResponsePwd.count == 1, "TEST: Interpret and Handle Command - PWD Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponsePwd.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: Interpret and Handle Command - PWD Response");
	}	
	else {
		BOOST_TEST_MESSAGE("TEST: Interpret and Handle Command - PWD Response skipped");
	}
	


	ServerResponse serverResponseDir;
	interpretAndHandleUserCommand("dir", clientFtpSession, serverResponseDir);
	BOOST_CHECK_MESSAGE(serverResponseDir.count == 4, "TEST: Interpret and Handle Command - DIR Response Count 4");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseDir.responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25)==0,
			"TEST: Interpret and Handle Command - DIR Passive Success Response");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseDir.responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
				strlen(DATA_CONNECTION_SUCCESS_RESPONSE))==0,
			"TEST: Interpret and Handle Command - DIR Data Connection Success Response");
	BOOST_CHECK_MESSAGE(serverResponseDir.responses[3].empty() == false, 
			serverResponseDir.responses[3].insert(0, "TEST: Interpret and Handle Command - DIR Directory List\n"));

	BOOST_CHECK_MESSAGE(strncmp(serverResponseDir.responses[2].c_str(), NLST_CONNECTION_CLOSE_RESPONSE,21)==0,
			"TEST: Interpret and Handle Command - DIR Data Connection Close Response");
	

	ServerResponse serverResponseCwd;
	interpretAndHandleUserCommand("cwd resource", clientFtpSession, serverResponseCwd);
	BOOST_CHECK_MESSAGE(serverResponseCwd.count == 1, "TEST: Interpret and Handle Command - CWD Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseCwd.responses[0].c_str(),CHANGE_DIRECTORY_RESPONSE, strlen(CHANGE_DIRECTORY_RESPONSE)) == 0,
			"TEST: Interpret and Handle Command - CWD Response");




	serverResponsePwd.count = 0 ;
	interpretAndHandleUserCommand("pwd", clientFtpSession, serverResponsePwd);
	BOOST_CHECK_MESSAGE(serverResponsePwd.count == 1, "TEST: Interpret and Handle Command - PWD Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponsePwd.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: Interpret and Handle Command - PWD Response");
	}	
	else {
		BOOST_TEST_MESSAGE("TEST: Interpret and Handle Command - PWD Response skipped");
	}
	

	ServerResponse serverResponseGet;
	interpretAndHandleUserCommand("get duck.jpeg", clientFtpSession, serverResponseGet);
	BOOST_CHECK_MESSAGE(serverResponseGet.count == 3, "TEST: Interpret and Handle Command - GET Response Count 3");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseGet.responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25)==0,
			"TEST: Interpret and Handle Command - GET Passive Success Response");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseGet.responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
				strlen(DATA_CONNECTION_SUCCESS_RESPONSE))==0,
			"TEST: Interpret and Handle Command - GET Data Connection Success Response");
	BOOST_CHECK_MESSAGE(access("duck.jpeg", F_OK) != -1, "TEST: Interpret and Handle Command - GET File");
       	
	struct stat statbuf2;
	stat("duck.jpeg", &statbuf2);
	BOOST_CHECK_MESSAGE(statbuf2.st_size == 5594, "TEST: Interpret and Handle Command - GET File Size");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseGet.responses[2].c_str(), RETR_CONNECTION_CLOSE_RESPONSE,40)==0,
			"TEST: Interpret and Handle Command - GET Data Connection Close");


	remove("duck.jpeg");	



	ServerResponse serverResponseCdup;
	interpretAndHandleUserCommand("cdup", clientFtpSession, serverResponseCdup);
	BOOST_CHECK_MESSAGE(serverResponseCdup.count == 1, "TEST: Interpret and Handle Command - CDUP Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseCdup.responses[0].c_str(),CHANGE_TO_PARENT_DIRECTORY_RESPONSE, 
				strlen(CHANGE_TO_PARENT_DIRECTORY_RESPONSE)) == 0,
			"TEST: Interpret and Handle Command - CDUP Response");


	serverResponsePwd.count = 0;
	interpretAndHandleUserCommand("pwd", clientFtpSession, serverResponsePwd);
	BOOST_CHECK_MESSAGE(serverResponsePwd.count == 1, "TEST: Interpret and Handle Command - PWD Response Count 1");
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(serverResponsePwd.responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: Interpret and Handle Command - PWD Response");
	}	
	else {
		BOOST_TEST_MESSAGE("TEST: Interpret and Handle Command - PWD Response skipped");
	}
	

	ServerResponse serverResponseQuit;
	interpretAndHandleUserCommand("quit", clientFtpSession, serverResponseQuit);
	BOOST_CHECK_MESSAGE(serverResponseQuit.count == 1, "TEST: Interpret and Handle Command - Quit Response Count 1");
	BOOST_CHECK_MESSAGE(strncmp(serverResponseQuit.responses[0].c_str(), QUIT_RESPONSE, 23) == 0,
			"TEST: Interpret and Handle Command - Quit Response");


	isFTPSession = clientFtpSession.controlConnection > 0 && clientFtpSession.isControlConnected;
	BOOST_CHECK_MESSAGE(isFTPSession==false, "TEST: Interpret and Handle Command - FTP Session stopped");
	if(access("duck.jpeg", F_OK) != -1) {
       		remove("duck.jpeg");
	}	

}

BOOST_AUTO_TEST_SUITE_END()

