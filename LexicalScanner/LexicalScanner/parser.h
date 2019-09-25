#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "scanner.h"


class parser {
private:
	scanner sc = scanner();
	std::vector<std::string> tokens;
	int tokenCount = 0;
	std::string currToken = "";
	std::string nextToken = "";
	std::fstream assembly = {};
	std::fstream error = {};

	int programRule();
	int blockRule();
	int varDecRule();
	int moreDeclsRule();
	int moreVarsRule();
	int arrayDefRule();
	int indexRangeRule();
	int indexListRule();

public:
	//gathers the tokens from the scanner.
	void getTokenList(std::vector<std::string> scanedTokens);
	//sets the value of currToken and nexToken using the tokenCount number;
	void getNextToken();
	//creates .asm and .err files
	void createFiles(std::string progName);
	int parseFile();
};
