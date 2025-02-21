#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include <stdio.h>

#include "SharedObject.h"
#include "thread.h"

using namespace std;

struct MyShared
{
	int id; // thread id
	int count; // count of reports from a given thread
	int timePassed; // time since last report of a given thread
	void printInfo(); // function to print all this info
};

// function of MyShared struct to print thread info
void MyShared::printInfo()
{
	cout << "Reader Thread: " << id << " " << count << " " << timePassed << endl; // print info
}

// class for writer's threads
class WriterThread : public Thread 
{
	public: // class fields
		int num;
		int delay;
		int reportCount;
		bool run;
		
		Sync::Event destroyThreads; // sync event from thread.h
		
		WriterThread(int n, int d) : Thread (8*1000) // constructor
		{
			this->num = n; // set thread number
			this->delay = d; // set thread delay
			this->reportCount = 0; // set number of reports sent
			this->run = true; // thread should run
		}
		
		virtual long ThreadMain (void) override 
		{
			Shared <MyShared> sharedMemory ("sharedMemory", false); // declare shared memory variable so the threads can access it
			time_t lastReportTime; // time of last report
			time_t nextReportTime; // time of current report
			
			lastReportTime = time(&lastReportTime); // set intial time for previous report as it is about to occur
		
			
			while (run) // run the thread continuously
			{
				reportCount++; // increase count of reports
				nextReportTime = time(&nextReportTime); // set time since previous report right before next report
				
				// add thread values to shared memory object
				sharedMemory->id = num;
				sharedMemory->count = reportCount;
				sharedMemory->timePassed = nextReportTime - lastReportTime;
				
				lastReportTime = time(&lastReportTime); // set time of most recent report
				
				sleep(delay); // delay program for input number of seconds
				 
			}
		}
		
		// desctructor to pair with sync event
		void EndThread()
		{
			destroyThreads.Trigger(); // call synced function
		}		
};

// main class
int main(void)
{
	cout << "I am a Writer" << endl; // initial message
	
	string userData = ""; // declare input string
	int writeDelay = 0; // declare input int
	int tcount = 0; // track the number of spawned threads
	
	vector <WriterThread *> threads; // vector array of all threads
	
	Shared <MyShared> shared("sharedMemory", true); // declare this process the owner of the shared memory
	
	while (true)
	{
		cout << "Would you like to create a writer thread?" << endl;
		cin >> userData; // collect user response
		cin.ignore(); // ignore enter
		
		if ((userData == "yes") || (userData== "y") || (userData == "Yes") || (userData == "Y"))
		{
			cout << "What is the delay time for this thread?\n";
			cin >> writeDelay; // collect write delay from user
			cin.ignore(); // ignore enter
			tcount++; // increment count
			
			// spawn new thread, begin writing to shared memory
			threads.push_back(new WriterThread(tcount, writeDelay)); // spawn new thread and add it to the array
			
		}
		else if ((userData == "no") || (userData == "n") || (userData == "No") || (userData == "N"))
		{
			break; // exit loop, terminate threads
		}
		else 
		{
			cout << "Invalid input -> try 'Yes' or 'No'" << endl; // inavlid input
		}	
	}
	
	for (vector <WriterThread *>::iterator i = threads.begin(); i != threads.end(); i++) // iterate through array of threads
	{
		(*i)->EndThread(); // end thread
	}
}
