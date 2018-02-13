// delegate.cpp
// Corey S. Gray
// 02 February 2018
// Source code for Assignment 2: Delegated Computation program

#include <atomic>
using std::atomic_int;
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
#include <limits>
#include <mutex>
using std::lock_guard;
using std::mutex;
#include <thread>
using std::thread;
#include <vector>
using std::vector;

#include "sa2a.h"	// For "int sa2a(int)"

// Constants
const int NUMBER_THREADS = 6;
vector<int> inputBuffer;
int sa2aInput;
int sa2aOutput;
mutex outputMutex;

// run
// Run function for spawned threads.
// Calls sa2a on the provider value.
void run(int value) {
	auto output = sa2a(value);
	lock_guard<mutex> outputGuard(outputMutex); 
	sa2aInput = value;
	sa2aOutput = output;
	cout << "sa2a(" << sa2aInput << ") = " << sa2aOutput << endl;
}

int main() {
	cout << "Delegated Computation" << endl;

	// Spawn worker threads
	vector<thread> workerPool (NUMBER_THREADS);

	// User Input
	int input;
	while (true) {
		cout << "Enter a positive integer (or 0 to end input): ";
		cin >> input;

		if (cin.fail()) {
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
			cout << "Only integers are valid input." << endl;
			continue;
		}

		else if (input < 1)
			break;
	
		inputBuffer.push_back(input);
	}

	// Delegate computations to workers
	auto currentItem = inputBuffer.begin();
	for (auto & currentWorker : workerPool) {
		currentWorker = thread(run, *currentItem);
		currentItem++;
		if (currentItem == inputBuffer.end())
			break;
	}

	// Retask threads if input remains
	for (auto & currentWorker : workerPool) {
		if (currentWorker.joinable())
			currentWorker.join();

		if (currentItem != inputBuffer.end()) {
			currentWorker = thread(run, *currentItem);
			currentItem++;
		}
	}

	// Join all threads before exiting
	for (auto & worker : workerPool) {
		if (worker.joinable())
			worker.join();
	}
}
