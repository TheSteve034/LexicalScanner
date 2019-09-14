//Class definition for the scanner class
#include <algorithm>
#include "scanner.h"

std::regex idRE("[A-Z][A-Z 0-9 _]*");
std::regex singelLineCommenetRE("\/\/.*");
std::regex intConstantRE("[1-9][0-9]*");

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

//bool scanner::isSingelLineComment(std::string token) {
//	if (std::regex_match(token, singelLineCommenetRE)) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}
//
//bool scanner::isMultiLineCommentStart(std::string token) {
//	if (token == "/*") {
//		return true;
//	}
//	else {
//		return false;
//	}
//}
//
//bool scanner::isMultiLineCommentEnd(std::string token) {
//	if (token == "*/") {
//		return true;
//	}
//	else {
//		return false;
//	}
//}

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

//bool scanner::isStringConst(std::string token) {
//	std::string capToken = toCaps(token);
//	if (std::regex_match(capToken, stringConstantRE)) {
//		std::cout << "STRING CONSTANT\t" + capToken << std::endl;
//		return true;
//	}
//	else {
//		return false;
//	}
//}
//
//bool scanner::isStringConstStart(std::string token) {
//	if (token == "\"") {
//		return true;
//	}
//	else {
//		return false;
//	}
//}
//
//bool scanner::isStringContEnd(std::string token) {
//	if (token == "\"") {
//		return true;
//	}
//	else {
//		return false;
//	}
//}

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

bool scanner::isEOF(std::string token) {
	if (token == "_EOF") {
		std::cout << "FILE IS EMPTY" << std::endl;
		return true;
	}
	else {
		return false;
	}
}

bool scanner::isNotEOL(std::string token) {
	if (token == "_EOL") {
		return false;
	}
	else {
		return true;
	}
}

/* The scan method takes the list of prepared tokens and identifies them as different lexems in the grammar.
The scaner works as follows:
1 - checks to see if the first token in _EOF, if so it reports an empty file.
2 - checks to see if the token is " this will indicate that all token that come after are in a string constant
	this check will ignore _EOL to allow for multi line strings. This will be true untill it scans another ". 
	The scanner will then report a string const. If it encounters an _EOF before finding the closing " it will
	report and illegal token.
3 - The scanner will next check for /* this will imply a multiline comment. The scanner will then ignore all
	subsequent tokens untill it encounters the closing multi line comment token. If it finds the _EOF token first
	it will report the commnet as an illegal token.
4 - the scanner will next look for the // token. This will imply a single line comment. The scanner will ignore
	all tokens after this until it finds an _EOL or _EOF.
5 - the scanner will now test to see if a token is a reserved word
6 - the scanner will now test to see if a token is a special symbol
7 - the scanner will now tets to see if a token is an inter constant
8 - the scanner will now tets to see if a token is an identifier
9 - if the token is not identified in any of the previous steps the scanner will report it as an illegal token.

the scanner should never report out _EOL or _EOF these are being used a special tokens and they are also illegal in
in the grammar. If the user atempts to use it as an intetifier it will either produce an illegal token or a malformed line
that will be caught by the parser.
*/
void scanner::scan(std::vector<std::string> tokens) {
	bool lexemeFound = false;
	bool inComment = false;
	bool inMultiLineComment = false;
	bool inStringConst = false;
	std::string tempStringConst = "";
	for (int i = 0; i < tokens.size(); i++) {
		//checking for " token
		if (tokens[i] == "\"" && inStringConst == false) {
			std::cout << "SPECIAL SYMBOL\t" + tokens[i] << std::endl;
			inStringConst = true;
			continue;
		}
		//handeling the string const
		if (inStringConst == true && (tokens[i] != "\"" && tokens[i] != "_EOF")) {
			std::string capToken = toCaps(tokens[i]);
			std::regex charOrNumRE("[A-Z0-9]*");
			//if this the first token in the string just add it
			if (tempStringConst.empty()) {
				if (isNotEOL(capToken)) {
					tempStringConst += capToken;
				}
				continue;
			}
			//if this is not the first string check to see if it is a non alphabet char
			//if it is add a space and the token. If not just add the token.
			if (std::regex_match(capToken, charOrNumRE)) {
				if (tempStringConst.empty()) {
					if (isNotEOL(capToken)) {
						tempStringConst += capToken;
						continue;
					}
				}
				else {
					if (isNotEOL(capToken) && tokens[i] != "\"") {
						tempStringConst += " ";
						tempStringConst += capToken;
						continue;
					}
					continue;
				}
			}
			if (isNotEOL(capToken) && tokens[i] != "\"") {
				if (tempStringConst.empty()) {
					tempStringConst += capToken;

				}
				else {
					tempStringConst += " ";
					tempStringConst += tokens[i];
				}
			}
		}
		//handeling an unbounded string that goes to EOF
		if (inStringConst == true && tokens[i] == "_EOF") {
			std::cout << "ILLEGAL TOKEN\t" + tempStringConst;
			break;
		}
		//handeling the end of the string
		if (inStringConst == true && tokens[i] == "\"") {
			std::cout << "STRING CONSTANT\t" + tempStringConst << std::endl;
			std::cout << "SPECIAL SYMBOL\t" + tokens[i] << std::endl;
			inStringConst = false;
			tempStringConst = "";
			continue;
		}

		//handle single line comment
		if (tokens[i] == "//" ) {
			inComment = true;
			continue;
		}
		if (inComment == true && tokens[i] != "_EOL") {
			continue;
		}
		if (inComment == true && tokens[i] == "_EOL") {
			inComment = false;
			continue;
		}

		//handle multi line coments
		if (tokens[i] == "/*") {
			inMultiLineComment = true;
		}
		if (inMultiLineComment == true && (tokens[i] != "_EOF" && tokens[i] != "*/")) {
			continue;
		}
		if (inMultiLineComment == true && tokens[i] == "_EOF") {
			std::cout << "ILLEGAL TOKEN\tUnbounded multiline comment found at end of file" << std::endl;
			break;
		}
		if (inMultiLineComment == true && tokens[i] == "*/") {
			inMultiLineComment = false;
			continue;
		}

		//test if a token is a reserved word
		if (isReserved(tokens[i])) {
			continue;
		}

		//test if a token is a special character
		if (isSpecial(tokens[i])) {
			continue;
		}

		//test if a token is an int const
		if (isIntConst(tokens[i])) {
			continue;
		}

		//tests if a token is an identifier
		if (isId(tokens[i])) {
			continue;
		}

		//token is now considered an illegal token
		if (tokens[i] != "_EOL" && tokens[i] != "_EOF") {
			std::cout << "ILLEGAL TOKEN\t" + tokens[i] << std::endl;
		}
	}

	/*std::cout << "\nTokens\n" << std::endl;
	for (auto& token : tokens) {
		std::cout << token << std::endl;
	}*/
}
