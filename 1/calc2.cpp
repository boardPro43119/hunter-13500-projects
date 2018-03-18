/*
calc2.cpp
Author: Benjamin Prud'homme
Course: CSCI-135
Assignment: Project 1C

Solves a series of arbitrarily long addition/subtraction
equations with integers (separated by semicolons).
*/

#include <iostream>
using namespace std;

int main(){
	// Initialize variables
	int result = 0; // Result of current equation

	int currentNum = 0; // Variable to store next integer

	// We initialize currentOperator to a '+' because the first integer is always added
	char currentOperator = '+'; // Variable to store operator that comes before each number

	// Read in each integer, update equation
	while(cin >> currentNum){
		// Add/subtract integer
		if(currentOperator == '+'){
			result = result + currentNum;
		}
		else if(currentOperator == '-'){
			result = result - currentNum;
		}

		// Read in following character
		cin >> currentOperator;

		// If this character is a semicolon, print out result and begin new equation
		if(currentOperator == ';'){
			cout << result << endl;
			result = 0;
			currentOperator = '+';
		}

	}

	return 0;
}