/**
 * @file ftp_server_test.cpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 24, 2020
 *
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ftp_server_test

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "ftp_server_string_util.hpp"
#include "ftp_server_net_util.hpp"
#include "ftp_server_connection.hpp"
#include "ftp_server_connection_listener.hpp"
#include "ftp_server_request.hpp"
#include "ftp_server_session.hpp"
#include "ftp_server_passive.hpp"
#include "ftp_server_response.hpp"

#include "ftp_client_test_connection.hpp"
#include "ftp_client_test_control.hpp"
#include "ftp_client_test_command.hpp"
#include "ftp_server_test_net_util.hpp"

#define TEST_SERVER_PORT 3030
#define TEST_PORT_FIRST 4000

using namespace std;

/** 
 * ********************************* Test Code for Deliverable 1 ************************************* 
 */


BOOST_AUTO_TEST_SUITE(ftp_server_deliverable1)

BOOST_AUTO_TEST_CASE(ftp_server_string_util) {
	char* str = strdup("192,168,100,102");
	const char* replacedStr = "192.168.100.102";
	replaceAll(str, ',', '.');
	BOOST_CHECK_MESSAGE(strncmp(str, replacedStr, 16 ) == 0, "TEST: replaceAll()");
	free(str);


	BOOST_CHECK(startsWith("./path", "./") == true);
       	BOOST_CHECK(startsWith("../path/subpath", "../") == true);
	BOOST_CHECK(startsWith("../path/next", "./") == false);	
	BOOST_CHECK(startsWith("/home/somebody", "/") == true);

	BOOST_CHECK(contains("/path/../..", "/.") == true);
	BOOST_CHECK(contains("./../path", "./") == true);
	BOOST_CHECK(contains("path/*", "*") == true);
	BOOST_CHECK(contains("path/subpath", "./") == false);


	char* lower = strdup("hello to upper test");
	const char* convUpper = "HELLO TO UPPER TEST";
	toUpper(lower);
	BOOST_CHECK_MESSAGE(strcmp(lower, convUpper) == 0, "TEST: toUpper()");
	free(lower);

	char* upper = strdup("HELLO TO LOWER TEST");
	const char* convLower = "hello to lower test";
	toLower(upper);
	BOOST_CHECK_MESSAGE(strcmp(convLower, upper) == 0, "TEST: toLower()");
	free(upper);

	char* withSpace = strdup("        I have space at my beginning and end         ");
	const char* noSpace = "I have space at my beginning and end";
	stripLeadingAndTrailingSpaces(withSpace);
	BOOST_CHECK_MESSAGE(strcmp(withSpace, noSpace) == 0, "TEST: stripLeadingAndTrailingSpaces()");
	free(withSpace);

	char* withNewLine = strdup("I have new lines\n\n\n\n");
	const char* noNewLine = "I have new lines";
	stripNewlineAtEnd(withNewLine);
	BOOST_CHECK_MESSAGE(strcmp(withNewLine, noNewLine) == 0, "TEST: stripNewlineAtEnd()"); 
	free(withNewLine);
}


BOOST_AUTO_TEST_CASE(ftp_server_net_util) {

	char* port = const_cast<char*>("4010");	
	/*
	 * Testing getHostIPAddress() function.
	 */
	BOOST_CHECK_MESSAGE((strcmp(getHostIPAddress(), getTestHostIP()) == 0), 
			"TEST: Host IP Address");

	
	/*
	 * Starting a test listener socket, which will be used in the tests.
	 */
	
	int listenerSocket = -1;

	struct sockaddr_in socketAddress;
	memset(&socketAddress, 0, sizeof(struct sockaddr_in));
	bool isListener = false;

	getTestListenerSocket(port, listenerSocket, &socketAddress, isListener);

	/*
	 * Testing getPortFromSocketDescriptor() function.
	 */
	int gotPort = getPortFromSocketDescriptor(listenerSocket);
	BOOST_CHECK_MESSAGE(gotPort == 4010, "TEST: Port from Socket Descriptor");


	
	/*
	 * Testing isSocketReadyToRead() function Timeout.
	 */
	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	bool isError = false;
	bool isTimeout = false;
	bool isListenerReady = false;
		
	isListenerReady = isSocketReadyToRead(listenerSocket, timeout.tv_sec, timeout.tv_usec, isError, isTimeout);
	BOOST_CHECK_MESSAGE(isError == false, "TEST: Is Socket Ready to Read - Not in Error");
	BOOST_CHECK_MESSAGE(isTimeout == true, "TEST: Is Socket Ready to Read - Timeout");
	BOOST_CHECK_MESSAGE(isListenerReady == false, "TEST: Is Socket NOT Ready to Read.");



	/*
	 * Testing isSocketReadyToRead() function Ready.
	 * A child process is created and a connect request is sent from the child
	 * process to the listener socket so that the listener socket in the main
	 * process becomes ready to read.
	 * isSocketReadyToRead() function is called on the ready socket.
	 */ 
	pid_t childProcessID = fork();

	if(childProcessID == 0) {
		//child process

		closeTestSocket(listenerSocket); // child does not need the listener socket
		listenerSocket = -1;
		int clientSocket;
		connectToTestRemoteAddress(&socketAddress, clientSocket);

		sleep(5);
		closeTestSocket(clientSocket);
	       	clientSocket = -1;	
		exit(0);
	} 
	else { 
		//parent process;
		sleep(2);
				
		isError = false;
		isTimeout = false;
		isListenerReady = false;
		isListenerReady = isSocketReadyToRead(listenerSocket, timeout.tv_sec, timeout.tv_usec, isError, isTimeout);
		BOOST_CHECK_MESSAGE(isError == false, "TEST: Is Socket Ready to Read - Not in Error");
		BOOST_CHECK_MESSAGE(isTimeout == false, "TEST: Is Socket Ready to Read - Not Timeout");
		BOOST_CHECK_MESSAGE(isListenerReady, "TEST: Is Socket Ready to Read.");
		
		/*
		 * Testing closeSocket() function.
		 */
		closeSocket(listenerSocket);
		BOOST_CHECK_MESSAGE(isTestSocketClosed(listenerSocket), "TEST: Socket Closed");


	}

}


