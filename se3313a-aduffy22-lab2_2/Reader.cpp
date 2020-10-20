#include <iostream>
#include <time.h>

#include "SharedObject.h"
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

int main(void)
{
	cout << "I am a reader" << endl;
	
	Shared<MyShared> sharedMemory ("sharedMemory"); // declare shared memory obejct to read from
	Semaphore memory("memory", 1, true); // declare semaphore for shared memory
	
	// temporary variables to look for changes in the shared memory
	int tempId;
	int tempCount;
	int tempTime;
	bool check= false;
	
	while (true)
	{
		// get variables from shared memory once initially before first print
		if (!check)
		{
			memory.Wait(); // lock reader into shared memory
			tempId = sharedMemory->id;
			tempCount = sharedMemory->count;
			tempTime = sharedMemory->timePassed;
			sharedMemory->read = true; // mark shared memory as read
			memory.Signal(); // unlock shared memory for another process
			check = true; // initial values have been grabbed
		}
		
		// only print shared memory contents if they have been changed since the last time they were printed
		memory.Wait(); // lock reader into shared memory
		if (sharedMemory->read)
		{
			
		}
		else if (!sharedMemory->read &&((tempId != sharedMemory->id) || (tempCount != sharedMemory->count) || (tempTime != sharedMemory->timePassed))) // check for updated values in shared memory
		{
			sharedMemory->printInfo(); // print updated info in shared memory
			sharedMemory->read = true; // mark shared memory as read
			
			// update temporary variables 
			tempId = sharedMemory->id;
			tempCount = sharedMemory->count;
			tempTime = sharedMemory->timePassed;
		}
		memory.Signal(); // unlock shared memory for another process
	} 
	
}
