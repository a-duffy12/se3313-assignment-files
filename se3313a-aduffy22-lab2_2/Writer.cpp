#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include <stdio.h>

#include "SharedObject.h"
#include "thread.h"
#include "Semaphore.h"

using namespace std;

struct MyShared
{
	int id; // thread id
	int count; // count of reports from a given thread
	int timePassed; // time since last report of a given thread
	bool read; // confirms that reader has read before writing again
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
			
			Semaphore memory("memory"); // semphore for access to shared memory
			
			lastReportTime = time(&lastReportTime); // set intial time for previous report as it is about to occur
		
			
			while (run) // run the thread continuously
			{
				sleep(delay); // delay program for input number of seconds
				
				memory.Wait(); // lock writer into shared memory
				
				while (!sharedMemory->read) // check if the memory has been read yet
				{
					memory.Signal(); // if not, then let another thread go first
					memory.Wait(); // then request access to the shared memory
				}
				
				if (sharedMemory->read) // if the shared memory has been read and the thread can now write
				{
					reportCount++; // increase count of reports
					nextReportTime = time(&nextReportTime); // set time since previous report right before next report
				
					// add thread values to shared memory object
					sharedMemory->id = num;
					sharedMemory->count = reportCount;
					sharedMemory->timePassed = nextReportTime - lastReportTime;
					sharedMemory->read = false; // set memory to unread as it has now changed
				
					lastReportTime = time(&lastReportTime); // set time of most recent report		
				}
				
				memory.Signal(); // unlock shared memoery for another process 
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
	
	Semaphore memory("memory", 1, true); // declare semaphore for shared memory
	
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
