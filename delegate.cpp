// delegate.cpp
// Corey S. Gray
// 02 February 2018
// Source code for Assignment 2: Delegated Computation program

#include <iostream>
using std::cin;
using std::cout;
using std::endl;
#include <limits>
#include <mutex>
using std::lock_guard;
using std::mutex;
#include <utility>
using std::pair;
using std::make_pair;
#include <queue>
using std::queue;
#include <thread>
using std::thread;
#include <vector>
using std::vector;

#include "sa2a.h"	// For "int sa2a(int)"

// Constants
const int NUMBER_THREADS = 6;
int totalInputs = 0;
queue<int> inputBuffer;
queue<pair<int, int>> outputBuffer;
mutex inputMutex;
mutex outputMutex;

// run
// Run function for spawned threads.
// Calls sa2a on the provider value.
void run() {
	int value = 0;
	while (!(inputBuffer.empty())) {
		inputMutex.lock();
		if (!(inputBuffer.empty())) {
			value = inputBuffer.front();
			inputBuffer.pop();
		}
		inputMutex.unlock();
		
		if (value != 0) {
			auto output = sa2a(value);
			lock_guard<mutex> outputGuard(outputMutex);
			outputBuffer.push(make_pair(value, output));
		}

		value = 0;
	}
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
	
		inputBuffer.push(input);
		totalInputs++;
	}

	// Delegate computations to worker threads
	for (auto &worker : workerPool)
		worker = thread(run);
	
	// Output results
	for (int i = 0; i < totalInputs; ++i) {
		while (outputBuffer.empty());

		auto resultPair = outputBuffer.front();
		outputBuffer.pop();
		cout << "sa2a(" << resultPair.first << ") = " << resultPair.second << endl;
	}

	
	// Wait for all threads to terminate
	for (auto &worker : workerPool)
		worker.join();
}
