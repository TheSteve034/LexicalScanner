#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "scanner.h"
#include "parser.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "Please provide a file for scanning." << std::endl;
		return -1;
	}

	scanner sc = scanner();
	parser p = parser();

	std::string filePath = argv[1];
	std::vector < std::string > tokens = sc.getTokens(filePath);
	int retval = sc.scan(tokens);
	if (retval == -1) {
		std::cout << "ILLEGAL TOKENS. FILE CANNOT BE SCANNED." << std::endl;
		return -1;
	}
	p.getTokenList(tokens);
	p.parseFile();
}  