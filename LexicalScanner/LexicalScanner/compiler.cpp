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
	sc.scan(tokens);
	p.getPath(filePath);
	p.createFiles();
	p.getTokenList(tokens);
}