BOOST_AUTO_TEST_CASE(ftp_server_connection) {

	char* port = const_cast<char*>("4020");
	int listenerSocket = -1;
	struct sockaddr_in socketAddress;
	memset(&socketAddress, 0, sizeof(struct sockaddr_in));
	bool isListener = false;


	getTestListenerSocket(port, listenerSocket, &socketAddress, isListener);

	char* message = const_cast<char*>("Message to test send and receive");
	char received[strlen(message)+1];

	pid_t childProcessID = fork();

	if(childProcessID == 0) {
		//child process

		closeTestSocket(listenerSocket); // child does not need the listener socket
		
		int clientSocket = -1;

		connectToTestRemote(getTestHostIP(), 4020, clientSocket);

		//BOOST_CHECK_MESSAGE(clientSocket > 0, "TEST: Client Connected");
		sleep(1);

		sendMessageToTestRemote(clientSocket, message, strlen(message));
		sleep(1);

		/*
		 * Testing sendToRemote() server function at the client side
		 * by receiving the sent message from the server side.
		 */
		struct timeval timeout;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		bool isError = false;
		bool isTimeout = false;
		bool isClientReady = isTestSocketReadyToRead(clientSocket, &timeout, isError, isTimeout);
		//BOOST_CHECK_MESSAGE(isError == false, "TEST: Test Client Connection Ready Not in Error");
		//BOOST_CHECK_MESSAGE(isTimeout == false, "TEST: Test Client Connection Ready  Not Timeout");
		BOOST_CHECK_MESSAGE(isClientReady, "TEST: Send To Remote - Ready");

		int len = sizeof(received);
		memset(received, 0, len);

		receiveMessageFromTestRemote(clientSocket, received, len);

		BOOST_CHECK_MESSAGE(strcmp(message, received) == 0, "TEST: Send To Remote");
		

		sleep(3);
		closeTestSocket(clientSocket); 
		exit(0);
	} 
	else { 
		//parent process;
		sleep(2);
		struct timeval timeout;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		bool isError = false;
		bool isTimeout = false;
		bool isListenerReady = isTestSocketReadyToRead(listenerSocket, &timeout, isError, isTimeout);
		//BOOST_CHECK_MESSAGE(isError == false, "TEST: Test Listener Ready Not in Error");
		//BOOST_CHECK_MESSAGE(isTimeout == false, "TEST: Test Listener Ready Not Timeout");
		BOOST_REQUIRE(isListenerReady);

		int clientSockDescriptor = -1;
		struct sockaddr_in peerAddress;
		acceptTestRemoteConnection(listenerSocket, clientSockDescriptor, &peerAddress); 
		BOOST_REQUIRE(clientSockDescriptor > 0);
		sleep(1);

		closeTestSocket(listenerSocket);

		
		isError = false;
		isTimeout = false;
		bool isClientSocketReady = isTestSocketReadyToRead(clientSockDescriptor, &timeout, isError, isTimeout);
		//BOOST_CHECK_MESSAGE(isError == false, "TEST: Is Test Connection Ready Not in Error");
		//BOOST_CHECK_MESSAGE(isTimeout == false, "TEST: Is Test Connection Ready Not Timeout");
		BOOST_REQUIRE(isClientSocketReady);


		/*
		 * Testing isConnectionReadyToRead() function.
		 */
		isError = false;
		isTimeout = false;
		bool isConnectionReady = isConnectionReadyToRead(clientSockDescriptor, 0, 0, isError, isTimeout);
		BOOST_CHECK_MESSAGE(isError == false, "TEST: Is Connection Ready Not in Error");
		BOOST_CHECK_MESSAGE(isTimeout == false, "TEST: Is Connection Ready  Not Timeout");
		BOOST_CHECK_MESSAGE(isConnectionReady, "TEST: Is Connection Ready");

		/*
		 * Testing recieveFromRemote() function.
		 */
		memset(received, 0, sizeof(received));

		receiveFromRemote(clientSockDescriptor, received, sizeof(received));

		BOOST_CHECK_MESSAGE(strcmp(message, received) == 0, "TEST: Receive From Remote");

		sleep(1);

		/*
		 * Testing sendToRemote() function.
		 */
		sendToRemote(clientSockDescriptor, message, strlen(message));

		sleep(3); 

		closeConnection(clientSockDescriptor);	
		BOOST_CHECK_MESSAGE(isTestSocketClosed(clientSockDescriptor), "TEST: Connection Closed");

	}

}


BOOST_AUTO_TEST_CASE(ftp_server_connection_listener) {

	/*
	 * Testing startListenerSocket() function.
	 */
	char* port = const_cast<char*>("4030");
	int listenerSocket = -1;
	bool isListener = false;
	startListenerSocket(port, listenerSocket, isListener);

	BOOST_CHECK_MESSAGE(isListener, "TEST: Start Listener Socket");
        BOOST_CHECK_MESSAGE(listenerSocket>0, "TEST: Start Listener Socket - Descriptor");	

	pid_t childProcessID = fork();

	if(childProcessID == 0) {
		//child process

		closeTestSocket(listenerSocket); // child does not need the listener socket
		
		int clientSocket = -1;
		
		connectToTestRemote(getTestHostIP(), 4030, clientSocket);

		sleep(2);
		
		closeTestSocket(clientSocket);
	        clientSocket = -1;
		
		sleep(2);	
		
		connectToTestRemote(getTestHostIP(), 4030, clientSocket);

		sleep(2);

		closeTestSocket(clientSocket);
		
		sleep(2);

		exit(0);
	} 
	else { 
		//parent process;
		sleep(2);
		struct timeval timeout;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		bool isError = false;
		bool isTimeout = false;
		bool isListenerReady = isTestSocketReadyToRead(listenerSocket, &timeout, isError, isTimeout);

		//BOOST_CHECK_MESSAGE(isError == false, "TEST: Start Listener Socket - Ready - Not in Error");
		//BOOST_CHECK_MESSAGE(isTimeout == false, "TEST: Start Listener Socket - Ready - Not Timeout");
		BOOST_CHECK_MESSAGE(isListenerReady, "TEST: Start Listener Socket - Ready");
		
		int clientSockDescriptor = -1;
		struct sockaddr_in peerAddress;
		acceptTestRemoteConnection(listenerSocket, clientSockDescriptor, &peerAddress); 
		BOOST_CHECK_MESSAGE(clientSockDescriptor > 0, "TEST: Start Listener Socket -  Accepted Connection");
		closeTestSocket(clientSockDescriptor);
		clientSockDescriptor = -1;

		sleep(2);

		/*
		 * Testing isListenerSocketReady() function.
		 */
		isError = false;
		isTimeout = false;
		isListenerReady = false;
		isListenerReady = isListenerSocketReady(listenerSocket, timeout.tv_sec, timeout.tv_usec, isError, isTimeout);
		BOOST_CHECK_MESSAGE(isError == false, "TEST: Is Listener Socket Ready -  Not in Error");
		BOOST_CHECK_MESSAGE(isTimeout == false, "TEST: Is Listener Socket Ready - Not Timeout");
		BOOST_CHECK_MESSAGE(isListenerReady, "TEST: Is Listener Socket Ready");

		/*
		 * Testing acceptClientConnection() function.
		 */
		acceptClientConnection(listenerSocket, clientSockDescriptor);
		BOOST_CHECK_MESSAGE(clientSockDescriptor > 0, "TEST: Accept Client Connection");
		
		closeTestSocket(clientSockDescriptor);
	
		
		closeListenerSocket(listenerSocket);
		BOOST_CHECK_MESSAGE(isTestSocketClosed(listenerSocket), "TEST: Listener Socket Closed");

	
	}

}



