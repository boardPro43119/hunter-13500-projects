/*
calc.cpp
Author: Benjamin Prud'homme
Course: CSCI-135
Assignment: Project 1B

Solves an arbitrarily long addition/subtraction equation
with nonnegative integers.
*/

#include <iostream>
using namespace std;

int main(){
	// Initialize variables
	int result = 0; // Result of equation

	int currentNum = 0; // Variable to store next integer

	// We initialize currentOperator to a '+' because the first integer is always added
	char currentOperator = '+'; // Variable to store operator that comes before each number

	// Compute equation
	while(cin >> currentNum){
		// Add/subtract integer
		if(currentOperator == '+'){
			result = result + currentNum;
		}
		else if(currentOperator == '-'){
			result = result - currentNum;
		}

		// Read operator for next integer
		cin >> currentOperator;
	}

	// Print out result
	cout << result;

	return 0;
}