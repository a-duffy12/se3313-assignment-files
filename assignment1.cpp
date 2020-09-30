// SE3313 Assignment 1
// Questions 9-13

#include <iostream>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <signal.h>
#include "sys/wait.h"

using namespace std;

int main() {
	
	string userData = ""; // decalre input string
	vector<int> childPids; // declare array of all child processes' id
	
	cout << "SE3316 Assignment 1" << endl; // title
	
	do 
	{
		cout << "Enter some text please: "; // inform user
		cin >> userData; // collect user data
		cin.ignore(); // ignore enter
		
		if (userData == "Done") // if input is Done, system should end
		{
			break;	
		}
		
		pid_t child = fork(); // fork to create new child process
		childPids.push_back(child); // add new pid to the end of the array of PIDs
		
		if (child > 0) // fork result > 0 indicates a successful fork
		{
			ofstream newFile; // create new file
			newFile.open(to_string(child) + ".txt"); // create new file using pid of process
			while (true) 
			{
				sleep(1); // sleep for 1 second
				
				if (newFile.is_open()) // check if created file(s) is/are open
				{
					newFile << userData << endl; // write to file
				}
				else // if created file(s) is/are not open
				{
					cout << "Error: Cannot write to file " + to_string(child) + ".txt" << endl; // output error message with name of file
				}
			}
			
			newFile.close(); // close file

		}
		else if (child == 0) // failed to create new process
		{
			continue; 
		}
	}
	while (userData != "Done");
	
	while (!(childPids.empty())) // kills off child processes one by one until none remain active
	{
		kill (childPids.back(), SIGTERM); // kill process of last PID in array 
		childPids.pop_back(); // remove last element in array
	}
		
	return 0; // parent process is concluded
}

// compile by going to directory in CMD and typing "g++ assignment1.cpp"
// run output file into same directory by typing "./a.out" for default file name
