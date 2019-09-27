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
		if (currToken == "PROCEDURE") {
			//call proc method
		}
		if (currToken == "BEGIN" ) {
			statmentPartRule();
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

/*
<procedure declaration part>	::=	<procedure declaration>   ; <procedure declaration part>   |
<empty-string>
*/
int parser::procDeclPartRule() {
	//this rule is only called is currToken is "PROCEDURE"
	//call procDeclRule
	if (procDeclRule() != 0) {
		error << "SYNTAX ERROR! MALFORMED PROCEDURE DECLARATION. Failed in parser::procDeclPartRule" << std::endl;
		std::cout << "SYNTAX ERROR! MALFORMED PROCEDURE DECLARATION." << std::endl;
		return -1;
	}
	//now check for ";"
	//now check if there is another proc decl part
}

/*
<procedure declaration>	::=	procedure <identifier>   (   <parameter list>   ;   <block>

	// Examples:
	//       procedure myProcedure(int * param1, string param2) ;
	//                   begin <statement> end
	//       procedure myProcedure(int * param1, string param2) ;
	//                   begin <statement> end
	//       PROcEDure myproc(); BEGIN <statement> END
	// Semantic note: When a parameter is declared as pass by reference
	// (an * after the type), the expression in the procedure call must
	// evaluate to an address. When a parameter is passed by value,
	// only the value of the expression is used and is stored in a
	// location available to the procedure.

*/
int parser::procDeclRule() {
	//the first step of this rule  is to check for an ID it will only be called if currtoken is PROCEDURE
	getNextToken();
	if (!sc.isId(currToken) != 0) {
		error << "SYNTAX ERROR! IDENTIFIER MUST FOLLOW KEYWORD \"PROCEDURE\". Failed in parser::procDeclRule" << std::endl;
		std::cout << "SYNTAX ERROR! IDENTIFIER MUST FOLLOW KEYWORD \"PROCEDURE\". " + currToken << std::endl;
		return -1;
	}
	else {
		//now check for a "("
		getNextToken();
		if (currToken != "(") {
			error << "SYNTAX ERROR! MISSING THE \"(\" TOKEN WHEN CREATING AN PARAMETER LIST. Failed in parser::procDeclRule" << std::endl;
			std::cout << "SYNTAX ERROR! MISSING THE \"(\" TOKEN WHEN CREATING AN PARAMETER LIST." << std::endl;
			return -1;
		}
		else {
			//now look for a paramter list. Need to call a function for this.
			if (paramListsRule() != 0) {
				error << "SYNTAX ERROR! MALFORMED PARAM LIST. Failed in parser::procDeclRule()" << std::endl;
				std::cout << "SYNTAX ERROR! MALFORMED PARAM LIST. " << std::endl;
				return -1;
			}
		}
	}
}

/*
<parameter list>	::=	<type identifier>   <param passing>   |		)
<type identifier>	::=	int   |		boolean   |		string

	// Semantic note: string is an array of characters which we
	// will limit to 128 bytes (127 characters with a NULL terminator).
	// Type checking must be performed for all statements when
	// variables are involved. The '+' operator is overloaded to perform
	// string concatenation, with the result truncated to fit the
	// defined size. No other operators are used with the string type.
	// The result of any expression evaluation must be the correct type.

*/
int parser::paramListsRule() {
	//check if the next token is a type identifier
	getNextToken();
	if (currToken != "INT" && currToken != "BOOLEAN" && currToken != "STRING") {
		error << "SYNTAX ERROR! EACH PARAMETER IN A PARMETER LIST MUST HAVE A VALID TYPE. Failed in parser::paramListsRule" << std::endl;
		std::cout << "SYNTAX ERROR!EACH PARAMETER IN A PARMETER LIST MUST HAVE A VALID TYPE. " + currToken << std::endl;
		return -1;
	}
	else {
		//now call paramPassingRule()
	}
}

/*
<statement part>	::=	<compound statement>
*/
int parser::statmentPartRule() {
	//this method is called when the currToken is BEGIN.
	if (compoundStatmentRule() != 0) {
		error << "SYNTAX ERROR! MALFORMED STAMENT. Failed in parser::statmentPartRule" << std::endl;
		std::cout << "SYNTAX ERROR! MALFORMED STAMENT." << std::endl;
		return -1;
	}
	else {
		return 0;
	}

}

/*
<compound statement>	::=	begin   <statement>   <more stmts>   end
	// NOTE: The final statement before an END is
	// not terminated by a semicolon.
*/
int parser::compoundStatmentRule() {
	//if this is called then currToken is Begin.
	getNextToken();
	//now check for a statment!
	if (statmentRule() != 0) {
		error << "SYNTAX ERROR! MALFORMED STATMENT. Failed in parser::compoundStatmentRule" << std::endl;
		std::cout << "SYNTAX ERROR! MALFORMED STATMENT." << std::endl;
		return -1;
	}
	else {
		//check for more statments
		if (moreStatmentsRule() != 0) {
			error << "SYNTAX ERROR. MALFORMED STATMENT. Failed in parser::compoundStatmentRule" << std::endl;
			return -1;
		}
		else {
			//test for END.
			if (currToken != "END") {
				error << "SYNTAX ERROR! BEGIN BLOCK MUST TERMINATE WITH THE WORD \"end\". Failed in parser::compoundStatmentRule" << std::endl;
				std::cout << "SYNTAX ERROR!BEGIN BLOCK MUST TERMINATE WITH THE WORD \"end\". " + currToken << std::endl;
				return -1;
			}
		}
	}
	return 0;
}

/*
<more stmts>	::=	;   <statement>   <more stmts>   |	 <empty-string>
*/
int parser::moreStatmentsRule() {
	//first we check for a ";" if we find one then there are more statments
	//if not the then return 0;
	//need to getNetToken
	getNextToken();
	if (currToken != ";") {
		return 0; //there are no more statments
	}
	else {
		//there are more statments. call statment again and then call more statments.
		getNextToken();
		if (statmentRule() != 0) {
			return -1;
		}
		else {
			if (moreStatmentsRule() != 0) {
				error << "SYNTAX ERROR! MALFORMED STATMENT. Failed in moreStatmentsRule()" << std::endl;
				std::cout << "SYNTAX ERROR!MALFORMED STATMENT." << std::endl;
				return -1;
			}
		}
	}
}

/*
<statement>	::=	<simple statement>   |	  <structured statement>
*/
int parser::statmentRule() {
	//chek for a simple stament then for a structured statment
	if (simpleStatmentRule() != 0) {
		error << "SYNTAX ERROR! MALFORMED SIMPLE STATMENT. Failed in parser::statmentRule" << std::endl;
		return -1;
	}
	else {
		//check for <structured statement>
	}

}

/*
<simple statement>	::=	<assignment statement>   |	  <procedure call>   |
<read statement>   |	<write statement>
*/
int parser::simpleStatmentRule() {
	//this method will need to check for all the types of staments
	//assingment statment
	if (assingmentStatmentRule() != 0) {
		error << "MALFORMED SIMPLE STATMENT. Failed in parser::simpleStatmentRule" << std::endl;
		return -1;
	}
	else {
		//check for procedure call
	}
}

/*
<assignment statement>	::=	<variable>   :=   <expression>

	// Array assignments can seem a little tricky, but the actual
	// assignment is of only the component type of the array (a
	// simple type). There is no assignemnt from one array to another,
	// only from an expression to an array element. For example, the
	// following would be valid array assignments:
	//       bob[2,4] := 25;
	//       harry[3*9] := "Gryfyndor forever";
	//   The index value must be checked to ensure it is in the range
	//   specified in the array declaration.
	//       bob[bob[0,-3], bob[bob[2,4],5-num1]] := num2*3-num3/42;
	// Semantic note: Type checking must be performed on all
	// assignments, the right side must match the left side. For string
	// concatenation, the variable and all components of the expression
	// must be string types (remember, the results of concatenation are
	// truncated before the assignment is made).
*/
int parser::assingmentStatmentRule() {
	if (variableRule() != 0) {
		error << "SYNTAX ERROR! MALFORMED ASSINGMENT STATMENT. Failed in parser::assingmentStatmentRule" << std::endl;
		return -1;
	}
	else {
		//now check for the token ":="
		getNextToken();
		if (currToken != ":=") {
			error << "SYNTAX ERROR! MUST USE \":=\" TO MAKE AN ASSINGMENT. Failed in parser::assingmentStatmentRule" << std::endl;
			std::cout << "SYNTAX ERROR! MUST USE \":=\" TO MAKE AN ASSINGMENT. " + currToken << std::endl;
			return -1;
		}
		else {
			//now check for an expression.
			getNextToken();
			if (expressionRule() != 0) {
				error << "SYNTAX ERROR! INVALID EXRESSION USED AS AN ASSINGMENT. Failed in assingmentStatmentRule" << std::endl;
				return -1;
			}
		}
	}
	return 0;
}

/*
<variable>				::=	<variable identifier>   <indexed var>
<variable identifier>	::=	<identifier>
<indexed var>			::=	[   <expression>   <array idx>   |	 <empty-string>
*/
int parser::variableRule() {
	//at this point currToken is supposed to varName.
	if (!sc.isId(currToken)) {
		error << "SYNTAX ERROR! VARIABLE CALL MUST HAVE A VALID IDENTIFIER. Failed in parser::variableRule" << std::endl;
		std::cout << "SYNTAX ERROR! UNKNOWN IDENTIFIER." << std::endl;
		return -1;
	}
	else {
		//now check for indexed var. This is optional
		if (nextToken == "[") {
			getNextToken();
			if (indexedVarRule() != 0) {
				error << "SYNTAX ERROR! INVALID ARRAY VAR. Failed in arser::variableRule()" << std::endl;
				std::cout << "SYNTAX ERROR!INVALID ARRAY VAR." << std::endl;
			}
			//now check for an indexed var
		}
		else {
			return 0;
		}
	}
	return 0;
}

/*
<indexed var>	::=	[   <expression>   <array idx>   | <empty-string>
*/
int parser::indexedVarRule() {
	//if we call this then we already know that currToken is "[" we don't need to check for
	//empty string
	getNextToken();
	if (expressionRule() != 0) {
		error << "SYNTAX ERROR! MALFORMED ARRAY REFERENCE. Failed in parser::indexedVarRule" << std::endl;
		return -1;
	}
	else {
		//now check for array index
		if (arrayIdxRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED ARRAY REFERENCE. Failed in parser::indexedVarRule" << std::endl;
			return -1;
		}
	} 
	return 0;
}

/*
<array idx>	::=	,   <expression>   <array idx>   |	 ]
*/
int parser::arrayIdxRule() {
	//if currToken is "]" then return 0
	getNextToken();
	if (currToken == "]") {
		getNextToken();
		return 0;
	}
	else {
		//now check for ","
		if (currToken != ",") {
			error << "SYNTAX ERROR! INVALID ARRAY INDEX FOUND. Failed in arser::arrayIdxRule" << std::endl;
			std::cout << "SYNTAX ERROR! INVALID ARRAY INDEX FOUND. " + currToken << std::endl;
			return -1;
		}
		else {

		}
	}
	return 0;
}

/*
<expression>	::=	<simple expression>   <rel exp>
*/
int parser::expressionRule() {
	//first check for simple expression.
	if (simpleExpressionRule() != 0) {
		error << "SYNTAX ERROR! MALFORMED EXPRESSION. Failed in parser::expressionRule" << std::endl;
		return -1;
	}
	return 0;
}

/*
<simple expression>	::=	<sign>   <term>   <add term>
<sign>				::=	+   |   -   |   <empty-string>
*/
int parser::simpleExpressionRule() {
	//in this step I can check for a sign or a term by looking at currToken;
	if (currToken == "+" || currToken == "-") {
		getNextToken();
	}
	//check for a term
	if (termRule() != 0) {
		error << "SYTNAX ERROR! MALFORMED TERM. Failed in parser::simpleExpressionRule" << std::endl;
		return -1;
	}
	else {
		//check for an add term
	}
	return 0;
}

/*
<term>	::=	<factor>   <mul factor>
*/
int parser::termRule() {
	//check for a factor. No need to get nextToken yet.
	if (factorRule() != 0) {
		error << "SYNTAX ERROR! MALFORMED FACTOR. Failed in parser::termRule()" << std::endl;
		return -1;
	}
	else {
		//check for a mul factor. It may not be here. Use nextToken.
	}
	return 0;
}

/*
<factor>	::=	<variable>   |	<constant>   |
(   <expression>   )   |	not   <factor>
*/
int parser::factorRule() {
	//first check for a constant. If it's a string const then currtoken will be '"'.
 	if (currToken == "\"") {
		//its a may be a string const
		getNextToken();
		//test for string const. To test check if the nextToken is '"'
		if (nextToken != "\"") {
			error << "SYNTAX ERROR! MALFORMED STRING CONST. Failed in parser::factorRule" << std::endl;
			std::cout << "STRING CONSTATNS BUT START AND END WITH \"." << std::endl;
			return -1;
		}
		else {
			return 0; //it is a well formed string const.
		}
	}
	if (sc.isIntConst(currToken)) {
		//if this is true then we have an int const
		return 0;
	}
	//we still need to check for ( expression ) | NOT factor.
	return 0;
}

/*
<mul factor>	::=	<mul op>   <factor>   <mul factor>   |	 <empty-string>
*/
int parser::mulFactorRule() {
	return 0;
}

/*
<mul op>	::=	*   |   /   |   and
*/
int mulOpRule() {
	return 0;
}

/*
<rel exp>	::=	<rel op>   <simple expression>   |	 <empty-string>
<rel op>	::=	=   | <>   | <   | <=   | >=   | >
*/
int parser::relExpressionRule() {
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