/*
pronounce.cpp
Author: Benjamin Prud'homme
Course: CSCI-135
Assignment: Project 2

Prints out the pronunciation (as given by the CMU Pronouncing Dictionary)
of a word entered by the user, as well as a list of words with similar
pronunciations.
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
using namespace std;

// Prototypes
string getPronunciation(string word);
string searchForSimilar(string inputWord, string inputPronunciation);
bool pronunciationsIdentical(string pronunciation1, string pronunciation2);
bool pronunciationsAddedPhoneme(string pronunciation1, string pronunciation2);
bool pronunciationsRemovedPhoneme(string pronunciation1, string pronunciation2);
bool pronunciationsReplacedPhoneme(string pronunciation1, string pronunciation2);
int numPhonemes(string pronunciation);
string toUpperCase(string str);
bool validWord(string word);
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
	
	if(pronunciation == "Not found"){

		cout << "Not found";

	}

	else {

		// Print out pronunciation
		cout << "Pronuncation: " << pronunciation << endl << endl;

		// Search dictionary for similarly pronounced words and print out results
		cout << searchForSimilar(input, pronunciation);

	}

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

		// If valid word is found, return its pronunciation and end function
		if((toUpperCase(word)==currentWord) && validWord(currentWord)){
			return currentPronunciation;
		}
	}

	// If word is not found, return error message
	return "Not found";
}

// Returns the results of searching the dictionary file for words with
// pronunciations similar to the passed-in pronunciation.
string searchForSimilar(string inputWord, string inputPronunciation){
	// Strings to store lists of similarly pronounced words
	string identicalMatches = "";
	string addMatches = "";
	string removeMatches = "";
	string replaceMatches = "";

	// Variables for searching dictionary file
	string currentLine; // Current line in file
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

	// Search dictionary file for similar pronounciations
	while(getline(inputStream, currentLine)){

		// Separate each word from its pronunciation, update the corresponding variables
		splitOnSpace(currentLine, currentWord, currentPronunciation);

		// Check each valid word in dictionary for each kind of similarity (identical pronunciation,
		// one added phoneme, one removed phoneme, or one replaced phoneme) and add current word to corresponding list
		if(validWord(currentWord)){
			if(pronunciationsIdentical(inputPronunciation, currentPronunciation) && (toUpperCase(inputWord) != currentWord)){
				identicalMatches += (currentWord + " ");
			}
			else if(pronunciationsAddedPhoneme(inputPronunciation, currentPronunciation)){
				addMatches += (currentWord + " ");
			}
			else if(pronunciationsRemovedPhoneme(inputPronunciation, currentPronunciation)){
				removeMatches += (currentWord + " ");
			}
			else if(pronunciationsReplacedPhoneme(inputPronunciation, currentPronunciation)){
				replaceMatches += (currentWord + " ");
			}
		}
	}

	// Create string to return results
	string result = "";
	result += "Identical        : " + identicalMatches + "\n";
	result += "Add phoneme      : " + addMatches + "\n";
	result += "Remove phoneme   : " + removeMatches + "\n";
	result += "Replace phoneme  : " + replaceMatches + "\n";

	return result;

}

// Returns true if the two passed-in pronuncations are identical; false otherwise.
bool pronunciationsIdentical(string pronunciation1, string pronunciation2){
	if(pronunciation1 == pronunciation2){
		return true;
	}
	return false;
}

// Returns true if the second passed-in pronunciation is the result of adding one phoneme
// to the first; false otherwise.
bool pronunciationsAddedPhoneme(string pronunciation1, string pronunciation2){
	// Variables to store last extracted phoneme from pronunciation1 and pronunciation2
	string currentPhoneme1, currentPhoneme2;
	// Variables to store remainder of pronunciation1 and pronunciation2 after extracting a phoneme
	string remainingPronunciation1, remainingPronunciation2;

	// If pronunciation2 does not have one more phoneme than pronunciation1, return false
	if(numPhonemes(pronunciation2) != numPhonemes(pronunciation1) + 1){
		return false;
	}

	// Extract first phoneme of both pronunciations
	splitOnSpace(pronunciation1, currentPhoneme1, remainingPronunciation1);
	splitOnSpace(pronunciation2, currentPhoneme2, remainingPronunciation2);

	// While corresponding phonemes are identical, continue extracting
	while(currentPhoneme1 == currentPhoneme2){
		splitOnSpace(remainingPronunciation1, currentPhoneme1, remainingPronunciation1);
		splitOnSpace(remainingPronunciation2, currentPhoneme2, remainingPronunciation2);
	}

	// If we don't find a difference before reaching the end of pronunciation1, currentPhoneme1
	// will be empty when exiting the while loop above.  In this case, pronunciation2 is the result of
	// adding one phoneme to the end of pronunciation1; return true
	if(currentPhoneme1.empty()){
		return true;
	}

	// If we find a difference before reaching the end of pronunciation1, extract the next
	// phoneme from pronunciation2, which should be identical to the previous phoneme in pronunciation1
	splitOnSpace(remainingPronunciation2, currentPhoneme2, remainingPronunciation2);

	// Now check if each remaining phoneme in pronunciation2 is identical to the previous phoneme
	// in pronunciation1.  If this holds true, return true; otherwise return false
	while(!(currentPhoneme1.empty() && currentPhoneme2.empty())){
		if(currentPhoneme1 != currentPhoneme2){
			return false;
		}
		splitOnSpace(remainingPronunciation1, currentPhoneme1, remainingPronunciation1);
		splitOnSpace(remainingPronunciation2, currentPhoneme2, remainingPronunciation2);
		
	}
	return true;

}

// Returns true if the second passed-in pronunciation is the result of removing one phoneme
// from the first; false otherwise.
bool pronunciationsRemovedPhoneme(string pronunciation1, string pronunciation2){
	// Variables to store last extracted phoneme from pronunciation1 and pronunciation2
	string currentPhoneme1, currentPhoneme2;
	// Variables to store remainder of pronunciation1 and pronunciation2 after extracting a phoneme
	string remainingPronunciation1, remainingPronunciation2;

	// If pronunciation2 does not have one less phoneme than pronunciation1, return false
	if(numPhonemes(pronunciation2) != numPhonemes(pronunciation1) - 1){
		return false;
	}

	// Extract first phoneme of both pronunciations
	splitOnSpace(pronunciation1, currentPhoneme1, remainingPronunciation1);
	splitOnSpace(pronunciation2, currentPhoneme2, remainingPronunciation2);

	// While corresponding phonemes are identical, continue extracting
	while(currentPhoneme1 == currentPhoneme2){
		splitOnSpace(remainingPronunciation1, currentPhoneme1, remainingPronunciation1);
		splitOnSpace(remainingPronunciation2, currentPhoneme2, remainingPronunciation2);
	}

	// If we don't find a difference before reaching the end of pronunciation2, currentPhoneme2
	// will be empty when exiting the while loop above.  In this case, pronunciation2 is the result of
	// removing the last phoneme from pronunciation1; return true
	if(currentPhoneme2.empty()){
		return true;
	}

	// If we find a difference before reaching the end of pronunciation2, extract the next
	// phoneme from pronunciation2, which should be identical to the following phoneme in pronunciation1
	splitOnSpace(remainingPronunciation1, currentPhoneme1, remainingPronunciation1);

	// Now check if each remaining phoneme in pronunciation2 is identical to the following phoneme
	// in pronunciation1.  If this holds true, return true; otherwise return false
	while(!(currentPhoneme1.empty() && currentPhoneme2.empty())){
		if(currentPhoneme1 != currentPhoneme2){
			return false;
		}
		splitOnSpace(remainingPronunciation1, currentPhoneme1, remainingPronunciation1);
		splitOnSpace(remainingPronunciation2, currentPhoneme2, remainingPronunciation2);
		
	}
	return true;
}

// Returns true if the second passed-in pronunciation is the result of replacing one phoneme
// in the first; false otherwise.
bool pronunciationsReplacedPhoneme(string pronunciation1, string pronunciation2){
	// Variables to store last extracted phoneme from pronunciation1 and pronunciation2
	string currentPhoneme1, currentPhoneme2;
	// Variables to store remainder of pronunciation1 and pronunciation2 after extracting a phoneme
	string remainingPronunciation1, remainingPronunciation2;
	// Variable to track whether we have encountered a difference
	bool differenceFound = false;

	// If pronunciation1 and pronunciation2 do not have the same number of phonemes, or if
	// they are identical, return false
	if((numPhonemes(pronunciation2) != numPhonemes(pronunciation1)) || pronunciationsIdentical(pronunciation1, pronunciation2)){
		return false;
	}

	// Extract first phoneme of both pronunciations
	splitOnSpace(pronunciation1, currentPhoneme1, remainingPronunciation1);
	splitOnSpace(pronunciation2, currentPhoneme2, remainingPronunciation2);

	// Extract and compare remaining phonemes
	for(int i=0; i<numPhonemes(pronunciation1)+1; i++){
		// Upon encountering our first phoneme difference, set differenceFound to true.  If we find
		// another difference after the first, return false
		if(currentPhoneme1 != currentPhoneme2){
			if(differenceFound){
				return false;
			}
			else{
				differenceFound = true;
			}
		}
		splitOnSpace(remainingPronunciation1, currentPhoneme1, remainingPronunciation1);
		splitOnSpace(remainingPronunciation2, currentPhoneme2, remainingPronunciation2);
	}

	// If we reach the end of the pronunciations and exactly one difference has been
	// found, return true
	return true;

}

// Returns the number of phonemes in the passed-in pronunciation.
int numPhonemes(string pronunciation){
	int phonemes = 0;
	for(int i=0; i<pronunciation.length(); i++){
		if(isspace(pronunciation[i])){
			phonemes++;
		}
	}

	return phonemes;
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

// Checks if a word should be accepted when checking for the input or for similarity.
// Causes words with non-alphabetic characters other than apostrophes to be ignored.
bool validWord(string word){
	for(int i=0; i<word.length(); i++){
		if(!isalpha(word[i]) && word[i] != '\''){
			return false;
		}
	}
	return true;
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