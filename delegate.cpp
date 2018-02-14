// delegate.cpp
// Corey S. Gray
// 13 February 2018
// Source code for Assignment 2: Delegated Computation program

#include <iostream> // For std::cin, std::cout, std::endl, std::streamsize
#include <limits>   // For std::numeric_limits
#include <mutex>    // For std::lock_guard, std::mutex
#include <utility>  // For std::make_pair, std::pair
#include <queue>    // For std::queue
#include <thread>   // For std::thread
#include <vector>   // For std::vector

#include "sa2a.h"	// For "int sa2a(int)"
#include "delegate.h"

// Constants and global variables
const int NUMBER_THREADS = 6;
int input;
int totalInputs = 0;
std::queue<int> inputQueue;
std::queue<std::pair<int, int>> outputQueue;
std::mutex inputMutex;
std::mutex outputMutex;

// getValueFromInputQueue
// Retrieves the first value from inputQueue and pops inputQueue.
// Thread safe and avoids temporal coupling.
// Preconditions:
//		inputQueue is a std::queue<int> with at least one item in it.
//		inputMutex is a valid mutex object.
int getValueFromInputQueue() {
	inputMutex.lock();
		auto value = inputQueue.front();
		inputQueue.pop();
	inputMutex.unlock();
	return value;
}

// void fillOutputQueueWithSA2AfromInputQueue() {
// Calls sa2a with values from inputQueue and fills outputQueue with the result.
// Thread safe.
// Preconditions:
//		inputQueue is a std::queue<int>.
//		outputQueue is a std::queue<std::pair<int, int>>
void fillOutputQueueWithSA2AfromInputQueue() {
	while (!(inputQueue.empty())) {
		auto value = getValueFromInputQueue();
		auto output = sa2a(value);
		std::lock_guard<std::mutex> outputGuard(outputMutex);
		outputQueue.push(std::make_pair(value, output));
	}
}

// printResultsFromOutputQueue
// Waits for a worker thread to put an item in OutputQueue then prints it.
// Preconditions:
//		outputQueue is a std::queue<std::pair<int, int>>
void printResultsFromOutputQueue() {
	while (outputQueue.empty());
	auto resultPair = outputQueue.front();
	outputQueue.pop();
	std::cout << "sa2a(" << resultPair.first << ") = " << resultPair.second << std::endl;
}

// main
// Spawns six threads. Collects one or more integers from the user then passes
// delegates calling sa2a with them to the worker threads. Outputs the results
// then waits for the threads to terminate before exiting.
int main() {
	std::cout << "Delegated Computation" << std::endl;

	// Spawn worker threads
	std::vector<std::thread> workerPool (NUMBER_THREADS);

	// Input numbers from user
	while (true) {
		std::cout << "Enter a positive integer (or 0 to end input): ";
		std::cin >> input;

		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
			std::cout << "Only integers are valid input." << std::endl;
			continue;
		}

		else if (input < 1)
			break;
	
		inputQueue.push(input);
		totalInputs++;
	}

	// Delegate all computations to worker threads
	for (auto &worker : workerPool)
		worker = std::thread(fillOutputQueueWithSA2AfromInputQueue);
	
	// Print results as they become available
	for (int i = 0; i < totalInputs; ++i)
		printResultsFromOutputQueue();

	// Terminate all threads
	for (auto &worker : workerPool)
		worker.join();

	return 0;
}