BOOST_AUTO_TEST_SUITE_END()


/**
 * FTP Server Test Fixture
 * At the beginning of each test case, fixture's constructor is invloked
 * to start ftp server as a child process of the test program.
 * At the end of each test case, fixture's destructor is invoked to
 * stop the ftp server by terminating the child process.
 */
class FTPServerTestFixture {
	private:
		pid_t serverPID; // Remembers ftpserver process ID
	public:
		FTPServerTestFixture() {
			char* codeToRun = const_cast<char*>("ftpserver");
			char port[10];
		       	sprintf(port, "%d", TEST_SERVER_PORT);
			char *args[] = {codeToRun, port, 0};
			char *env[] = {0};
			serverPID = -1;
			
			serverPID = fork(); // Creates a child process
			if(serverPID == 0) {
				// Inside the child process.
				execve("./bin/ftpserver", args, env); // Child process is set to run ftpserver code
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
		~FTPServerTestFixture() {
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


	
/** 
 * ********************************* Test Code for Deliverable 2 ************************************* 
 */


BOOST_AUTO_TEST_SUITE(ftp_server_deliverable2)

BOOST_AUTO_TEST_CASE(ftp_server_request_unsupported) {
		int listenerSocket = -1;
	 	struct sockaddr_in socketAddress;
		memset(&socketAddress, 0, sizeof(struct sockaddr_in));
		bool isListener = false;

		getTestListenerSocket(const_cast<char*>("4040"), listenerSocket, &socketAddress, isListener);

		pid_t childPID = fork();

		if(childPID == 0) {
			//child process
			closeTestSocket(listenerSocket);
			
			int clientSocket = -1;

			connectToTestRemote(getTestHostIP(), 4040, clientSocket);

			BOOST_REQUIRE(clientSocket > 0);

			/*
			 * Test Receive Unsupported Command Response.
			 */
			char response[40];
			memset(response, 0, 40);
			int responseLength = -1;
			int expectedLength = strlen(UNSUPPORTED_COMMAND_RESPONSE);
			responseLength = receiveFromServer(clientSocket, response, 40);
			BOOST_REQUIRE(responseLength==expectedLength);
			BOOST_CHECK_MESSAGE(strncmp(response, UNSUPPORTED_COMMAND_RESPONSE, responseLength-1)==0, 
					"TEST: FTP Request - UNSUPPORTED_COMMAND_RESPONSE");
			

			sleep(3);
			closeTestSocket(clientSocket);
			exit(0);
			
		}
		else {
			//parent process
			struct timeval timeout;
			timeout.tv_sec = 2;
			timeout.tv_usec = 0;
			bool isError = false;
			bool isTimeout = false;
			bool isListenerReady = isTestSocketReadyToRead(listenerSocket, &timeout, isError, isTimeout);
			BOOST_REQUIRE(isListenerReady);

			int clientSockDescriptor = -1;
			struct sockaddr_in peerAddress;
			acceptTestRemoteConnection(listenerSocket, clientSockDescriptor, &peerAddress); 
			BOOST_REQUIRE(clientSockDescriptor > 0);

			closeTestSocket(listenerSocket);

			ClientFtpSession clientFtpSession;
			clientFtpSession.controlConnection = clientSockDescriptor;
			clientFtpSession.isControlConnected = true;

			/*
			 * Test handleFtpRequestUnsupported() function.
			 */
			handleFtpRequestUnSupported(clientFtpSession);
			sleep(3);

			closeTestSocket(clientSockDescriptor);

		}
}
	

BOOST_AUTO_TEST_CASE(ftp_server_request_quit) {
		int listenerSocket = -1;
	 	struct sockaddr_in socketAddress;
		memset(&socketAddress, 0, sizeof(struct sockaddr_in));
		bool isListener = false;

		getTestListenerSocket(const_cast<char*>("4050"), listenerSocket, &socketAddress, isListener);

		pid_t childPID = fork();

		if(childPID == 0) {
			//child process
			closeTestSocket(listenerSocket);
			
			int clientSocket = -1;

			connectToTestRemote(getTestHostIP(), 4050, clientSocket);
			
			BOOST_REQUIRE(clientSocket>0);
		
			/*
			 * Test Receive QUIT Response
			 */
			char response[40];
			memset(response, 0, 40);
			int responseLength = -1;
			int expectedLength = strlen(QUIT_RESPONSE);
			responseLength = receiveFromServer(clientSocket, response, 40);
			BOOST_REQUIRE(responseLength==expectedLength);
			BOOST_CHECK_MESSAGE(strncmp(response, QUIT_RESPONSE, responseLength-1)==0, 
					"TEST: Handle FTP Request QUIT");

			sleep(3);
			closeTestSocket(clientSocket);
			exit(0);

		}
		else {
			//parent process
			struct timeval timeout;
			timeout.tv_sec = 2;
			timeout.tv_usec = 0;
			bool isError = false;
			bool isTimeout = false;
			bool isListenerReady = isTestSocketReadyToRead(listenerSocket, &timeout, isError, isTimeout);
			BOOST_REQUIRE(isListenerReady);

			int clientSockDescriptor = -1;
			struct sockaddr_in peerAddress;
			acceptTestRemoteConnection(listenerSocket, clientSockDescriptor, &peerAddress); 
			BOOST_REQUIRE(clientSockDescriptor > 0);

			closeTestSocket(listenerSocket);

			ClientFtpSession clientFtpSession;
			clientFtpSession.controlConnection = clientSockDescriptor;
			clientFtpSession.isControlConnected = true;


			/*
			 * Test handleFtpRequestQUIT() function.
			 */
			handleFtpRequestQUIT(clientFtpSession);

			sleep(3);
			closeTestSocket(clientSockDescriptor);

		}

}


BOOST_AUTO_TEST_CASE(ftp_server_request_interpret) {
		int listenerSocket = -1;
	 	struct sockaddr_in socketAddress;
		memset(&socketAddress, 0, sizeof(struct sockaddr_in));
		bool isListener = false;

		getTestListenerSocket(const_cast<char*>("4060"), listenerSocket, &socketAddress, isListener);

		pid_t childPID = fork();

		if(childPID == 0) {
			//child process
			closeTestSocket(listenerSocket);
			
			int clientSocket = -1;

			connectToTestRemote(getTestHostIP(), 4060, clientSocket);

			BOOST_REQUIRE(clientSocket>0);
		
		
			/*
			 * Test Receive Unsupported Command Response.
			 */
			char response[40];
			memset(response, 0, 40);
			int responseLength = -1;
			int expectedLength = strlen(UNSUPPORTED_COMMAND_RESPONSE);
			responseLength = receiveFromServer(clientSocket, response, 40);
			BOOST_REQUIRE(responseLength==expectedLength);
			BOOST_CHECK_MESSAGE(strncmp(response, UNSUPPORTED_COMMAND_RESPONSE, responseLength-1)==0, 
					"TEST: Interpret FTP Request - UNSUPPORTED_COMMAND_RESPONSE");
			


			/*
			 * Test Receive QUIT Response
			 */
			memset(response, 0, 40);
			responseLength = -1;
			expectedLength = strlen(QUIT_RESPONSE);
			responseLength = receiveFromServer(clientSocket, response, 40);
			BOOST_REQUIRE(responseLength==expectedLength);
			BOOST_CHECK_MESSAGE(strncmp(response, QUIT_RESPONSE, responseLength-1)==0, 
					"TEST: Interpret FTP Request -  QUIT");

			sleep(3);
			closeTestSocket(clientSocket);
			exit(0);

		}
		else {
			//parent process
			struct timeval timeout;
			timeout.tv_sec = 2;
			timeout.tv_usec = 0;
			bool isError = false;
			bool isTimeout = false;
			bool isListenerReady = isTestSocketReadyToRead(listenerSocket, &timeout, isError, isTimeout);
			BOOST_REQUIRE(isListenerReady);

			int clientSockDescriptor = -1;
			struct sockaddr_in peerAddress;
			acceptTestRemoteConnection(listenerSocket, clientSockDescriptor, &peerAddress); 
			BOOST_REQUIRE(clientSockDescriptor > 0);

			closeTestSocket(listenerSocket);

			ClientFtpSession clientFtpSession;
			clientFtpSession.controlConnection = clientSockDescriptor;
			clientFtpSession.isControlConnected = true;


			/*
			 * Test interpretFtpRequest() function.
			 */
			interpretFtpRequest("Unsupported", clientFtpSession);

			sleep(1);

			interpretFtpRequest("QUIT", clientFtpSession);

			sleep(3);
			closeTestSocket(clientSockDescriptor);

		}

}



BOOST_AUTO_TEST_CASE(ftp_server_session) {
		int listenerSocket = -1;
	 	struct sockaddr_in socketAddress;
		memset(&socketAddress, 0, sizeof(struct sockaddr_in));
		bool isListener = false;

		getTestListenerSocket(const_cast<char*>("4070"), listenerSocket, &socketAddress, isListener);

		pid_t childPID = fork();

		if(childPID == 0) {
			//child process
			closeTestSocket(listenerSocket);
			
			int clientSocket = -1;

			connectToTestRemote(getTestHostIP(), 4070, clientSocket);

			BOOST_REQUIRE(clientSocket > 0);



			/*
			 * Test connected response.
			 */	
			char response[40];
			int expectedLength = strlen(CONNECTED_RESPONSE);
			memset(response, 0, 40);
			int responseLength = -1;
			responseLength = receiveFromServer(clientSocket, response, 40);
			BOOST_REQUIRE(responseLength==expectedLength);
			BOOST_CHECK_MESSAGE(strncmp(response, CONNECTED_RESPONSE, responseLength-1)==0, 
					"TEST: Start Client FTP Session - CNNECTED RESPONSE");

			
			/*
			 * Test Send Unsupported Command and Receive Unsupported Response.
			 */
			int sendCount= -1;
			sendCount = sendToServer(clientSocket, "Unsupported", 11 );
			BOOST_CHECK_MESSAGE(sendCount==11, "TEST: Start Client FTP Session -  Send Unsupported");

			sleep(1);
			memset(response, 0, 40);
			responseLength = -1;
			expectedLength = strlen(UNSUPPORTED_COMMAND_RESPONSE);
			responseLength = receiveFromServer(clientSocket, response, 40);
			BOOST_REQUIRE(responseLength==expectedLength);
			BOOST_CHECK_MESSAGE(strncmp(response, UNSUPPORTED_COMMAND_RESPONSE, responseLength-1)==0, 
					"TEST: Start Client FTP Session - UNSUPPORTED_COMMAND_RESPONSE");

			/*
			 * Test Send QUIT and Receive Response
			 */
			sendCount= -1;
			sendCount = sendToServer(clientSocket, "QUIT", 4 );
			BOOST_CHECK_MESSAGE(sendCount==4, "TEST: Stop Client FTP Session -  Send QUIT");

			sleep(1);
			memset(response, 0, 40);
			responseLength = -1;
			expectedLength = strlen(QUIT_RESPONSE);
			responseLength = receiveFromServer(clientSocket, response, 40);
			BOOST_REQUIRE(responseLength==expectedLength);
			BOOST_CHECK_MESSAGE(strncmp(response, QUIT_RESPONSE, responseLength-1)==0, 
					"TEST: Stop Client FTP Session - QUIT_RESPONSE");

			
			sleep(3);
			closeTestSocket(clientSocket);
			exit(0);

		}
		else {
			//parent process
			struct timeval timeout;
			timeout.tv_sec = 2;
			timeout.tv_usec = 0;
			bool isError = false;
			bool isTimeout = false;
			bool isListenerReady = isTestSocketReadyToRead(listenerSocket, &timeout, isError, isTimeout);
			BOOST_REQUIRE(isListenerReady);

			int clientSockDescriptor = -1;
			struct sockaddr_in peerAddress;
			acceptTestRemoteConnection(listenerSocket, clientSockDescriptor, &peerAddress); 
			BOOST_REQUIRE(clientSockDescriptor > 0);

			closeTestSocket(listenerSocket);

			ClientFtpSession clientFtpSession;
			clientFtpSession.controlConnection = clientSockDescriptor;
			clientFtpSession.isControlConnected = true;

			/*
			 * Test startClientFTPSession() function.
			 */
			startClientFTPSession(clientFtpSession);

			sleep(3);

		}

}


BOOST_FIXTURE_TEST_CASE(ftp_server_request, FTPServerTestFixture) {

	/*
	 * Test connection.
	 */
	connectToServerControl(getTestHostIP(), TEST_SERVER_PORT);
	sleep(2);
	BOOST_REQUIRE(isControlConnectionAvailable());

	const int RESPONSE_SIZE = 50;
	char response[RESPONSE_SIZE];


	/*
	 * Test receieve connected.
	 */
	memset(response, 0, RESPONSE_SIZE);
        int responseLength = -1;
	int expectedLength = strlen(CONNECTED_RESPONSE);
       	responseLength = receiveOnControl(response, RESPONSE_SIZE);
	BOOST_REQUIRE(responseLength == expectedLength);
	BOOST_CHECK_MESSAGE(strncmp(response, CONNECTED_RESPONSE, responseLength-1)==0, "TEST: Connected Response");

	/*
	 * Test USER with invalid username.
	 */
	string responses[MAX_NUMBER_OF_RESPONSES];
	int responseCount = 0;	
	handleSimpleCommand("USER nobody", false, responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), INVALID_USERNAME_RESPONSE,22)==0, 
			"TEST: Invalid User Response");
	
	/*
	 * Reconnect after loosing control connection due to sending an invalid username.
	 */ 
	BOOST_TEST_MESSAGE("INFO: Client Connection is lost after Invalid User");

	connectToServerControl(getTestHostIP(), TEST_SERVER_PORT);
	BOOST_REQUIRE(isControlConnectionAvailable());
	
	memset(response, 0, RESPONSE_SIZE);
        responseLength = -1;
       	responseLength = receiveOnControl(response, RESPONSE_SIZE);
	BOOST_REQUIRE(responseLength == expectedLength);
	BOOST_CHECK_MESSAGE(strncmp(response, CONNECTED_RESPONSE, responseLength-1)==0, "TEST: Connected Response");

	/*
	 * Test USER with valid username.
	 */
	responseCount = 0;
	handleSimpleCommand("USER csci460", false, responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
      	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), USERNAME_OK_RESPONSE, 33)==0,
		"TEST: Valid User Response");
	
