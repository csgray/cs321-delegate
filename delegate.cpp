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
using std::mutex;
#include <thread>
using std::thread;
#include <vector>
using std::vector;

#include "sa2a.h"	// For "int sa2a(int)"

int main() {
	cout << "Delegated Computation" << endl;
	
	int input;
	vector<int> inputBuffer;

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

	for (auto i : inputBuffer) {
		cout << "sa2a(" << i << ") = " << sa2a(i) << endl;
	}
}
