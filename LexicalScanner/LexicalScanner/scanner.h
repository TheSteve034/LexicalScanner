#pragma once
//Class declaration for the scanner class. This class has all the methods needed to identify a legal token.
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <regex>

class scanner {
	
private:
	//const arrays used to define the reserved words and special characters
	const std::string reservedWords[25] = { "AND", "ARRAY", "BEGIN", "CASE",
	"DEFAULT", "DO", "ELSE", "END", "IF", "NOT", "OF", "OR", "PROCEDURE",
	"PROGRAM", "READ", "SWITCH", "THEN", "VAR", "WHILE", "WRITE", "INT",
	"STRING", "BOOLEAN", "TRUE", "FALSE" };

	const std::string specialSymbols[22] = { "+", "-", "*", "=", "<>", "<",
	">", "<=", ">=", "(", ")", "[", "]", ":=", ".", ",", ";", ":", "..", "'",
	"\"", "/" };

public:
	bool isReserved(std::string token);
	bool isSpecial(std::string token);
	bool isIntConst(std::string token);
	bool isId(std::string token);
	bool isEOF(std::string token);
	bool isNotEOL(std::string token);
	bool isStringConst(std::string token);

	std::string toCaps(std::string token);
	std::vector<std::string> getTokens(std::string filePath);
	int scan(std::vector<std::string> tokens);

};
