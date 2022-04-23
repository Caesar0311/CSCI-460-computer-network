/**
 * @file ftp_server.cpp
 * @author Humayun Kabir, Instructor, CSCI 460, VIU
 * @version 1.0.0
 * @date June 22, 2020
 *
 * FTP Server Application.
 * Accepts FTP client connections and starts independent FTP session for each client.
 * Starts a listener socket at a user specified port and waits for client's connection.
 * Creates child processes to handle individual client's session.
 * Each child process receives and handles its client's FTP requests independently.
 * Capable of handling following FTP requests only:
 *  	USER <username>
 *   	PASS <password>
 *    	PWD
 *     	CWD  <dir>
 *      CDUP
 *      PASV
 *      NLIST
 *      RETR <filename>
 *      QUIT
 * 
 * Main process keeps track of the child processes and stops them when it is being stopped. 
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <list>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "ftp_server_connection_listener.hpp"
#include "ftp_server_session.hpp"
#include "ftp_server_net_util.hpp"


using namespace std;


list<pid_t> clients(MAX_CLIENT_CONNECTIONS);  	// Holds the ids of the child process 
						// that deals with individual ftp
						// client session. 


/*
 * Forward declarations of the helper functions
 */
void usage(const char* prog);			// Prints usage text of FTP server application
char* baseName(const char* pathname);
//{
//    char *path;
//    path = get_current_dir_name();
//    return path;
//}// Returns the name of the code base of FTP server application
void handleSIGTERM(int signal);			// Terminate signal handler
void handleSIGINT(int signal);			// Interrupt signal handler
void stopAllProcesses();			// Function to stop FTP server's main and child processes

/**
 * The main function for FTP server application.
 * Accepts port number as a command line  parameter.
 * If the user missed to pass the port number, prints the usage text of the application.
 * Starts FTP server application on the port number passed by the user.
 *
 * @param argc Counts the number of command line arguments plus 1 (for code base).
 * @param argv Holds the code base name and the command line arguments.
 * 
 * @return 0, if successful. -1, otherwise.
 */
int main (int argc, char** argv) {
   
    //Check whether the port number has passed as the command line argument.
    if(argc<2) {
	//Port number is missing, show usage text to the user and return.    
        usage(baseName(argv[0]));
	return -1;
    }

    //Grab the port number from the command line argument.
    char* port = strdup(argv[1]);  
    
    //Declare a descriptor for the listener socket.
    int listenerSockDescriptor = -1;
    
    //Declare a flag to hold the status of the listener socket
    bool succeded = false;
    
    /*
     * Register signal handlers against Terminate and Interrupted signals to
     * stop the main and all child procesess of FTP server application
     */
    signal(SIGTERM, handleSIGTERM);
    signal(SIGINT, handleSIGINT);

    //Grab IP of the machine, where FTP server application is running
    char* myIPAddress = getHostIPAddress();
    cout<<"FTP Server at IP address: "<< myIPAddress << " is starting........."<<endl;
    
    //Start the listener socket to accept client connection
    startListenerSocket(port, listenerSockDescriptor, succeded);
    if(succeded && listenerSockDescriptor != -1) {
    	clients.clear();
        while(true) {
            int clientSockDescriptor = -1;
            bool isError = false;
            bool isTimeout = false;

            /*
             * Check whether the maximum number of client connections has not been reached yet and
             * the listener socket has some client connection request(s) pending
             */
            if(clients.size() < MAX_CLIENT_CONNECTIONS &&
               isListenerSocketReady(listenerSockDescriptor, 555, 0, isError, isTimeout) &&
               !isError && !isTimeout) {
                    //Accept the pending connection request
                   acceptClientConnection(listenerSockDescriptor, clientSockDescriptor);
                   /*
                    * Start a child process with the accepted connection request
                    * to deal with the client FTP session independently.
                    */
                    if(clientSockDescriptor != -1) {
                    pid_t pid = -1;
                    //starts a child process to handle individual client's ftp session
                    if((pid=fork())==0) {
                        //listener socket is not required in the child process, close it
                        closeSocket(listenerSockDescriptor);
                        //starts a client session using client's connection
                        ClientFtpSession clientFtpSession;
                        clientFtpSession.controlConnection = clientSockDescriptor;
                        clientFtpSession.rootDir = get_current_dir_name();

                        startClientFTPSession(clientFtpSession);
                        //close client connection when client session ends
                        closeSocket(clientSockDescriptor);
                        //remove the child process from the list of client processes
                        clients.remove(getpid());
                        //exits the child process that was dealing with a client session
                        exit(0);
                    } //child process code ends here.
                    //此时在父进程里面，将子进程的pid加进来
                    if(pid != -1) {
                        //add the child process in the list of client processes
                        clients.push_back(pid);
                    }
                    //创建新进程失败
                    //client socket is not required in the parent process, so close it
                    closeSocket(clientSockDescriptor);
                }
            }
	    }
    }

    /*
     * Library function strdup() returns a pointer of a dynamically allocated memory from heap.
     * For that reason, the memory has to be released from the pointer before return to void memory leak
     */
    free(port);

    return 0;

}

/**
 * Shows the usage text of FTP server application.
 * 
 * @param base Passes the name of the code base of FTP server application
 */
void usage(const char* base) {
    cout<<setw(6)<<" "<<"Usage:  "<<base<<" <server_port>"<<endl;
}

/**
 * Gets the name of the code base of FTP server application from its pathname.
 * 
 * @param pathname Passes the pathname of the code base of FTP server application.
 * @return the name of the code base of FTP server application.
 */ 
char* baseName(const char* pathname) {
    char* path_name = const_cast<char*>(pathname);
    char* base_name = path_name;
    if((base_name=strrchr(path_name, '/')) != NULL) {
        ++base_name;
    }
    return base_name;
}

/**
 * Handler for Terminate signal, stops the main and all child processes.
 *
 * @param signal Passes the signal number.
 */
void handleSIGTERM(int signal) {
	// Terminated, stop all processes.
	stopAllProcesses();
}

/**
 * Handler for Interrupt signal, stops the main and all child processes.
 * 
 * @param signal Passes the signal number.
 */
void handleSIGINT(int signal) {
	// Interrupted, stop all processes.
	stopAllProcesses();
}

/**
 * Helper function to stop the main and all child processes.
 */
void stopAllProcesses() {
	
	/*
	 * First deal with the child procesess.
	 * Sends signals to the child processes to stop.
	 */
	for (list<pid_t>::iterator client = clients.begin(); client != clients.end(); client++) {
		kill(*client, SIGTERM);
		sleep(1);
		kill(*client, SIGKILL);
		sleep(1);
	}
	sleep(1);
	/*
	 * Signals the server process itself to stop.
	 */
	raise(SIGKILL);
}