	/*
	 * Test PASS with invalid password.
	 */
	responseCount = 0;
	handleSimpleCommand("PASS nopass", false, responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), NOT_LOGGED_IN_RESPONSE, 18) == 0,
		"TEST: Client Invalid Password Response"); 	

	/*
	 * Reconnect after loosing control connection due to sending an invalid password.
	 */
	BOOST_TEST_MESSAGE("INFO: Client Connection is lost after Invalid Password");


	connectToServerControl(getTestHostIP(), TEST_SERVER_PORT);
	BOOST_REQUIRE(isControlConnectionAvailable());
	
	memset(response, 0, RESPONSE_SIZE);
        responseLength = -1;
       	responseLength = receiveOnControl(response, RESPONSE_SIZE);
	BOOST_REQUIRE(responseLength == expectedLength);
	BOOST_CHECK_MESSAGE(strncmp(response, CONNECTED_RESPONSE, responseLength-1)==0, 
			"TEST: Connected Response");


	/*
	 * Test USER with valid username.
	 */
	string username= "csci460";
	responseCount = 0;
	handleCommandUser(username, responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), USERNAME_OK_RESPONSE,33)==0, 
			"TEST: Valid USER Response");

	/*
	 * Test PASS with vaild password.
	 */
	string password= "460pass";
	responseCount = 0;
	handleCommandPassword(password, responses, responseCount);

	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), LOGIN_RESPONSE, 27)==0, 
			"TEST: Valid Password Response");

	/*
	 * Test PWD.
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	char currentDir[1024];
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: Client PWD Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));
	}	
	else {
		BOOST_TEST_MESSAGE("INFO: Client PWD Response skipped");
	}
	


	/*
	 * Test PWD before unsuccessful CDUP
	 */
