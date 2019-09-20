#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


class parser {
private:
	std::vector<std::string> tokens;
	int tokenCount = 0;
	int lineCount = 0;
	
	//gathers the tokens from the scanner.
	void getTokenList();
	std::string getNextToken();
	//need functions for each rule so that that can be called to determine if we have a semantically correct
	//file.
public:
	void parseFile();
};
