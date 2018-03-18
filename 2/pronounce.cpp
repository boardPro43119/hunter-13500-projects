/*
pronounce.cpp
Author: Benjamin Prud'homme
Course: CSCI-135
Assignment: Project 2


*/

#include <iostream>
using namespace std;

// Prototypes
string getPronunciation(string word);
void searchForSimilar(string pronunciation);
string printArrayContents(string[] arr);

// Driver
int main(){
	// Initialize variables
	string input; // Inputted word
	string pronunciation; // Pronunciation of inputted word	
	
	// Arrays to store similarly pronounced words
	string[] identicalPhonemes;
	string[] addPhoneme;
	string[] removePhoneme;
	string[] replacePhoneme;

	// Prompt user for word (case insensitive)
	cout << "Enter a word: ";
	cin >> input;

	// Search dictionary for input's pronunciation
	pronunciation = getPronunciation(input);
	// Print out pronunciation (still need to handle not found case)
	cout << "Pronuncation: " << pronunciation << endl << endl;

	// Search dictionary for similarly pronounced words
	searchForSimilar(pronunciation, identicalPhonemes, addPhoneme, removePhoneme, replacePhoneme);

	// Print out search results
	cout << "Identical pronunciation: " << printArrayContents(identicalPhonemes) << endl;
	cout << "Add one phoneme: " << printArrayContents(addPhoneme) << endl;
	cout << "Remove one phoneme: " << printArrayContents(removePhoneme) << endl;
	cout << "Replace one phoneme: " << printArrayContents(replacePhoneme) << endl;
}