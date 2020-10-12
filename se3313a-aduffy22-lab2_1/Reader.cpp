#include <iostream>
#include <time.h>

#include "SharedObject.h"

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

int main(void)
{
	cout << "I am a reader" << endl;
	
	Shared<MyShared> sharedMemory ("sharedMemory"); // declare shared memory obejct to read from
	
	while (true)
	{
		sharedMemory->printInfo(); // print info in shared memory
		sleep(1); // wait 1s before printing again
	} 
	
}
