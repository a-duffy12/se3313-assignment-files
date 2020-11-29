#include "thread.h"
#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace Sync;

// thread for clients that connect to the server's socket
class ClientThread : public Thread
{
private:
	Socket& socket; // server's socket that client connects to
	ByteArray data; // data the client sends to the server
public:
	bool running = true; // whether or not the client is running
	
	ClientThread(Socket& socket) // constructor
	: socket(socket)
	{}
	
	virtual long ThreadMain()
	{
		socket.Open(); // open the socket connection
		
		while (true)
		{
			std::string input =  ""; // string for user input
		
			std::cout << "Please input your data, or type 'done' to quit: ";
			std::cin >> input; // collect user data
			std::cin.ignore(); // ignore enter
			std::cout.flush();

			data = ByteArray(input); // send this data to a byte array
			
			if (input == "done")
			{
				std::cout << "Terminating client..." << std::endl; // termination message
				running = false; // client is no longer active
				break; // exit client
			}
			
			try // try to read and write to and from the socket
			{
				socket.Write(data); // send data to server
			
				socket.Read(data); // receive response from server
			
				std::cout << data.ToString() << std::endl; // output data from server
			}
			catch (...) // if the operation fails
			{
				std::cout << "Error accessing socket from client!" << std::endl; 
			}
		}
		return 0; // exit function with all clear
	}
};

int main(void)
{
	// Welcome the user 
	std::cout << "SE3313 Lab 3 Client" << std::endl;
	
	// Create our socket
	Socket socket("127.0.0.1", 3000);
	
	ClientThread clientThread(socket); // create a client thread connected to the socket
	
	while (clientThread.running == true) // so long as the client is running
	{
		sleep(1); // sleep in between sending messages so as not to flood the server
	}
	
	socket.Close(); // close the socket

	return 0; // exit function with all clear
}