/*	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD before unsuccessful CDUP Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "TEST: FTP Server Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("INFO: PWD before unsuccessful CDUP Response skipped");
	}
*/


	/*
	 * Test CDUP, while it is not permitted.
	 */
	responseCount = 0;
	handleCommandChangeDirectoryUp(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),CDUP_FAIL_RESPONSE, strlen(CDUP_FAIL_RESPONSE)) == 0,
			"TEST: CDUP Unsuccessful Response");

	
	/*
	 * Test PWD after unsuccessful CDUP
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after unsuccessful CDUP  Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("INFO: PWD after unsuccessful CDUP Response skipped");
	}

	/*
	 * Test PWD before unsuccessful CWD
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD before unsuccessful CWD  Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("INFO: PWD before unsuccessful CWD Response skipped");
	}
	
	
	/*
	 * Test CWD with invalid path.
	 */
	responseCount = 0;
	handleCommandChangeDirectory("nodir", responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),CWD_FAIL_RESPONSE, strlen(CWD_FAIL_RESPONSE)) == 0,
			"TEST: CWD Fail Response");


	/*
	 * Test CWD with invalid path.
	 */
	responseCount = 0;
	handleCommandChangeDirectory("./", responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE)) == 0,
			"TEST: CWD Invalid Path Response");

	/*
	 * Test CWD with invalid path.
	 */
	responseCount = 0;
	handleCommandChangeDirectory("..", responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE)) == 0,
			"TEST: CWD Invalid Path Response");


	/*
	 * Test CWD with invalid path.
	 */
	responseCount = 0;
	handleCommandChangeDirectory("../", responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE)) == 0,
			"TEST: CWD Invalid Path Response");

	/*
	 * Test CWD with invalid path.
	 */
	responseCount = 0;
	handleCommandChangeDirectory("resource/../../../", responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE)) == 0,
			"TEST: CWD Invalid Path Response");

	/*
	 * Test CWD with invalid path.
	 */
	responseCount = 0;
	handleCommandChangeDirectory("/home/${USER}", responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE)) == 0,
			"TEST: CWD Invalid Path Response");
	/*
	 * Test CWD with invalid path.
	 */
	responseCount = 0;
	handleCommandChangeDirectory("resource/*", responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE)) == 0,
			"TEST: CWD Invalid Path Response");


	/*
	 * Test PWD after unsuccessful CWD
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after unsuccessful CWD  Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("INFO: PWD after unsuccessful CWD Response skipped");
	}


	/*
	 * Test CWD with a valid path.
	 */
	responseCount = 0;
	handleCommandChangeDirectory("resource", responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),CHANGE_DIRECTORY_RESPONSE, strlen(CHANGE_DIRECTORY_RESPONSE)) == 0,
			"TEST: CWD Successful Response");

	/*
	 * Test PWD after successful CWD
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after successful CWD  Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("INFO: PWD after successful CWD Response skipped");
	}

	/*
	 * Test PWD before CDUP
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD before CDUP  Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("INFO: PWD before CDUP Response skipped");
	}



	/*
	 * Test CDUP, while it is permitted.
	 */
	responseCount = 0;
	handleCommandChangeDirectoryUp(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),CHANGE_TO_PARENT_DIRECTORY_RESPONSE, 
				strlen(CHANGE_TO_PARENT_DIRECTORY_RESPONSE)) == 0,
			"TEST: CDUP Successful Response");

	/*
	 * Test PWD after CDUP
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after CDUP  Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("INFO: PWD after CDUP Response skipped");
	}


	/*
	 * Test QUIT.
	 */
	responseCount = 0;
	handleCommandQuit(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), QUIT_RESPONSE, 23) == 0,
			"TEST: Quit Response");
	
}

