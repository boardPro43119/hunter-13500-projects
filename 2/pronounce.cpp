/*
pronounce.cpp
Author: Benjamin Prud'homme
Course: CSCI-135
Assignment: Project 2

(description)
*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Prototypes
string getPronunciation(string word);
string getIdenticalPronunciations(string pronunciation);
string getAddPronunciations(string pronunciation);
string getRemovePronunciations(string pronunciation);
string getReplacePronunciations(string pronunciation);
string toUpperCase(string str);
void splitOnSpace(string s, string & before, string & after);

// Driver
int main(){
	// Initialize variables
	string input; // Inputted word
	string pronunciation; // Pronunciation of inputted word	

	// Prompt user for word (case insensitive)
	cout << "Enter a word: ";
	cin >> input;

	// Search dictionary for input's pronunciation
	pronunciation = getPronunciation(input);
	// Print out pronunciation (still need to handle not found case)
	cout << "Pronuncation: " << pronunciation << endl << endl;

	// Print out search results
	cout << "Identical pronunciation: " << getIdenticalPronunciations(pronunciation) << endl;
	cout << "Add one phoneme: " << getAddPronunciations(pronunciation) << endl;
	cout << "Remove one phoneme: " << getRemovePronunciations(pronunciation) << endl;
	cout << "Replace one phoneme: " << getReplacePronunciations(pronunciation) << endl;
}

// Returns the pronunciation of the passed-in word.
string getPronunciation(string word){
	string currentLine; // Current line in dictionary file
	string currentWord; // Current word being searched
	string currentPronunciation; // Pronunciation of current word being searched
	
	ifstream inputStream; // Input file stream to read from dictionary file

	// Open stream
	inputStream.open("cmudict.0.7a");

	// If stream fails to open, print error message and exit program
	if(inputStream.fail()){
		cout << "Error: file stream failed to open";
		exit(1);
	}

	// Search dictionary file for word
	while(getline(inputStream, currentLine)){

		// Split each line into word and its pronunciation
		splitOnSpace(currentLine, currentWord, currentPronunciation);

		// If word is found, return its pronunciation and end function
		if(toUpperCase(word)==currentWord){
			return currentPronunciation;
		}
	}

	// If word is not found, return error message
	return "Word not found";
}

string getIdenticalPronunciations(string pronunciation){

}

string getAddPronunciations(string pronunciation){

}

string getRemovePronunciations(string pronunciation){

}

string getReplacePronunciations(string pronunciation){

}

// Returns the conversion of a string to uppercase.
string toUpperCase(string str){

	string result = ""; // Result string

	for(int i=0; i<str.length(); i++){

		// If a character is alphabetic and lowercase, convert it to uppercase and add it to result string
		if(isalpha(str[i]) && str[i]>96){
			result += (char)str[i]-32;
		}

		// Otherwise, add original character to result string
		else{
			result += str[i];
		}
	}

	return result;
}


void splitOnSpace(string s, string & before, string & after) {
	// reset strings
	before = ""; 
	after = "";

	// accumulate before space
	int i = 0;
	while(i < s.size() && not isspace(s[i])){
		before += s[i]; i++;
	}

	// skip the space
	i++;

	// accumulate after space
	while (i < s.size()){
		after += s[i]; i++;
	}

}