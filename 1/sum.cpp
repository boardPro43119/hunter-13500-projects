/*
sum.cpp
Author: Benjamin Prud'homme
Course: CSCI-135
Assignment: Project 1A

Reads a sequence of integers from cin (via console input
or input redirection) and prints out their sum.
*/


#include <iostream>
using namespace std;

int main(){
	// Initialize variables
	int sum = 0;
	int currentNum = 0; // Variable to store next integer

	// As long as there is input remaining to be read,
	// add each integer to the sum
	while(cin >> currentNum){
		sum = sum + currentNum;
	}

	// Print out sum
	cout << sum;

	return 0;
}