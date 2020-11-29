#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>

using namespace Sync;

// thread to create the socket
class SocketThread : public Thread
{
private:
	Socket& socket; // the socket server offers
	ByteArray data; // incoming data from clients
public:
	SocketThread(Socket& socket) // constructor
	: socket(socket)
	{}
	
	// function to get the socket objects
	Socket& GetSocket()
	{
		return socket;
	}
	
	virtual long ThreadMain()
	{
		while (true)
		{
			try // try to read/write from socket
			{
				socket.Read(data); // read incoming data from socket
				
				std::string val = data.ToString(); // turn data into a string
				
				for (int i = 0; i < val.length(); i++) // for the length of the input data
				{
					val[i] = toupper(val[i]); // switch each character to uppercase
				}
				
				data = ByteArray(val); // make new word into byte array format
				
				socket.Write(data); // send reformatted data back to the user
			}
			catch (...) // if the operation fails
			{
				std::cout << "Error accessing socket from server!" << std::endl;
			}
		}
		return 0; // exit function with all clear
	}
};
					

// This thread handles the server operations
class ServerThread : public Thread
{
private:
	SocketServer& server; // server object
public:
	std::vector <SocketThread*> clientCons; // array of all the socket threads in use by clients

	ServerThread(SocketServer& server) // constructor
	: server(server)
	{}

	~ServerThread() // destructor is different from default as it must close socket instances
	{
		for (auto c: clientCons) // for each socket instance when server is shutdown
		{
			try // try to iteratively close each socket
			{
				Socket& soc = c->GetSocket();
				soc.Close(); 
			}
			catch (...) // if the operation fails
			{
				std::cout << "Error closing socket!" << std::endl;
			}
		}
	}

	virtual long ThreadMain()
	{	
		while (true)
		{	
			// Wait for a client socket connection
			Socket* newConnection = new Socket(server.Accept());

			// A reference to this pointer 
			Socket& socketReference = *newConnection; // get a reference to this new connection
			SocketThread* socThread = new SocketThread(socketReference); // create a socket thread for this connection
			
			clientCons.push_back(socThread); // add new socket thread to the array of client connections
		}
		return 0; // exit function with all clear
	}
};


int main(void)
{
	std::string input = "";

	std::cout << "I am a server." << std::endl;
	std::cout << "Press 'enter' to terminate the server: ";
	std::cout.flush();
	
	// Create our server
	SocketServer server(3000);    

	// Need a thread to perform server operations
	ServerThread serverThread(server);

	// This will wait for input to shutdown the server
	FlexWait cinWaiter(1, stdin); // any input will initiate next step
	cinWaiter.Wait(); // wait for input
	
	std::cout << "Terminating server..." << std::endl; // termination message
	std::cout.flush();
	
	// Shut down and clean up th server
	server.Shutdown(); // gracefully terminate the server
}