BOOST_AUTO_TEST_SUITE_END()

/** 
 * ********************************* Test Code for Deliverable 3 ************************************* 
 */

BOOST_AUTO_TEST_SUITE(ftp_server_deliverable3)

BOOST_AUTO_TEST_CASE(ftp_server_passive_response) {

	//Get a test listener socket to use in function createPassiveSuccessResponse()
	int listenerSocket = -1;
        struct sockaddr_in socketAddress;
	memset(&socketAddress, 0, sizeof(struct sockaddr_in));	
	bool isListener = false;
	getTestListenerSocket(const_cast<char*>("4080"), listenerSocket, &socketAddress, isListener);

	//Create test success response from the listener socket
	char testResponse[FTP_RESPONSE_MAX_LENGTH];
	testPassiveSuccessResponse(testResponse, listenerSocket);

	//Create a the success response calling createPassiveSuccessResponse() function
	char response[FTP_RESPONSE_MAX_LENGTH];
	createPassiveSuccessResponse(response, listenerSocket);

	//Compare the test response with the created response
	BOOST_CHECK_MESSAGE(strcmp(testResponse, response) == 0, "TEST: Create Passive Success Response");
	
}	

BOOST_AUTO_TEST_CASE(ftp_server_passive_start) {
	
	/*
	 * Testing startPassiveListener()
	 */
	ClientFtpSession clientFtpSession;

	bool isSuccess = false;
	startPassiveListener(clientFtpSession, isSuccess);
	BOOST_CHECK_MESSAGE(isSuccess == true, "TEST: Start Passive Listener - Success");
	BOOST_CHECK_MESSAGE(clientFtpSession.dataListener != -1, "TEST: Start Passive Listner - Socket Descriptor");


	const char* message = "Message to test passive listener send and receive";
	char received[strlen(message)+1];

	pid_t childProcessID = fork();

	if(childProcessID == 0) {
		//child process
		
		int clientSocket = -1;
		
		int listenerPort = -1;
		getTestSocketPort(clientFtpSession.dataListener, listenerPort);
		
		closeTestSocket(clientFtpSession.dataListener); //child does not need the listener socket any more
		
		connectToTestRemote(getTestHostIP(), listenerPort, clientSocket);

		BOOST_CHECK_MESSAGE(clientSocket > 0, "TEST: Start Passive Listener - Client Connected");
		sleep(1);

		sendMessageToTestRemote(clientSocket, message, strlen(message));
		sleep(1);

		struct timeval timeout;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		bool isError = false;
		bool isTimeout = false;
		bool isClientReady = isTestSocketReadyToRead(clientSocket, &timeout, isError, isTimeout);
		BOOST_CHECK_MESSAGE(isClientReady, "TEST: Start Passive Listener -  Client Connection Ready");

		int len = sizeof(received);
		memset(received, 0, len);

		receiveMessageFromTestRemote(clientSocket, received, len);

		BOOST_CHECK_MESSAGE(strcmp(message, received) == 0, "TEST: Start Passive Listener - Client Send and Receive");
		
		sleep(2);
		closeTestSocket(clientSocket); 
		exit(0);
	} 
	else { 
		//parent process;
		sleep(2);
		struct timeval timeout;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		bool isError = false;
		bool isTimeout = false;
		bool isListenerReady = isTestSocketReadyToRead(clientFtpSession.dataListener, &timeout, isError, isTimeout);
		BOOST_REQUIRE(isListenerReady == true);


		int clientSockDescriptor = -1;
		acceptClientConnection(clientFtpSession.dataListener, clientSockDescriptor);
		BOOST_CHECK_MESSAGE(clientSockDescriptor > 0, "TEST: Start Passive Listener - Accept Connection");
		sleep(1);

		closeTestSocket(clientFtpSession.dataListener);

		isError = false;
		isTimeout = false;
		bool isClientSocketReady = isTestSocketReadyToRead(clientSockDescriptor, &timeout, isError, isTimeout);
		BOOST_CHECK_MESSAGE(isClientSocketReady, "TEST: Start Passive Listener - Accepted Connection Ready");

		memset(received, 0, sizeof(received));

		receiveFromRemote(clientSockDescriptor, received, sizeof(received));

		sleep(1);

		sendToRemote(clientSockDescriptor, message, strlen(message));

		sleep(3);

		closeTestSocket(clientSockDescriptor);	
		
	}
	
}

