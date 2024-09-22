/*
	Author of the starter code
    Yifan Ren
    Department of Computer Science & Engineering
    Texas A&M University
    Date: 9/15/2024
	
	Please include your Name, UIN, and the date below
	Name: Justin Moore
	UIN: 832005024
	Date: 09/17/24
*/
#include "common.h"
#include "FIFORequestChannel.h"

using namespace std;


int main (int argc, char *argv[]) {
	int opt;
	int p = 1;
	// changed t and e defaults
	// double t = 0.0;
	double t = -1;
	// int e = 1; 
	int e = 0;
	string filename = "";

	//Add other arguments here
	while ((opt = getopt(argc, argv, "p:t:e:f:")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi (optarg);
				break;
			case 't':
				t = atof (optarg);
				break;
			case 'e':
				e = atoi (optarg);
				break;
			case 'f':
				filename = optarg;
				break;
		}
	}
	// cout <<"p:"<<p<<"t:"<<t<<"e:"<<e<<endl; // testing while loop for getting cmdline args     Acts as expected

	//Task 1: //////////////////////
	//Run the server process as a child of the client process

	// fork
	pid_t pid;
	pid = fork(); // 0 is child, >0 is parent, <0 is error
	assert(pid >= 0);

	// in child, run server using exec
	if (pid == 0) {
		char* nullList[] = {nullptr};
		execvp("./server", nullList); 
	}

	// in parent, connect to control fifo on the client side
    FIFORequestChannel chan("control", FIFORequestChannel::CLIENT_SIDE);
	// End Task 1 /////////////////////

	// TODO: later 
	//Task 4: ////////////////////////////
	//Request a new channel
	
	//Task 2: //////////////////////////
	//Request data points

	if (t!=-1 || e!=0) { // TODO check this logic
		// construct message (datamsg) with given arguments
		// datamsg x(1, 0.0, 1);
		datamsg x(p,t,e);
		
		char buf[MAX_MESSAGE];
		// copy message to buffer and write to channel fifo
		memcpy(buf, &x, sizeof(datamsg));
		chan.cwrite(buf, sizeof(datamsg));
		// read reply from server 
		double reply;
		chan.cread(&reply, sizeof(double));
		cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;
	} else {
		// get the first 1000 data points for a given patient and put them in a file called x1.csv
		// 		times are in increments of 0.004 and start at 0.0
		
		// open/create file x1.csv
		ofstream myFile;
		myFile.open("./received/x1.csv");

		t = 0.0;
		double reply1,reply2;
		char buf[MAX_MESSAGE];
		for (int i = 0; i < 1000; i++) {
			// construct messages to get ecg1 and ecg2 data from person
			double currTime = t+(0.004*i);
			datamsg x(p, currTime, 1); 
			datamsg y(p, currTime, 2); 
			// write and get response for message 1
			memcpy(buf, &x, sizeof(datamsg));
			chan.cwrite(buf, sizeof(datamsg));
			chan.cread(&reply1, sizeof(double));
			// write and get response for message 2
			memcpy(buf, &y, sizeof(datamsg));
			chan.cwrite(buf, sizeof(datamsg));
			chan.cread(&reply2, sizeof(double));
			// write to x1          format:  t,ecg1,ecg2
			myFile << currTime<<","<<reply1<<","<<reply2<<"\n";
		}
		// close x1.csv
		myFile.close();
	}
	// End Task 2 /////////////////////
	
	//Task 3: ////////////////////////
	//Request files
	filemsg fm(0, 0);
	string fname = "1.csv";
	
	int len = sizeof(filemsg) + (fname.size() + 1);
	char* buf2 = new char[len];
	memcpy(buf2, &fm, sizeof(filemsg));
	strcpy(buf2 + sizeof(filemsg), fname.c_str());
	chan.cwrite(buf2, len);

	delete[] buf2;
	__int64_t file_length;
	chan.cread(&file_length, sizeof(__int64_t));
	cout << "The length of " << fname << " is " << file_length << endl;
	// End Task 3 /////////////////////

	//Task 5:
	// Closing all the channels
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite(&m, sizeof(MESSAGE_TYPE));
	
}
