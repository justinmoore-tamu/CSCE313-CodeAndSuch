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
// #include <stdlib.h>
#include <string>
#include <stdlib.h>

using namespace std;


int main (int argc, char *argv[]) {
	int opt;
	int m = MAX_MESSAGE;
	int p = 1;
	// changed t and e defaults
	// double t = 0.0;
	double t = -1;
	// int e = 1; 
	int e = 0;
	string filename = "";

	//Add other arguments here
	while ((opt = getopt(argc, argv, "p:t:e:f:m:")) != -1) {
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
			case 'm':
				m = atoi (optarg);
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
		// TODO get ability to pass m as an optarg
		string workaround = to_string(m);
		char* nullList[] = {(char*) "./server", (char*) "-m", (char*) workaround.c_str(), nullptr};
		execvp(nullList[0], nullList); 
	}

	// in parent, connect to control fifo on the client side
    FIFORequestChannel chan("control", FIFORequestChannel::CLIENT_SIDE);
	// End Task 1 /////////////////////

	// TODO: later 
	//Task 4: ////////////////////////////
	//Request a new channel
	// End Task 4 /////////////////////
	
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
	} else if (filename=="") {
		// get the first 1000 data points for a given patient and put them in a file called x1.csv
		// 		times are in increments of 0.004 and start at 0.0
		
		// open/create file x1.csv
		ofstream myFile;
		myFile.open("./received/x1.csv");
		// myFile.write();
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
		// close file
		myFile.close();
	}
	// End Task 2 /////////////////////
	
	//Task 3: ////////////////////////
	//Request files      #filemsg(offset, receiveSize)
	if (filename != "") {
		// get size of requested file
		// construct file message to get file size
		filemsg fm(0, 0);
		string fname = filename;
		// create propperly sized buffer based on message size and filename size
		int len = sizeof(filemsg) + (fname.size() + 1);
		char* buf2 = new char[len];
		// fill buffer with data and write to fifo
		memcpy(buf2, &fm, sizeof(filemsg));
		strcpy(buf2 + sizeof(filemsg), fname.c_str());
		chan.cwrite(buf2, len);
		// read from fifo to get length of file
		delete[] buf2;
		__int64_t file_length;
		chan.cread(&file_length, sizeof(__int64_t));
		// cout << "The length of " << fname << " is " << file_length << endl;

		// get contents of file in chunks of m and put into received directory under the same name
		// open file
		ofstream myFile;
		myFile.open("./received/"+filename);
		// loop to get chunks of file
		__int64_t offset = 0;
		__int64_t remaining = file_length;
		char* buf3 = new char[len]; // for filemsg+name to be passed to server
		char* result = new char[m];
		while (remaining > m) {
			// make filemsg
			filemsg fm2(offset, m);
			// fill buffer
			memcpy(buf3, &fm2, sizeof(filemsg));
			strcpy(buf3 + sizeof(filemsg), fname.c_str());
			// write to fifo
			chan.cwrite(buf3, len);
			// read from fifo
			//    for some reason passing result as a reference (&result) causes a seg fault
			chan.cread(result, (size_t)m); // what am i reading from server?
			// write to file
			//    myFile << result will not work here
			myFile.write(result,m);
			// increment offset and remaining
			offset += (__int64_t) m;
			remaining -= (__int64_t) m;
		}
		delete [] result;
		// final filemsg send and recieve
		char* finResult = new char[remaining];
		// fm.(offset, remaining);
		// construct file message
		fm.offset = offset;
		fm.length = remaining;
		// fill buffer
		// cout <<fm.offset<<" "<<fm.length<<" "<<remaining<<std::endl;
		memcpy(buf3, &fm, sizeof(filemsg));
		strcpy(buf3 + sizeof(filemsg), fname.c_str());
		// write and read from buffer
		chan.cwrite(buf3, len);
		chan.cread(finResult, (size_t)remaining);
		// write to file
		// myFile << finResult;
		myFile.write(finResult, remaining);
		// delete created arrays
		delete [] finResult;
		delete [] buf3;
		// close file
		myFile.close();

	}
	// End Task 3 /////////////////////

	//Task 5:
	// Closing all the channels
    MESSAGE_TYPE mes = QUIT_MSG;
    chan.cwrite(&mes, sizeof(MESSAGE_TYPE));
	
}
