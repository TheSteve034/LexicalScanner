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
	}
	if (tokenCount == 0) {
		currToken = sc.toCaps(tokens[tokenCount]);
		tokenCount++;
		nextToken = sc.toCaps(tokens[tokenCount]);
		tokenCount++;
	}
	else {
		currToken = nextToken;
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
	if (nextToken != ";") {
		error << "SYNTAX ERROR. DID NOT FIND \":\" AFTER PROGRAM <IDENTIFIER>. FAILED IN parser::programRule." << std::endl;
		std::cout << "SYNTAX ERROR! MISSING \";\" IN PROGRAM DECLRATION." << std::endl;
		return -1;
	}
	else {
		getNextToken();
		getNextToken();
		if (blockRule() != 0) {
			error << "SYNTAX ERROR. DID NOT FIND WELL FORMED BLOCK. FAILED CALL to parser::blockRule from parser::programRule()." << std::endl;
			std::cout << "SYNTAX ERROR! MALFORMED PROGRAM BLOCK." << std::endl;
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
	if (currToken != "VAR" && currToken != "PROCEDURE" && currToken != "BEGIN") {
		error << "SYNTAX ERROR! MISSING \"VAR\" OR \"PROCEDURE\" OR \"BEGIN\". FAILED IN parser::blockRule()" << std::endl;
		std::cout << "SYNTAX ERROR! MISSING \"VAR\" OR \"PROCEDURE\" OR \"BEGIN\ AFTER PROGRAM DECLRATION." << std::endl;
		return -1;
	}
	else {
		if (currToken == "VAR") {
			getNextToken();
			if (varDecRule() != 0) {
				error << "SYNTAX ERROR MALFORMED VAR BLOCK. Faild in parser::blockRule" << std::endl;
				std::cout << "SYNTAX ERROR! MALFORMED VAR BLOCK." << std::endl;
				return -1;
			}
		}
		if (nextToken == "PROCEDURE") {
			//call proc method
		}
		if (nextToken == "BEGIN" ) {
			//call BEGIN method
		}
	}
	return 0;
}

/*
var <variable declaration> ; <more vars> | <empty-string>
*/
int parser::varDecRule() {
	//needs to check for a list of 1 or more vars with the following format
	// <name> : <type>, repeate | ;
	if (sc.isId(currToken)) {
		//call moreDeclsRule;
		getNextToken();
		if( moreDeclsRule() != 0) {
			error << "SYNTAX ERROR! INVALID VARIABLE DECLARATION. Failed in parser::varDeclRule." << std::endl;
			return -1;
		}
	}
	else {
		error << "SYNTAX ERROR! INVALID VARIABLE DECLARATION IN A VAR BLOCK. Failed in parser::varDecRule" << std::endl;
		return -1;
	}

	//getNextToken();
	//need to handle arrays here
	if (currToken == "ARRAY") {
		getNextToken();
		arrayDefRule();
	}
	
	if (currToken == "BEGIN" || currToken == "PROCEDURE") {
		return 0;
	}
	//check for ";" at the end of var declarations.
	getNextToken();
	if (currToken != ";") {
		error << "SYNTAX ERROR! MISSING \";\" AT END OF VAR DECLARATION. Failed in parser::varDecRule" << std::endl;
		std::cout << "SYNTAX ERROR! Missing \";\" at the end of a variable declaration." << std::endl;
		return -1;
	}
	//now if the next token is PROCEDURE OR BEGIN. If this is the case then I need to call moreVarsRule();
	//check to see if there are more var types.
	getNextToken();
	if (currToken != "PROCEDURE" && currToken != "BEGIN") {
		if (varDecRule() != 0) {
			error << "SYNTAX ERROR MALFORMED VAR BLOCK. Faild in parser::blockRule" << std::endl;
			std::cout << "SYNTAX ERROR! MALFORMED VAR BLOCK." << std::endl;
			return -1;
		}
	}
	return 0;
}

/*
 : <type> | , <variable declaration>
*/
int parser::moreDeclsRule() {
	/* To options here. 1st case is just one var in that case we are looking for curre token to be ":"
	2ns case list of vars, in this case we are looking for ","*/
	if (currToken != ":" && currToken != ",") {
		error << "SYNTAX ERROR! INVALID CHARACTER TYPE. Failed in parser::moreDeclsRule." << std::endl;
		std::cout << "SYNTAX ERROR! VARIABLE DECLARATIONS MUST HAVE EITHER \":\" OR \",\" IMEDIATELY AFTER. INVALID TOKEN " + currToken << std::endl;
		return -1;
	}
	/*Now if it's the first case then we are looking for current token to be a valid type.
	Otherwise we call varDeclRule*/
	if (currToken == ":") {
		getNextToken();
		if (currToken != "INT" && currToken != "BOOLEAN" && currToken != "STRING" && currToken != "ARRAY") {
			error << "SYNTAX ERROR! UNKNOWN VARIABLE TYPE. Failed in parser::moreDeclsRule." << std::endl;
			std::cout << "SYNTAX ERROR! UNKNOWN TYPE " + currToken << std::endl;
			return -1;
		}
		else {
			return 0;
		}
	}
	else {
		getNextToken();
		if (varDecRule() != 0) {
			//some errors
		}
	}
}

int parser::moreVarsRule() {
	if (!sc.isId(currToken)) {
		error << "SYNTAX ERROR! INVALID IDENTIFIER. Failed in parser::moreVarsRule()" << std::endl;
		std::cout << "SYNTAX ERROR! INVALID IDENTIFIER IN A VAR BLOCK : " + currToken << std::endl;
		return -1;
	}
	else {
		getNextToken();
		if (moreDeclsRule() != 0) {
			error << "SYNTAX ERROR! INVALID VARIABLE DECLARATION. Failed in parser::moreVarsRule." << std::endl;
			return -1;
		}
	}
}
/*
<array type>	::=	array   [   <index range>   of   <simple type>

	// Array declarations can be of any simple type and also
	// can be multidimensional. See the handout on arrays for
	// determining the offset of an array element. Here are a
	// few examples:
	//     bob : array [0..10, -5..5] of int;
	//     harry : array [1..100] of string;

*/
int parser::arrayDefRule() {
	if (currToken != "[") {
		error << "SYNTAX ERROR! MISSING OPENING BRACKET FOR ARRAY DECLARATION. Failed in parser::arrayDefRule" << std::endl;
		std::cout << "SYNTAX ERROR! MISSING OPENING BRACKET FOR ARRAY DECLARATION. " + currToken << std::endl;
		return -1;
	}
	else {
		//call another methe to check if it is a valid index range.
		if (indexRangeRule() != 0) {
			error << "SYNTAX ERROR! INVALID ARRAY DECLARATION. Failed in parser::arrayDefRule" << std::endl;
			std::cout << "SYNTAX ERROR! INVALID ARRAY DECLARATION. " << std::endl;
			return -1;
		}
		else {
			getNextToken();
			if (currToken != "OF") {
				error << "SYNTAX ERROR! INVALID ARRAY DECLARATION. Failed in parser::arrayDefRule" << std::endl;
				std::cout << "SYNTAX ERROR! INVALID ARRAY DECLARATION. THE WORD \"of\" MUST BE USED BETWEEN ARRAY INDEX RANGE LIST AND ARRAY TYPE." << std::endl;
				return -1;
			}
			else {
				//now check to method to test for simple type rule.
				getNextToken();
				if (currToken != "INT" && currToken != "BOOLEAN" && currToken != "STRING") {
					error << "SYNTAX ERROR! UNKNOWN VARIABLE TYPE. Failed in parser::arrayDefRule." << std::endl;
					std::cout << "SYNTAX ERROR! UNKNOWN TYPE " + currToken << std::endl;
				}
				else {
					return 0;
				}
			}
		}
	}
}

/*
<index range>	::=	<index bound> . . <index bound>   <index list>
<index bound>	::=	<sign>   <integer constant>
*/
int parser::indexRangeRule() {
	//first we will check to see if currToken is a sign (- or +) if it is then check to see if the next token is 
	//an int constant.
	getNextToken();
	if (currToken == "-" || currToken == "+") {
		getNextToken();
		if (!sc.isIntConst(currToken)) {
			error << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. Failed in parser::indexRangeRule" << std::endl;
			std::cout << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. " + currToken << std::endl;
			return -1;
		}
		else {
			//now check for ".."
			getNextToken();
			if (currToken != "..") {
				error << "SYNTAX ERROR! INVALID TOKEN. Failed in parser::indexRangeRule" << std::endl;
				std::cout << "SYNTAX ERROR! INVALILD TOKEN, INDEX RANGE USES THE FORM <int const>..<int const>. " + currToken << std::endl;
				return -1;
			}
			else {
				//check to see if the next token is a signed int const
				getNextToken();
				if (currToken == "-" || currToken == "+") {
					getNextToken();
					if (!sc.isIntConst(currToken)) {
						error << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. Failed in parser::indexRangeRule" << std::endl;
						std::cout << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. " + currToken << std::endl;
						return -1;
					}
				}
				else {
					//it wasn't a signed int const. Now check to see if it is an int const.
					if (!sc.isIntConst(currToken)) {
						error << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. Failed in parser::indexRangeRule" << std::endl;
						std::cout << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. " + currToken << std::endl;
						return -1;
					}
				}
			}
		}

	}
	//this the case that it is an unsigned number
	else{
		if (!sc.isIntConst(currToken)) {
			error << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. Failed in parser::indexRangeRule" << std::endl;
			std::cout << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. " + currToken << std::endl;
			return -1;
		}
		else {
			//now check for ".."
			getNextToken();
			if (currToken != "..") {
				error << "SYNTAX ERROR! INVALID TOKEN. Failed in parser::indexRangeRule" << std::endl;
				std::cout << "SYNTAX ERROR! INVALILD TOKEN, INDEX RANGE USES THE FORM <int const>..<int const>. " + currToken << std::endl;
				return -1;
			}
			else {
				//check to see if the next token is a signed int const
				getNextToken();
				if (currToken == "-" || currToken == "+") {
					getNextToken();
					if (!sc.isIntConst(currToken)) {
						error << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. Failed in parser::indexRangeRule" << std::endl;
						std::cout << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. " + currToken << std::endl;
						return -1;
					}
				}
				else {
					//it wasn't a signed int const. Now check to see if it is an int const.
					if (!sc.isIntConst(currToken)) {
						error << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. Failed in parser::indexRangeRule" << std::endl;
						std::cout << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. " + currToken << std::endl;
						return -1;
					}
				}
			}
		}
	}
	//now we need to check for an index list
	getNextToken();
	if (indexListRule() != 0) {
		return -1;
	}
}

/*
<index list>	::=	,   <index bound> . . <index bound>   <index list>   |	]
*/
int parser::indexListRule() {
	//first need to check if currToken is either "]" or ","
	if (currToken != "]" && currToken != ",") {
		error << "SYNTAX ERROR! INNCORRECTLY BOUNDED ARRAY DEFFINITON. Failed in parser::indexListRule" << std::endl;
		std::cout << "SYNTAX ERROR! INNCORRECTLY BOUNDED ARRAY DEFFINITON. " + currToken << std::endl;
		return -1;
	}
	else if (currToken == "]") {
		//if it is "]" then we are done with the array
		return 0;
	}
	else {
		//this the case where we have another list of ranges. This means we are defining a multi-level array
		//so we call indexRangeRule
		if (indexRangeRule() != 0) {
			error << "SYNTAX ERROR! INNCORRECTLY BOUNDED ARRAY DEFFINITON. Failed in parser::indexListRule" << std::endl;
			std::cout << "SYNTAX ERROR! INNCORRECTLY BOUNDED ARRAY DEFFINITON. " + currToken << std::endl;
			return -1;
		}
		else {
			return 0;
		}
	}
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