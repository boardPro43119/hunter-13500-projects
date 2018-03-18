/*
calc3.cpp
Author: Benjamin Prud'homme
Course: CSCI-135
Assignment: Project 1D

Solves a series of arbitrarily long addition/subtraction
equations containing squared integers.
*/

#include <iostream>
using namespace std;

int main(){
	// Initialize variables
	int result = 0; // Result of current equation

	int currentNum = 0; // Variable to store next integer

	// We initialize currentOperator to a '+' because the first integer is always added
	char currentOperator = '+'; // Variable to store operator coming before currentNum
	
	char nextOperator = ' '; // Variable to store operator coming after currentNum

	// Read in each integer, update equation
	while(cin >> currentNum){
		// Check if integer is squared
		
		cin >> nextOperator; // Read operator following integer
		
		// If this character is a '^', square integer
		if(nextOperator == '^'){
			currentNum *= currentNum;
		}

		// Add/subtract currentNum
		if(currentOperator == '+'){
			result = result + currentNum;
		}
		else if(currentOperator == '-'){
			result = result - currentNum;
		}

		// Advance to next operator if square operation was performed
		if(nextOperator == '^'){
			cin >> nextOperator;
		}

		// Here, we are setting currentOperator for the next currentNum equal to nextOperator for this currentNum
		//
		currentOperator = nextOperator;

		// If following operator is a semicolon, print result and begin new equation
		if(currentOperator == ';'){
			cout << result << endl;
			result = 0;
			currentOperator = '+';
		}
		

	}

	return 0;
}