#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "scanner.h"
#include "compiler.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "Please provide a file for scanning." << std::endl;
		return -1;
	}

	scanner sc = scanner();

	std::string filePath = argv[1];
	tokens = sc.getTokens(filePath);
	sc.scan(tokens);
}

std::string getNextToken() {
	std::string currToken = tokens[tokenCount];
	if (currToken == "_EOF") {
		lineCount++;
	}
	tokenCount++;
	return currToken;
}