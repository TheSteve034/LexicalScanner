#include "parser.h"

void parser::getTokenList(std::vector<std::string> scanedTokens) {
	tokens = scanedTokens;
	return;
}

void parser::getNextToken() {
	if (tokens[tokenCount] == "_EOF") {
		currToken = tokens[tokenCount];
		tokenCount++;
		return;
	}
	if (tokens[tokenCount] == "_EOL") {
		tokenCount++;
		currToken = sc.toCaps(tokens[tokenCount]);
		tokenCount++;
		nextToken = sc.toCaps(tokens[tokenCount]);
		tokenCount++;
	}
	else {
		currToken = sc.toCaps(tokens[tokenCount]);
		tokenCount++;
		nextToken = sc.toCaps(tokens[tokenCount]);
		tokenCount++;
	}
	return;
}

void parser::createFiles(std::string progName) {
	std::string asemFile = progName + ".asm";
	std::string errFile = progName + ".err";
 	assembly.open(asemFile, std::ios::out | std::ios::trunc);
	error.open(errFile, std::ios::out | std::ios::trunc);
}

//these two functions should only be called after getNextToken;
void parser::progressPastSingleLineComments() {
	if (currToken == "//") {
		if (tokenCount != 0) {
			tokenCount--;
		}
		while (tokens[tokenCount] != "_EOL") {
			std::string temp = tokens[tokenCount];
			tokenCount++;
		}
		tokenCount = tokenCount + 2;
		getNextToken();
	}
}

int parser::progressPastMultiLineComments() {
	if (currToken == "/*") {
		if (tokenCount != 0) {
			tokenCount--;
		}
		while (tokens[tokenCount] != "*/") {
			if (tokens[tokenCount] == "_EOL") {
			}
			if (tokens[tokenCount] == "_EOF") {
				return -1;
			}
			tokenCount++;
		}
		tokenCount++;
		if (tokens[tokenCount] == "_EOL") {
			tokenCount++;
		}
		getNextToken();
	}
}

/*
< program >	::=	program   <identifier>   ;   <block>   .
*/
int parser::programRule() {
	//check for the PROGRAM token then that the next token is 
	//an indetifier. If both od these things are the case then 
	//create the assembly file and the error file.
	if (currToken != "PROGRAM") {
		std::cout << "SYNTAX ERROR! Program must start with the word PROGRAM." << std::endl;
		return -1;
	}
	else {
		//check the next token to see if it is an identifier
		if(sc.isId(nextToken)) {
			createFiles(nextToken);
		}
		else {
			std::cout << "SYNTAX ERROR! INVALID IDENTIFIER " + nextToken << std::endl;
			return -1;
		}
	}
	getNextToken();
	//now check to see if the next token is ";"
	if (currToken != ";") {
		error << "SYNTAX ERROR. DID NOT FIND \":\" AFTER PROGRAM <IDENTIFIER>. FAILED IN parser::programRule." << std::endl;
		std::cout << "SYNTAX ERROR! MISSING \";\" IN PROGRAM DECLRATION." << std::endl;
		return -1;
	}
	else {
		if (blockRule() != 0) {
			error << "SYNTAX ERROR. DID NOT FIND WELL FORMED BLOCK. FAILED CALL to parser::blockRule from parser::programRule()." << std::endl;
			std::cout << "SYNTAX ERROR! MALFORMED BLOCK, DID NOT FIND THE WORD VAR, PROCEDURE, OR BEGIN AFTER PROGRAM DECLARATION." << std::endl;
			return -1;
		}
	}
	return 0;
}
/*
<block>	::=	<variable declaration part>
<procedure declaration part>
<statement part>
*/
int parser::blockRule() {
	//needs to check to see if the nextoken is VAR or PROCEDURE or BEGIN
	//if it is not any of these return an error.
	if (nextToken != "VAR" || nextToken != "PROCEDURE" || nextToken != "BEGIN") {
		error << "SYNTAX ERROR! MISSING \"VAR\" OR \"PROCEDURE\" OR \"BEGIN\". FAILED IN parser::blockRule()" << std::endl;
		std::cout << "SYNTAX ERROR! MISSING \"VAR\" OR \"PROCEDURE\" OR \"BEGIN\ AFTER PROGRAM DECLRATION." << std::endl;
		return -1;
 	}
	return 0;
}

int parser::parseFile() {
	getNextToken();
	if (programRule() != 0) {
		//error << "SYNTAX ERROR. Call to program rule failed in function parser::parseFile().";
		std::cout << "SYNTAX ERROR! CANNOT PARSE FILE." << std::endl;
		return -1;
	}
	else {
		return 0;
	}
}