//Class definition for the scanner class
#include <algorithm>
#include "scanner.h"

std::regex idRE("[A-Z][A-Z 0-9 _]*");
std::regex singelLineCommenetRE("\/\/.*");
std::regex intConstantRE("[1-9][0-9]*");
std::regex stringConstantRE("\".*\"");
std::regex missingEndQuote("\"[A-Z ]+");

std::vector<std::string> scanner::getTokens(std::string filePath) {
	std::ifstream file(filePath.c_str(), std::ifstream::in);
	std::string line = "";
	std::vector<std::string> tokens = {};

	while (std::getline(file, line)) {
		std::string token = "";
		bool inStringConst = false;
		int lineLength = line.length();
		for (int i = 0; i < lineLength; i++) {
			//Checks for all two character tokens. In the case of a two char token a token will be made of the currently
			//checked char and the next and the loop variable will be incremented. Two char tokens are defined as special 
			//symbols. This section will also test for symbols that start coments. This will be tracked so that the scanner 
			//method can discard them.

			//tests for the "/*" token
			if (line[i] == '/' && line[i + 1] == '*') {
				token = line[i];
				token += line[i + 1];
				tokens.push_back(token);
				token = "";
				i = i + 1;
				continue;
			}
			//tests for the "*/" token
			if (line[i] == '*' && line[i + 1] == '/') {
				token = line[i];
				token += line[i + 1];
				tokens.push_back(token);
				token = "";
				i = i + 1;
				continue;
			}
			//tests fot he "//" token
			if (line[i] == '/' && line[i + 1] == '/') {
				token = line[i];
				token += line[i + 1];
				tokens.push_back(token);
				token = "";
				i = i + 1;
				continue;
			}
			//tests for all special two char tokens as decribed by the grammar
			if ((line[i] == '<' && line[i + 1] == '>') || (line[i] == '<' && line[i + 1] == '=') || (line[i] == '>' && line[i + 1] == '=')
				|| (line[i] == ':' && line[i + 1] == '=') || (line[i] == '.' && line[i + 1] == '.')) {
				if (token.empty()) {
					token = line[i];
					token += line[i + 1];
					tokens.push_back(token);
					token = "";
					i = i + 1;
					continue;
				}
				else {
					tokens.push_back(token);
					token = line[i];
					token += line[i + 1];
					tokens.push_back(token);
					token = "";
					i = i + 1;
					continue;
				}
			}

			//check for all one char special tokens. These tokens are defined by the grammar.
			if (line[i] == '+' || line[i] == '-' || line[i] == '*' || line[i] == '=' || line[i] == '<' || line[i] == '>'
				|| line[i] == '(' || line[i] == ')' || line[i] == '[' || line[i] == ']' || line[i] == '.' || line[i] == ','
				|| line[i] == ';' || line[i] == '\'' || line[i] == '/' || line[i] == ':'|| line[i] == '\"') {
				if (token.empty()) {
					token = line[i];
					tokens.push_back(token);
					token = "";
					continue;
				}
				else {
					tokens.push_back(token);
					token = line[i];
					tokens.push_back(token);
					token = "";
					continue;
				}
			}

			/*
			Remaining chars that are not caught will be processed as follows
			if a char no white space it will be added to the token

			if char is white then the token will be checked. If it is not empty then the token will be pushed into
			tokens. This implies that we were just recording a token and have hit white space. In this case the token
			will be set to empty. If the token is empty then we skip the white space char.
			*/
			//testing for non white space chars
			if (line[i] != ' ' && line[i] != '\t') {
				token += line[i];
				continue;
			}
			//testing for white space chars.
			if (line[i] == ' ' || line[i] == '\t') {
				if (token.empty()) {
					continue;
				}
				else {
					tokens.push_back(token);
					token = "";
					continue;
				}
			}

		}
		//before the next line is consumed a check is made to see if token is empty
		//if its not then it is added to tokens. Then an _EOL token is added to denote
		//that all the tokens before it are on the same line. The _EOL token is an illegal
		//symbol is the grammar so the scanner can assume that is the end of line. If a user 
		//tries to define this symbol it will cause a complie error. If token is empty then
		//just an EOL char is added.
		if (token.empty()) {
			token = "_EOL";
			tokens.push_back(token);
		}
		else if (!token.empty()) {
			tokens.push_back(token);
			token = "_EOL";
			tokens.push_back(token);
		}
	}
	/*Finally, when the while loop ends we have consumed the whole file an _EOF will be added to
	the tokens list to indicate the the entire file has been tokenized.*/
	std::string endToken = "_EOF";
	tokens.push_back(endToken);
	return tokens;
}