BOOST_AUTO_TEST_CASE(ftp_server_passive_enter) {

	/*
	 * Get a test listener that a client child process will use to get a control connection
	 * with server process.
	 */
	int testListenerSocket = -1;
	bool isTestListener = false;
	struct sockaddr_in testListenerSocketAddress;
	getTestListenerSocket(const_cast<char*>("4090"), testListenerSocket, &testListenerSocketAddress, isTestListener); 

	BOOST_REQUIRE(isTestListener == true  && testListenerSocket > 0);
	BOOST_REQUIRE(testListenerSocketAddress.sin_family == AF_INET);
	BOOST_REQUIRE(ntohs(testListenerSocketAddress.sin_port) == 4090);

	char* data = strdup("Data Test Message");
	int dataLength = strlen(data)+1 ;

	pid_t childProcessID2 = fork();

	if(childProcessID2 == 0) {
		//Child Process
		closeTestSocket(testListenerSocket); //child process does not need listener socket, close it

		//Get the control connection with the server
		int clientControlSocket = -1;
		connectToTestRemoteAddress(&testListenerSocketAddress, clientControlSocket);
		sleep(1);
		BOOST_REQUIRE(clientControlSocket>0);
		sleep(1);

		//Receiving Passive Success Response from the server on the control connection
		int responseLength = FTP_RESPONSE_MAX_LENGTH;
		char passiveResponse[responseLength];
		memset(passiveResponse,0, responseLength);
		receiveMessageFromTestRemote(clientControlSocket, passiveResponse, responseLength);

		//Retrieving Server IP and Port from Passive Success response
		char passiveHost[IP_ADDRESS_LENGTH];
		memset(passiveHost, 0, IP_ADDRESS_LENGTH);
		int passivePort = -1;
		getHostIPAndPortFromTestPassiveSuccessResponse(passiveResponse, passiveHost, passivePort);

		//Making a data connection to the server through passive listener
		struct sockaddr_in passiveSockAddress;
		passiveSockAddress.sin_family = AF_INET;
		passiveSockAddress.sin_port = htons(passivePort);

		struct in_addr sin_addr;
		sin_addr.s_addr = inet_addr(passiveHost);
		passiveSockAddress.sin_addr = sin_addr;
		
		int dataSocket = -1;
		
		connectToTestRemoteAddress(&passiveSockAddress, dataSocket);
		
		sleep(1);

		BOOST_CHECK_MESSAGE(dataSocket > 0, "TEST: Entering Into Passive - Client Data Connection");
		
		//Recieving data connection success response from the server on the control connection
		responseLength = FTP_RESPONSE_MAX_LENGTH;
		char dataResponse[responseLength];
		memset(dataResponse, 0, responseLength);
		receiveMessageFromTestRemote(clientControlSocket, dataResponse, responseLength);
		BOOST_CHECK_MESSAGE(strcmp(dataResponse, DATA_CONNECTION_SUCCESS_RESPONSE) == 0, 
				"TEST: Entering Into Passive - Client Data Connection Success Respons");

		sleep(1);

		int receiveLength = strlen(data) + 1;
		char receivedData[receiveLength];
		memset(receivedData, 0, receiveLength);
		receiveMessageFromTestRemote(dataSocket, receivedData, receiveLength);
		
		//Receiving data from the server on the data connection
		BOOST_CHECK_MESSAGE(strcmp(data, receivedData) == 0, "TEST: Entering Into Passive - Client Receive Data");

		sleep(1);

		free(data);

		closeTestSocket(dataSocket);	
		closeTestSocket(clientControlSocket);

		exit(0);
	}
	else {
		//parent process;
		sleep(2);
		struct timeval timeout;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		bool isError = false;
		bool isTimeout = false;
		bool isTestListenerReady = isTestSocketReadyToRead(testListenerSocket, &timeout, isError, isTimeout);

		BOOST_REQUIRE(isTestListenerReady == true);

		int serverControlSocket = -1;
		acceptClientConnection(testListenerSocket, serverControlSocket);
		BOOST_REQUIRE(serverControlSocket > 0);
		sleep(1);
	

		ClientFtpSession clientFtpSession2;
		clientFtpSession2.controlConnection = serverControlSocket;
		clientFtpSession2.isControlConnected = true;

		enteringIntoPassive(clientFtpSession2);
		sleep(1);

		BOOST_CHECK_MESSAGE(clientFtpSession2.dataConnection > 0, "TEST: Entering Into Passive - Server Data Connection");
		
		int sendCount = sendMessageToTestRemote(clientFtpSession2.dataConnection, data, dataLength);

		BOOST_CHECK_MESSAGE(sendCount > 0, "TEST: Entering Into Passive _ Server Sent Data on Data Connection");

		sleep(1);
		
		free(data);
   		stopClientFTPSession(clientFtpSession2) ;
		

	}	
}


