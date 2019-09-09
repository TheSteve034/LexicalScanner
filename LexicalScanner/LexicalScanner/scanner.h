#pragma once
//Class declaration for the scanner class. This class has all the methods needed to identify a legal token.
#include <string>
#include <vector>
#include <iostream>
#include <regex>

class scanner {
	
private:
	const std::string reservedWord[25] = { "and", "array", "begin", "case",
	"default", "do", "else", "end", "if", "not", "of", "or", "procedure",
	"program", "read", "switch", "then", "var", "while", "write", "int",
	"string", "boolean", "true", "false" };

	const std::string specialSymbols[22] = { "+", "-", "*", "=", "<>", "<",
	">", "<=", ">=", "(", ")", "[", "]", ":=", ".", ",", ";", ":", "..", "'",
	"\"", "/" };

};