std::string scanner::toCaps(std::string token) {
	std::string capString = "";
	for (int i = 0; i < token.size(); i++) {
		capString+= std::toupper(token[i]);
	}
	return capString;
}

bool scanner::isReserved(std::string token) {
	bool isReserved = false;
	for (const auto & reservedWord : reservedWords) {
		std::string capToken = toCaps(token);
		if (capToken == reservedWord) {
			std::cout << "RESERVED WORD\t" + capToken << std::endl;
			isReserved = true;
			break;
		}
	}
	return isReserved;
}

bool scanner::isSingelLineComment(std::string token) {
	if (std::regex_match(token, singelLineCommenetRE)) {
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isMultiLineCommentStart(std::string token) {
	if (token == "/*") {
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isMultiLineCommentEnd(std::string token) {
	if (token == "*/") {
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isSpecial(std::string token) {
	bool isSpecial = false;
	for (const auto& specialChar : specialSymbols) {
		if (token == specialChar) {
			std::cout << "SPECIAL SYMBOL\t" + token << std::endl;
			return true;
			break;
		}
	}
	return isSpecial;
}

bool scanner::isStringConst(std::string token) {
	std::string capToken = toCaps(token);
	if (std::regex_match(capToken, stringConstantRE)) {
		std::cout << "STRING CONSTANT\t" + capToken << std::endl;
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isStringConstStart(std::string token) {
	if (token == "\"") {
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isStringContEnd(std::string token) {
	if (token == "\"") {
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isIntConst(std::string token) {
	if (token == "0") {
		std::cout << "INTEGER CONSTANT\t" + token << std::endl;
		return true;
	}
	if (std::regex_match(token, intConstantRE)) {
		std::cout << "INTEGER CONSTANT\t" + token << std::endl;
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isId(std::string token) {
	std::string capToken = toCaps(token);
	if (std::regex_match(capToken, idRE)) {
		std::cout << "IDENTIFIER\t" + capToken << std::endl;
		return true;
	}
	else {
		return false;
	}
}

void scanner::scan(std::vector<std::string> tokens) {
	bool lexemeFound = false;
	bool inComment = false;
	bool inStringConst = false;
	std::string tempStringConst = "";
	/*for (auto& token : tokens) {
		if (isSingelLineComment(token) && (inComment == false && inStringConst == false)) {
			lexemeFound = false;
			continue;
		}
		if (isMultiLineCommentStart(token)) {
			lexemeFound = false;
			inComment = true;	
		}
		if (isMultiLineCommentEnd(token)) {
			lexemeFound = false;
			inComment = false;
			continue;
		}
		if (isStringConstStart(token) && inStringConst == false) {
			inStringConst = true;
			tempStringConst += token;
			continue;
		}
		if (isStringContEnd(token) && inStringConst == true) {
			inStringConst = false;
			tempStringConst += token;
			isStringConst(tempStringConst);
			tempStringConst = "";
			continue;
		}
		if (inStringConst) {
			std::regex numORChar("[A-Z0-9]*");
			std::string capToken = toCaps(token);
			if (std::regex_match(capToken, numORChar)) {
				if (tempStringConst == "\"") {
					tempStringConst += capToken;
				}
				else {
					tempStringConst += " ";
					tempStringConst += capToken;
				}
			}
			else {
				tempStringConst += capToken;
			}
			continue;
		}
		if (inComment) {
			continue;
		}
		if (isReserved(token) && (inComment == false && inStringConst == false)) {
			lexemeFound = true;
			continue;
		}
		if (isSpecial(token)) {
			lexemeFound = true;
			continue;
		}
		if (isId(token)) {
			lexemeFound = true;
			continue;
		}
		if (isIntConst(token)) {
			lexemeFound == true;
			continue;
		}
		if (isStringConst(token)) {
			lexemeFound == true;
			continue;
		}
		if (inStringConst == true || inComment == true) {
			lexemeFound = false;
		}
		lexemeFound = false;
		if (lexemeFound == false) {
			std::cout << "ILLEGAL TOKEN\t" + token << std::endl;
		}
	}*/

	std::cout << "Tokens" << std::endl;
	for (auto& token : tokens) {
		std::cout << token << std::endl;
	}
}