BOOST_FIXTURE_TEST_CASE(ftp_server_request, FTPServerTestFixture) {
	const int RESPONSE_SIZE = 50;
	char response[RESPONSE_SIZE];

	/*
	 * Test connectToServerControl() function.
	 */
	connectToServerControl(getTestHostIP(), TEST_SERVER_PORT);
	sleep(2);
	BOOST_REQUIRE(isControlConnectionAvailable() == true);
	
	/*
	 * Test receiveOnControl() function.
	 */
	memset(response, 0, RESPONSE_SIZE);
        int responseLength = -1;
       	responseLength = receiveOnControl(response, RESPONSE_SIZE);
	BOOST_REQUIRE(responseLength == 19);
	BOOST_CHECK_MESSAGE(strncmp(response, CONNECTED_RESPONSE, responseLength-1)==0, "TEST: Connected Response");

	/*
	 * Test USER with valid username.
	 */
	string responses[MAX_NUMBER_OF_RESPONSES];

	int responseCount = 0;
	handleSimpleCommand("USER csci460", false, responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
      	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), USERNAME_OK_RESPONSE, 33)==0,
		"TEST: Valid User Response");
	/*
	 * Test PASS with vaild password.
	 */
	string password= "460pass";
	responseCount = 0;
	handleCommandPassword(password, responses, responseCount);

	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), LOGIN_RESPONSE, 27)==0, 
			"TEST: Valid Password Response");

	/*
	 * Test PWD.
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	char currentDir[1024];
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: Client PWD Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));
	}	
	else {
		BOOST_TEST_MESSAGE("INFO: Client PWD Response skipped");
	}
	
	/*
	 * Test NLST.
	 */
	responseCount = 0;
	handleCommandDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 4);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25)==0,
			"TEST: DIR - Passive Success Response");
	BOOST_CHECK_MESSAGE(strncmp(responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
				strlen(DATA_CONNECTION_SUCCESS_RESPONSE))==0,
			"TEST: DIR - Data Connection Success Response");
	//BOOST_TEST_MESSAGE("TEST: DIR - Directory List\n");	
	BOOST_CHECK_MESSAGE(responses[3].empty() == false, responses[3].insert(0, "TEST: DIR - Directory List: \n"));

	BOOST_CHECK_MESSAGE(strncmp(responses[2].c_str(), NLST_CONNECTION_CLOSE_RESPONSE,21)==0,
			"TEST: DIR - Data Connection Close Response");
	


	/*
	 * Test CWD with a valid path.
	 */
	responseCount = 0;
	handleCommandChangeDirectory("resource", responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),CHANGE_DIRECTORY_RESPONSE, strlen(CHANGE_DIRECTORY_RESPONSE)) == 0,
			"TEST: CWD Success Response");

	/*
	 * Test PWD after successful CWD
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after successful CWD  Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("INFO: PWD after successful CWD Response skipped");
	}

	/*
	 * Test RETR with invalid filename.
	 */
	responseCount = 0;
	handleCommandGetFile("nofile", responses, responseCount);
	BOOST_REQUIRE(responseCount == 3);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25)==0,
			"TEST: GET File - Passive Success Response");
	BOOST_CHECK_MESSAGE(strncmp(responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
				strlen(DATA_CONNECTION_SUCCESS_RESPONSE))==0,
			"TEST: GET File - Data Connection Success Response");
	BOOST_CHECK_MESSAGE(strncmp(responses[2].c_str(), RETR_UNAVAILABLE_ERROR_RESPONSE, 
				strlen(RETR_UNAVAILABLE_ERROR_RESPONSE))==0,
			"TEST: GET File - Unavailable Response");

	/*
	 * Test RETR with invalid file path.
	 */
	responseCount = 0;
	handleCommandGetFile("../README", responses, responseCount);
	BOOST_REQUIRE(responseCount == 3);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25)==0,
			"TEST: GET File - Passive Success Response");
	BOOST_CHECK_MESSAGE(strncmp(responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
				strlen(DATA_CONNECTION_SUCCESS_RESPONSE))==0,
			"TEST: GET File - Data Connection Success Response");

	BOOST_CHECK_MESSAGE(strncmp(responses[2].c_str(),INVALID_PATH_RESPONSE, strlen(INVALID_PATH_RESPONSE)) == 0,
			"TEST: CWD Invalid Path Response");



	/*
	 * Test RETR function with valid filename.
	 */
	responseCount = 0;
	handleCommandGetFile("duck.jpeg", responses, responseCount);
	BOOST_REQUIRE(responseCount == 3);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), PASSIVE_SUCCESS_RESPONSE, 25)==0,
			"TEST: GET File - Passive Success Response");
	BOOST_CHECK_MESSAGE(strncmp(responses[1].c_str(), DATA_CONNECTION_SUCCESS_RESPONSE, 
				strlen(DATA_CONNECTION_SUCCESS_RESPONSE))==0,
			"TEST: GET File - Data Connection Success Response");
	BOOST_CHECK_MESSAGE(strncmp(responses[2].c_str(), RETR_CONNECTION_CLOSE_RESPONSE,40)==0,
			"TEST: GET File - Data Connection Close Response");

	BOOST_CHECK_MESSAGE(access("duck.jpeg", F_OK) != -1, "TEST: GET File - File");

	struct stat statbuf;
	stat("duck.jpeg", &statbuf);
	BOOST_CHECK_MESSAGE(statbuf.st_size == 5594, "TEST: GET File - File Size");

       	remove("duck.jpeg");	


	/*
	 * Test PWD before CDUP
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD before CDUP  Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("INFO: PWD before CDUP Response skipped");
	}



	/*
	 * Test CDUP, while it is permitted.
	 */
	responseCount = 0;
	handleCommandChangeDirectoryUp(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(),CHANGE_TO_PARENT_DIRECTORY_RESPONSE, 
				strlen(CHANGE_TO_PARENT_DIRECTORY_RESPONSE)) == 0,
			"TEST: CDUP Success Response");

	/*
	 * Test PWD after CDUP
	 */
	responseCount = 0;
	handleCommandPrintDirectory(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	memset(currentDir, 0, 1024);
	if(getcwd(currentDir, 1024) != NULL) {
		BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), currentDir, strlen(currentDir))==0,
				"TEST: PWD after CDUP Response");
		BOOST_TEST_MESSAGE(responses[0].insert(0, "INFO: FTP Server Current Directory: "));

	}	
	else {
		BOOST_TEST_MESSAGE("INFO: PWD after CDUP Response skipped");
	}


	/*
	 * Test QUIT.
	 */
	responseCount = 0;
	handleCommandQuit(responses, responseCount);
	BOOST_REQUIRE(responseCount == 1);
	BOOST_CHECK_MESSAGE(strncmp(responses[0].c_str(), QUIT_RESPONSE, 23) == 0,
			"TEST: Quit Response");
	
}


BOOST_AUTO_TEST_SUITE_END()

