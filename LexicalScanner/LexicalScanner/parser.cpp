#include "parser.h"

void parser::getTokenList(std::vector<std::string> scanedTokens) {
	tokens = scanedTokens;
	return;
}

void parser::resetSymVars() {
	aStart = 0;
	aEnd = 0;
	aSize = 0;
	symIds.clear();
	storageType = "";
	baseType = "";
	passType = "value";
	st.setArraySize(0);
}

void parser::resetLandRVars() {
	lValue = "";
	rValue = "";
}

void parser::getNextToken() {
	if (tokens[tokenCount] == "_EOF") {
		currToken = tokens[tokenCount];
		tokenCount++;
		return;
	}
	if (tokenCount == 0) {
		currToken = sc.toCaps(tokens[tokenCount]);
		tokenCount++;
		nextToken = sc.toCaps(tokens[tokenCount]);
		tokenCount++;
		return;
	}
	if(tokens[tokenCount] != "_EOL" && tokens[tokenCount+1] != "EOL"){
		currToken = nextToken;
		nextToken = sc.toCaps(tokens[tokenCount]);
		tokenCount++;
		return;
	}
}

/*
This method checks the types of lValue and rValue to make sure the types match
currently does not handle array assingments.
*/
int parser::isValidAssingmnet() {
	//check to see if rValue is a constant can be either an int or a string
	//if not then get the type information for the rValue from the sym table
	//test types and return approriately
	symTable::symbolInfo* lVar = st.getSym(lValue, procCount);
	//if lvar is an array return, not currently implemented
	if (lVar != NULL && lVar->storageType == "ARRAY") {
		return 0;
	}
	//test rval to see if it is an int const
	if (sc.isIntConst(rValue) && lVar->storageType == "INT") {
		if (lVar != NULL && (lVar->storageType == "INT") && (lVar->baseType == "INT")) {
			return 0;
		}
		else {
			return -1;
		}
	}
	//test for string const
	if (rValue == "\"" && lVar->storageType == "STRING") {
		//this is the case where we are assigning an inline string to an already delcared var
		return 0;
	}

	//test for Boolean assingment being made with TRUE or FALSE
	if (lVar != NULL && lVar->storageType == "BOOLEAN") {
		return 0;
	}

	symTable::symbolInfo* rVar = st.getSym(rValue, procCount);
	if (rVar != NULL && ((lVar->storageType == rVar->storageType) && (lVar->baseType == rVar->baseType))) {
		return 0;
	}
	else {
		return -1;
	}
}

/*
This method creates the assembly file and error file. The assembly file will have the following headers added
1 - export section
2 - import section
3 - initialized data section
4 - un initialized data section
The file will then be ready for vars to be declared.
*/
void parser::createFiles(std::string progName) {
	std::string asemFile = progName + ".asm";
	std::string errFile = progName + ".err";
 	assembly.open(asemFile, std::ios::out | std::ios::trunc);
	error.open(errFile, std::ios::out | std::ios::trunc);
	//add header data to .asm file. The cg object is the object that generats code
	assembly << cg.getExportHeader() << std::endl;
	assembly << cg.getImportHeader() << std::endl;
	assembly << cg.getSubRoutineCode() << std::endl;
	assembly << "\nJMP\t_main" << std::endl;
	//assembly << cg.getiDataHeader() << std::endl;
	//int compGenVars = 0;
	//check for any string constants declared in the begin blocks
	//for (int i = 0; i < tokens.size(); i++) {
	//	if (tokens[i] == "\"") {
	//		std::string var = tokens[i + 1];
	//		i = i + 2;
	//		//add string var to idata set
	//		assembly << cg.addIStringVar(var, compGenVars) << std::endl;
	//		compGenVars++;
	//	}
	//}
	//assembly << "\n" + cg.getuDataHeader() << std::endl;
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
			//add the program name to the symtable
			symTable::symbolInfo *var = new symTable::symbolInfo();
			var->name = nextToken;
			var->storageType = "program";
			var->baseType = "string";
			var->scope = st.getLevel();
			st.insertSymbol(var);
			//st.printInsertedVar(var);
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
	if (currToken == "_EOF") {
		currToken = nextToken;
	}
	if (currToken != ".") {
		error << "SYNTAX ERROR! PROGRAM MUST END WITH \".\"" << std::endl;
		std::cout << "SYNTAX ERROR! PROGRAM MUST END WITH \".\" " + currToken << std::endl;
	}
	else {
		std::cout << "Program parsed successfuly" << std::endl;
		assembly << cg.getExitHeader() << std::endl;
		//add the .data section to my asm file
		assembly << "\n" << std::endl;
		assembly << cg.getiDataHeader() << std::endl;
		for (auto& var : cg.iVar) {
			assembly << var << std::endl;
		}
		//add .bss section to asm file
		assembly << "\n" << std::endl;
		assembly << cg.getuDataHeader() << std::endl;
		cg.uVar.push_back(cg.addSringVar("source_ptr"));
		cg.uVar.push_back(cg.addSringVar("dest_ptr"));
		for (auto& var : cg.uVar) {
			assembly << var << std::endl;
		}
		return 0;
	}
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
			if (procDeclPartRule() != 0) {
				error << "SYNTAX ERROR! MALFORMED PROCEDURE DECL." << std::endl;
				return -1;
			}
			else {
				if (currToken != ";") {
					error << "SYNTAX ERROR! MISSING \";\" AT END OF PROCEDURE DECLRATION. Failed in parser::blockRule";
					return -1;
				}
				else {
					getNextToken();
				}
			}
		}
 		if (currToken == "BEGIN" ) {
			//at this point we need to add the main header to the asm file if we are not in a procedure.
			if (inProc == false) {
				assembly << "\n" << std::endl;
				assembly << cg.getCodeDataHeader() << std::endl;
				assembly << "_main:" << std::endl;
			}
			if (statmentPartRule() != 0) {
				error << "SYNTAX ERROR! MALFORMED STATMENT. Failed in parser::blockRule" << std::endl;
				return -1;
			}
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
		//add the var to the symbol table. We need to collect all the infomration we can here and then wait for all the type information to be returned.
		symIds.push_back(currToken);
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
	if (storageType != "ARRAY") {
		if (st.insertSimpleSyms(symIds, storageType, baseType, passType) != 0) {
			error << "SYNTAX ERROR! MULTIPLY DEFINED SYMBOL. Failed in parser::varDecRule" << std::endl;
			std::cout << "SYNTAX ERROR! MULTIPLY DEFINED SYMBOL. ";
			for (auto& sym : symIds) {
			std::cout << sym + " ";
			}
			std::cout << std::endl;
			return -1;
		}
		//add  var to asm file
		if (storageType == "INT") {
			for (auto& name : symIds) {
				cg.uVar.push_back(cg.addIntVar(name));
			}
		}
		if (storageType == "STRING") {
			for (auto& name : symIds) {
				cg.uVar.push_back(cg.addSringVar(name));
			}
		}
		resetSymVars();
	}
	//need to handle arrays here
	if (currToken == "ARRAY") {
		getNextToken();
		arrayDefRule();
		//insert the array var to sym table
		st.insertSimpleSyms(symIds, storageType, baseType, passType);
		resetSymVars();
		//afte every insert clean out the global vars.
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
			storageType = currToken;
			baseType = currToken;
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

//int parser::moreVarsRule() {
//	if (!sc.isId(currToken)) {
//		error << "SYNTAX ERROR! INVALID IDENTIFIER. Failed in parser::moreVarsRule()" << std::endl;
//		std::cout << "SYNTAX ERROR! INVALID IDENTIFIER IN A VAR BLOCK : " + currToken << std::endl;
//		return -1;
//	}
//	else {
//		getNextToken();
//		if (moreDeclsRule() != 0) {
//			error << "SYNTAX ERROR! INVALID VARIABLE DECLARATION. Failed in parser::moreVarsRule." << std::endl;
//			return -1;
//		}
//	}
//}
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
					baseType = currToken;
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
			//capture array starting index
			aStart = std::stoi(currToken);
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
					//capture end inex valule and calculate it's value;
					aEnd = std::stoi(currToken);
					aSize = aSize + st.calucalteIndexRange(aStart, aEnd);
					st.setArraySize(aSize);
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
			aStart = std::stoi(currToken);
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
					aEnd = std::stoi(currToken);
					aSize = aSize + st.calucalteIndexRange(aStart, aEnd);
					st.setArraySize(aSize);
				}
				else {
					//it wasn't a signed int const. Now check to see if it is an int const.
					if (!sc.isIntConst(currToken)) {
						error << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. Failed in parser::indexRangeRule" << std::endl;
						std::cout << "SYNTAX ERROR! ONLY INT CONSTANTS ARE ALLOWED AS INDEX RANGE VALULES. " + currToken << std::endl;
						return -1;
					}
					aEnd = std::stoi(currToken);
					aSize = aSize + st.calucalteIndexRange(aStart, aEnd);
					st.setArraySize(aSize);
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
	if (currToken != ";") {
		error << "SYNTAX ERROR! PROCEDURE DECL MUST END WITH \";\"" << std::endl;
		return -1;
	}
	//now check for more procedures
	if (nextToken == "PROCEDURE") {
		getNextToken();
		if (procDeclPartRule() != 0) {
			error << "SYNTAX ERROR. MALFORMED PROC DECL PART. Failed in procDeclPartRule" << std::endl;
		}
	}
	//getNextToken();
	return 0;
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
		//create proc lable in the assembly
		inProc = true;
		procCount = ++procCount;
		procName = currToken;
		assembly << "\n" + cg.generateLable(currToken) << std::endl;
		//capture the ID of the proc in preperation for adding it to the sym table 
		symIds.push_back(currToken);
		//now add proc ID to the sym table.
		st.setLevel();
		st.insertProc(symIds);
		resetSymVars();
		//now check for a "("
		getNextToken();
		if (currToken != "(") {
			error << "SYNTAX ERROR! MISSING THE \"(\" TOKEN WHEN CREATING AN PARAMETER LIST. Failed in parser::procDeclRule" << std::endl;
			std::cout << "SYNTAX ERROR! MISSING THE \"(\" TOKEN WHEN CREATING AN PARAMETER LIST." << std::endl;
			return -1;
		}
		else {
			//now look for a paramter list.
			if (paramListsRule() != 0) {
				error << "SYNTAX ERROR! MALFORMED PARAM LIST. Failed in parser::procDeclRule()" << std::endl;
				std::cout << "SYNTAX ERROR! MALFORMED PARAM LIST. " << std::endl;
				return -1;
			}
			else {
				//check for ";"
				//need to get nextToken()
				//then make the check
				getNextToken();
				if (currToken != ";") {
					error << "SYNTAX ERROR! MISSING \";\" IN A PROC DECL. Failed in parser::procDeclRule" << std::endl;
					std::cout << "SYNTAX ERROR! MISSING \";\" IN A PROC DECL." << std::endl;
					return -1;
				}
				//check for a block next
				getNextToken();
				if (blockRule() != 0) {
					error << "SYNTAX ERROR! MAILFORMED BLOCK IN A PROC DECL. Failed in parser::procDeclRule" << std::endl;
					std::cout << "SYNTAX ERROR! MAILFORMED BLOCK IN A PROC DECL." << std::endl;
					return -1;
				}
				else {
					return 0;
				}
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
	if (currToken == ")") {
		return 0;
	}
	if (currToken != "INT" && currToken != "BOOLEAN" && currToken != "STRING") {
		error << "SYNTAX ERROR! EACH PARAMETER IN A PARMETER LIST MUST HAVE A VALID TYPE. Failed in parser::paramListsRule" << std::endl;
		std::cout << "SYNTAX ERROR!EACH PARAMETER IN A PARMETER LIST MUST HAVE A VALID TYPE. " + currToken << std::endl;
		return -1;
	}
	else {
		//any parameter is a scope level lower than the proc it's self.
		//st.setLevel();
		//we can also set the storage type and base type.
		storageType = currToken;
		baseType = currToken;
		getNextToken();
		//now call paramPassingRule()
		if (paramPassingRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED PARAM PASSING. Failed in parser::paramListsRule" << std::endl;
		}
		else {
			return 0;
		}
	}
}

/*
<param passing>	::=	<pass by value>   |	*   <pass by reference>
*/
int parser::paramPassingRule() {
	if (currToken == "*") {
		passType = "Reference"; 
		getNextToken();
		if (passByReference() != 0) {
			error << "SYNTAX ERROR! INVALID PARAM. Failed in parser::paramPassingRule" << std::endl;
			return -1;
		}
	}
	else {
		if (passByValue() != 0) {
			error << "SYNTAX ERROR! INVALID PARAM. Failed in parser::paramPassingRule" << std::endl;
			return -1;
		}
	}
	return 0;
}

/*
<pass by value>	::=	<identifier>   <more params>
*/
int parser::passByValue() {
	if(!sc.isId(currToken)) {
		error << "SYNTAX ERROR! INVALID IDENTIFIER. Failed in parser::passByValue" << std::endl;
		return -1;
	}
	else {
		symIds.push_back(currToken);
		st.insertSimpleSyms(symIds, storageType, baseType, passType);
		resetSymVars();
		getNextToken();
		if (moreParamsRule() != 0) {
			error << "SYNTAX ERROR! INVALID VAR IN PARAM LIST. Failed in parser::passByValue" << std::endl;
			std::cout << "SYNTAX ERROR. INVALID VARIABLE. " + currToken << std::endl;
			return -1;
		}
	}
	return 0;
}

/*
<pass by reference>	::=	<identifier>   <more params>
*/
int parser::passByReference() {
	//we are looking at what should be the var right now
	if (variableRule() != 0) {
		error << "SYNTAX ERROR! INVALID VAR IN PARAM LIST. Failed in parser::passByReference" << std::endl;
		std::cout << "SYNTAX ERROR. INVALID VARIABLE. " + currToken << std::endl;
		return -1;
	}
	else {
		symIds.push_back(currToken);
		st.insertSimpleSyms(symIds, storageType, baseType, passType);
		resetSymVars();
		getNextToken();
		if (moreParamsRule() != 0) {
			error << "SYNTAX ERROR! INVALID VAR IN PARAM LIST. Failed in parser::passByReference" << std::endl;
			std::cout << "SYNTAX ERROR. INVALID VARIABLE. " + currToken << std::endl;
			return -1;
		}
	}
	return 0;
}

/*
<more params>	::=	,   <type identifier>   <param passing>   |	  )
*/
int parser::moreParamsRule() {
	if (currToken == ")") {
		return 0;
	}
	if (currToken != ",") {
		error << "SYNTAX ERROR! EACH PARAM NEEDS TO BE SEPERATED BY \",\". Failed in parser::moreParamsRule." << std::endl;
		std::cout << "SYNTAX ERROR! EACH PARAM NEEDS TO BE SEPERATED BY \",\". " + currToken << std::endl;
		return -1;
	}
	else {
		getNextToken();
		if (currToken != "INT" && currToken != "BOOLEAN" && currToken != "STRING") {
			error << "SYNTAX ERROR! EACH PARAMETER IN A PARMETER LIST MUST HAVE A VALID TYPE. Failed in parser::paramListsRule" << std::endl;
			std::cout << "SYNTAX ERROR!EACH PARAMETER IN A PARMETER LIST MUST HAVE A VALID TYPE. " + currToken << std::endl;
			return -1;
		}
		else {
			storageType = currToken;
			baseType = currToken;
			getNextToken();
			if (paramPassingRule() != 0) {
				error << "SYNTAX ERROR! INVALID PARAM LIST. Failed in parser::moreParamsRule" << std::endl;
				return -1;
			}
		}
	}
	return 0;
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
			else {
				inProc = false;
				procCount = --procCount;
				st.moveLevelDown();
				getNextToken();
				return 0;
			}
		}
	}
}

/*
<more stmts>	::=	;   <statement>   <more stmts>   |	 <empty-string>
*/
int parser::moreStatmentsRule() {
	//first we check for a ";" if we find one then there are more statments
	//if not the then return 0;
	//need to getNetToken
	if (currToken == ";") {
		getNextToken();
		if (currToken == "END") {
			return 0;
		}
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
	else {
		//there are no more statments;
		return 0;
	}
}

/*
<statement>	::=	<simple statement>   |	  <structured statement>
*/
int parser::statmentRule() {
	//need to check for a structured statment first
	//these start with "IF", "CASE", "BEGIN" or "WHILE"
	if (currToken == "IF" || currToken == "WHILE" || currToken == "SWITCH" || currToken == "BEGIN") {
		//if I make it here then there are structure statments
		if (structuredStatmentRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED STRUCTURED STATMENT. Failed in parser::statmentRule()" << std::endl;
			return -1;
		}
	}
	else {
		if (simpleStatmentRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED SIMPLE STATMENT. Failed in parser::statmentRule" << std::endl;
			return -1;
		}
		else {
			return 0;
		}
	}
	return 0;
}

/*
<structured statement>	::=	<compound statement>   |	<if statement>   |
<case statement>   |	<while statement>
*/
int parser::structuredStatmentRule() {
	//currToken will be a non terminal as we can branch from here without
	//calling next token.
	if (currToken == "WHILE") {
		getNextToken();
		if (whileStatmentRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED WHILE STATMENT. Failed in parser::structuredStatmentRule()" << std::endl;
			return -1;
		}
		else {
			return 0;
		}
		
	}
	if (currToken == "IF") {
		getNextToken();
		if (ifStatmentRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED IF STATMENT. Failed in parser::structuredStatmentRule" << std::endl;
			return -1;
		}
		else {
			return 0;
		}

	}
	if (currToken == "SWITCH") {
		if (caseRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED CASE STATMENT. Failed in parser::structuredStatmentRule" << std::endl;
			return -1;
		}
		else {
			return 0;
		}
		
	}
	if (currToken == "BEGIN") {
		if (compoundStatmentRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED COMPOUND STATMENT. Failed parser::structuredStatmentRule" << std::endl;
			return -1;
		}
		else {
			return 0;
		}
	}
}

/*
<case statement>	::=	switch (   <variable identifier>   )   <case part>
*/
int parser::caseRule() {
	//currToken should be "SWITCH" at this point.
	getNextToken();
	if (currToken != "(") {
		error << "SYNTAX ERROR! MALFORMED SWITCH STATMENT. MISSING OPEN PAREN. Failed in parser::caseRule" << std::endl;
		std::cout << "SYNTAX ERROR! MALFORMED SWITCH STATMENT. MISSING OPEN PAREN. " + currToken << std::endl;
		return -1;
	}
	else {
		getNextToken();
		if (variableRule() != 0) {
			error << "SYNTAX ERROR! INVALID VARIABLE IN SWITCH STATMENT. Failed in parser::caseRule" << std::endl;
			std::cout << "SYNTAX ERROR! INVALID VARIABLE IN SWITCH STATMENT. " + currToken << std::endl;
			return -1;
		}
		else {
			getNextToken();
			if (currToken != ")") {
				error << "SYNTAX ERROR! MALFORMED SWITCH STATMENT. MISSING CLOSING PAREN. Failed in parser::caseRule" << std::endl;
				std::cout << "SYNTAX ERROR! MALFORMED SWITCH STATMENT. MISSING CLOSING PAREN. " + currToken << std::endl;
				return -1;
			}
			else {
				//test for case part.
				getNextToken();
				if (casePartRule() != 0) {
					error << "SYNTAX ERROR! MALFORMED CASE/DEFAULT STATMENT. Failed in parser::caseRule()" << std::endl;
					return -1;
				}
				else {
					return 0;
				}
			}
		}
	}
}

/*
<case part>	::=	case   <expression>   :   <compound statement>   <case part>   |
default :   <compound statement>
	// Each case will have a compound statement, even if there is
	// only one statement to perform
*/
int parser::casePartRule() {
	//first check for a deafualt case.
	//this way if there is only a default then we do less work. otherwise this will allow us to catch the default at the end of the
	//list of cases.
	if (currToken == "DEFAULT") {
		getNextToken();
		if (currToken != ":") {
			error << "SYNTAX ERROR! MISSING \":\" IN A DEFAULT STATMENT. Failed in parser::casePartRule" << std::endl;
			std::cout << "SYNTAX ERROR! MISSING \":\" IN A DEFAULT STATMENT. " + currToken << std::endl;
			return -1;
		}
		else {
			//now check for a compound statment.
			getNextToken();
			if (compoundStatmentRule() != 0) {
				error << "SYNTAX ERROR! INVALID COMPOUND STATMENT IN DEFAULT STATMENT. Failed in parser::casePartRule" << std::endl;
				std::cout << "SYNTAX ERROR! INVALID COMPOUND STATMENT IN DEFAULT STATMENT." << std::endl;
			}
			else {
				return 0;
			}
		}
	}
	if (currToken != "CASE") {
		error << "SYNTAX ERROR! EACH CASE IN A SWITCH STATMENT MUST START WITH \"case\". Failed in parser::casePartRule" << std::endl;
		std::cout << "SYNTAX ERROR! EACH CASE IN A SWITCH STATMENT MUST START WITH \"case\". " + currToken << std::endl;
		return -1;
	}
	else {
		getNextToken();
		//new test for an expresion.
		if (expressionRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED EXPRESSION IN A CASE STATMENT. Failed in parser::casePartRule()" << std::endl;
			return -1;
		}
		else {
			if (currToken != ":") {
				error << "SYNTAX ERROR! MISSING \":\" IN A CASE STATMENT. Failed in parser::casePartRule" << std::endl;
				std::cout << "SYNTAX ERROR! MISSING \":\" IN A CASE STATMENT. " + currToken << std::endl;
				return -1;
			}
			else {
				// now check for a compound statment.
				getNextToken();
				if (compoundStatmentRule() != 0) {
					error << "SYNTAX ERROR! INVALID COMPOUND STATMENT IN DEFAULT STATMENT. Failed in parser::casePartRule" << std::endl;
					std::cout << "SYNTAX ERROR! INVALID COMPOUND STATMENT IN DEFAULT STATMENT." << std::endl;
				}
				else {
					//now check for another case statment.
					if (casePartRule() != 0) {
						error << "SYNTAX ERROR! MALFORMED CASE/DEFAULT STATMENT. Failed in parser::caseRule()" << std::endl;
						return -1;
					}
					else {
						return 0;
					}
				}
			}
		}
	}
}

/*
<if statement>	::=	if   <expression>   then   <statement>   <else part>
*/
int parser::ifStatmentRule() {
	if (expressionRule() != 0) {
		error << "SYNTAX ERROR! MALFORMED IF STATMENT. Failed in parser::ifStatmentRule." << std::endl;
		return -1;
	}
	else {
		if (currToken != "THEN") {
			error << "SYNTAX ERROR! MISSING THEN IN IF STATMENT. Failed in parser::ifStatmentRule" << std::endl;
			std::cout << "SYNTAX ERROR! MISSING THEN IN IF STATMENT." << std::endl;
			return -1;
		}
		else {
			getNextToken();
			//now check for a statment
			if (statmentRule() != 0) {
				error << "SYNTAX ERROR! MALFORMED STATMENT. Failed in parser::ifStatmentRule" << std::endl;
				return -1;
			}
			else {
				if (elseRule() != 0) {
					error << "SYNTAX ERROR! MALFORMED ELSE STATMENT. Failed in parser::ifStatmentRule" << std::endl;
					std::cout << "SYNTAX ERROR! MALFORMED ELSE STATMENT." << std::endl;
					return -1;
				}
				else {
					return 0;
				}
			}
		}
	}

}

/*
<else part>	::=	else   <statement>   |	 <empty-string>
*/
int parser::elseRule() {

	if (currToken == "ELSE") {
		getNextToken();
		if (statmentRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED STATMENT SECTION. Failed in parser::elseRule" << std::endl;
			return -1;
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}

/*
<while statement>	::=	while   <expression>   do   < compound statement>
	// Note: this language uses "begin" and "end" to indicate blocks
	// of code rather than the curly brackets, '{' and '}', used in
	// other languages.
*/
int parser::whileStatmentRule() {
	//currToken will be pointed at the correct spot
	if (expressionRule() != 0) {
		error << "SYNTAX ERROR! INVALID EXPRESSION. Failed in parser::whileStatmentRule" << std::endl;
		return -1;
	}
	else {
		if (currToken != "DO") {
			error << "SYNTAX ERROR! MISSING \"DO\" TO COMPLETE DECLARING A WHILE LOOP. Failed in parser::whileStatmentRule" << std::endl;
			std::cout << "SYNTAX ERROR! WHILE LOOP MUST USE INCLUDE THE WORD \"DO\"." << std::endl;
			return -1;
		}
		else {
			//now check for the compound statment
			getNextToken();
			if (currToken != "BEGIN") {
				error << "SYNTAX ERROR! CONTENT OF WHILE LOOP MUST START WITH BGEIN. Failed in parser::whileStatmentRule()" << std::endl;
			}
			else{
				if (compoundStatmentRule() != 0) {
					error << "SYNTAX ERROR! WHILE STATMNENT MUST HAVE A WELL FORMED COMPOUND STATMENT. Failed in parser::whileStatmentRule()" << std::endl;
				}
				else {
					return 0;
				}
			}
		}
	}
}

/*
<simple statement>	::=	<assignment statement>   |	  <procedure call>   |
<read statement>   |	<write statement>
*/
int parser::simpleStatmentRule() {
	//this method will need to check for all the types of staments
	//now check for a write call
	if (currToken == "WRITE") {
		if (writeRule() != 0) {
			error << "SYNTAX ERROR! MALFORED WRITE CALL. Failed in parser::simpleStatmentRule." << std::endl;
			return -1;
		}
		else {
			return 0;
		}
	}
	if (currToken == "READ") {
		if (readRule() != 0) {
			error << "SYNTAX ERROR! MALFORED READ CALL. Failed in parser::simpleStatmentRule." << std::endl;
			return -1;
		}
		else {
			return 0;
		}
	}
	//assingment statment
	if (nextToken == ":=") {
		if (assingmentStatmentRule() != 0) {
			error << "MALFORMED SIMPLE STATMENT. Failed in parser::simpleStatmentRule" << std::endl;
			return -1;
		}
		//her we check if the assingment is valid.

	}
	else {
		if (procedureCallRule() != 0) {
			error << "MALFORMED PROC CALL. Failed in parser::simpleStatmentRule" << std::endl;
			return -1;
		}
	}
}

/*
<write statement>	::=	write   (   <expression>   )

	// Semantic note: For simplicity, all write statements will write
	// only one value, a string or an integer. Booleans will not be
	// written to the screen.
*/
int parser::writeRule() {
	getNextToken();
	if (currToken != "(") {
		error << "MALFORMED WRITE STATMENT. Failed in parser::writeRule" << std::endl;
		std::cout << "MALFORMED WRITE STATMENT. MISSING \"(\"." << std::endl;
		return -1;
	}
	else {
		getNextToken();
		if (expressionRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED EXPRESSION. Failed in parser::writeRule" << std::endl;
			return -1;
		}
		else {
			if (currToken != ")") {
				error << "SYNTAX ERROR! MISSING CLOSING PAREN. Failed in parser::writeRule" << std::endl;
				std::cout << "SYNTAX ERROR! MISSING CLOSING PAREN." << std::endl;
				return -1;
			}
			else {
				symTable::symbolInfo* lVar = st.getSym(lValue, procCount);
				if (lVar == NULL) {
					//this means we are dealing with some type of constant
					if (rValue == "\"") { //we are passing an inline string to write
						assembly << cg.writeCode("NONE", 0, 0,procName,compGenCount,inProc,tokens[tokenCount-4]) << std::endl;
						++compGenCount;
						resetLandRVars();
					}
					if (sc.isIntConst(rValue)) { //we are passing an inline int
						assembly << cg.writeCode(rValue, 1, 1,"NONE",-1, inProc, "PL") << std::endl;
						resetLandRVars();
					}
				}
				if (lVar != NULL && lVar->storageType == "STRING") {
					//we are passing a string var to write
					assembly << cg.writeCode(lValue, 0, 0,"NONE", -1, inProc, "PL") << std::endl;
					resetLandRVars();
				}
				if (lVar != NULL && lVar->storageType == "INT" && lVar != NULL) {
					//we are sending an int var so send the flags 1,0
					assembly << cg.writeCode(lValue, 1, 0,"NONE", -1, inProc, "PL") << std::endl;
					resetLandRVars();
				}
				getNextToken();
				return 0;
			}
		}
	}
}

/*
<read statement>	::=	read   (   <variable>   )
	// Semantic note: For simplicity, all read statements will read only
	// one value, a string or an integer. Booleans cannot be read from
	// the keyboard.
*/
int parser::readRule() {
	getNextToken();
	if (currToken != "(") {
		error << "MALFORMED READ STATMENT. Failed in parser::readRule" << std::endl;
		std::cout << "MALFORMED READ STATMENT. MISSING \"(\"." << std::endl;
		return -1;
	}
	else {
		getNextToken();
		if (variableRule() != 0) {
			error << "MALFORMED READ STATMENT. Failed in parser::readRule" << std::endl;
			std::cout << "INVALID VARIABLE. MISSING \"(\"." << std::endl;
			return -1;
		}
		else {
			getNextToken();
			if (currToken != ")") {
				error << "MALFORMED READ STATMENT. Failed in parser::readRule" << std::endl;
				std::cout << "MALFORMED READ STATMENT. MISSING \")\"." << std::endl;
				return -1;
			}
			else {
				//add asm code for the read.
				//first we need to check the type of the var
				symTable::symbolInfo* readVar = st.getSym(lValue, procCount);
				if (readVar != NULL && readVar->storageType == "INT") {
					//call read with an int var
					assembly << cg.readCode(lValue, "INT") << std::endl;
					resetLandRVars();
				}
				else if (readVar != NULL && readVar->storageType == "STRING") {
					//call read with a string var
					assembly << cg.readCode(lValue, "STRING") << std::endl;
					resetLandRVars();
				}
				getNextToken();
				return 0;
			}
		}
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
	//check types to make sure assignment is valid.
	if (assingmentDone == false) {

		if (isValidAssingmnet() != 0) {
			error << "SYNTAX ERROR! TYPE MISSMATCH OR SYMBOL UNKNOWN. Failed in assingmentStatmentRule" << std::endl;
			std::cout << "SYNTAX ERROR, TYPE MISSMATCH BETWEEN " + lValue + " AND " + rValue + " OR ONE OR BOTH VALUES ARE UNKNOWN" << std::endl;
			return -1;
		}
		else {
			symTable::symbolInfo* lVar = st.getSym(lValue, procCount);
			//pass lValue and rValue to cg object so it can add the code for the assingment.
			if (lVar->storageType == "INT" && sc.isIntConst(rValue)) { //this is here to restrict gode generation to int assingments only since that is all the has been implmented.
				assembly << cg.integerAssingment(lValue, rValue, 1) << std::endl;
				resetLandRVars();
			}
			else if (lVar->storageType == "INT") {
				assembly << cg.integerAssingment(lValue, rValue, 0) << std::endl;
				resetLandRVars();
			}
			if (lVar->storageType == "STRING") {
				assembly << cg.stringAssingment(lValue, rValue, 0) << std::endl;
				resetLandRVars();
			}
		}
	}
	if (assingmentDone == true && inAddOp == true) {
		assembly << "\tmov DWORD[_" + lValue + "], edi" << std::endl;
	}

	return 0;
}

/*
<procedure call>	::=	<procedure identifier>   (   <arg list>

	// Using the example procedures declared above we could have:
	//       MYPROCEDURE(bob[2,-1], "Merrily we go along");
	//       myprocedure(num1, string1);
	// However the following won't work:
	//       myProcedure(3, "So sorry, so sad...");
	// in this case the first argument is not a reference to a
	// memory location.
*/
int parser::procedureCallRule() {
 	if (!sc.isId(currToken)) {
		error << "SYNTAX ERROR! PROC CALL MUST START WITH A VARIABLE. Failed in parser::procedureCallRule" << std::endl;
		std::cout << "SYNTAX ERROR! PROC CALL MUST START WITH A VARIABLE. " + currToken << std::endl;
		return -1;
	}
	else {
		getNextToken();
		if (currToken != "(") {
			error << "SYNTAX ERROR! MISSING OPENING PAREN. Failed in parser::procedureCallRule" << std::endl;
			std::cout << "SYNTAX ERROR! MISSING OPENING PAREN." << std::endl;
			return -1;
		}
		else {
			getNextToken();
			if (argListRule() != 0) {
				error << "SYNTAX ERROR! MALFORMED ARG LIST IN A PROC CALL. Failed in parser::procedureCallRule" << std::endl;
				return -1;
			}
			else {
				return 0;
			}
		}
	}
}

/*
<arg list>	::=	<expression>   <more args>   |	 )
*/
int parser::argListRule() {
	if (currToken == ")") {
		getNextToken();
		return 0;
	}
	if (expressionRule() != 0) {
		error << "SYNTAX ERROR! MALFORMED STATMENT IN A PROC CALL. Failed in parser::argListRule" << std::endl;
		return -1;
	}
	else {
		if (moreArgListRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED ARG LIST IN A PORC CALL. Failed in parser::argListRule" << std::endl;
			return -1;
		}
		else {
			return 0;
		}
	} 
}

/*
<more args>	::=	,   <expression>   <more args>   |	 )
*/
int parser::moreArgListRule() {
	if (currToken == ")") {
		getNextToken();
		return 0;
	}
	if (currToken != ",") {
		error << "SYNTAX ERROR! MISSING \",\" IN A PROC ARG LIST. Failed in parser::moreArgListRule()";
		std::cout << "SYNTAX ERROR! MISSING \",\" IN A PROC ARG LIST." << std::endl;
		return -1;
	}
	else {
		getNextToken();
		if (expressionRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED STATMENT IN A PROC ARG LIST. Failed in parser::moreArgListRule" << std::endl;
			return -1;
		}
		else {
			if (moreArgListRule() != 0) {
				error << "SYNTAX ERROR! MALFORMED ARG LIST IN A PORC CALL. Failed in parser::moreArgListRule" << std::endl;
				return -1;
			}
		}
	}
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
		}
		else {
			//capture non-indexed var lValue;
			if (lValue == "") {
				lValue = currToken;
			}
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
	if (currToken == "]") {
		return 0;
	}
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
			getNextToken();
			if (expressionRule() != 0) {
				error << "SYNTAX ERROR! MALFORMED EXPRESSION. Failed in parser::arrayIdxRule" << std::endl;
				return -1;
			}
			else {
				if (arrayIdxRule() != 0) {
					error << "SYNTAX ERROR! MALFORMED EXPRESSION. Failed in parser::arrayIdxRule" << std::endl;
					return -1;
				}
			}
		}
	}
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
	else {
		//check for rel expression
		if (relExpressionRule() != 0) {
			//some error
			return -1;
		}
	}
return 0;
}

/*
<rel exp>	::=	<rel op>   <simple expression>   |	 <empty-string>
<rel op>	::=	=   | <>   | <   | <=   | >=   | >
*/
int parser::relExpressionRule() {
	if (currToken == "<>" || currToken == "<" || currToken == "<=" || currToken == ">=" || currToken == ">") {
		//there are rel ops
		//now check for a vlid expression
		getNextToken();
		if (simpleExpressionRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED REL OP. Failed in parser::relExpressionRule" << std::endl;
			return -1;
		}
	}
	else {
		return 0;
	}
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
	if(termRule() != 0) {
		error << "SYTNAX ERROR! MALFORMED TERM. Failed in parser::simpleExpressionRule" << std::endl;
		return -1;
	}
	else {
		//check for an add term
		getNextToken();
		if (addTermRule() != 0) {
			//then there is no add term.
			return 0;
		}
	}
	return 0;
}

/*
<add term>	::=	<add op>   <term>   <add term>   |	 <empty-string>
<add op>	::=	+   |   -   |   or
*/
int parser::addTermRule() {
	//if there is no add opp then there are no more terms;
	if (currToken != "-" && currToken != "+" && currToken != "OR") {
		return 0;
	}
	//now test for a term
	if (inAddOp == false) {
		addOppLval = tokens[tokenCount - 3];
		inAddOp = true;
	}
	//operation = currToken;
	getNextToken();
	if (termRule() != 0) {
		//some errors
		return -1;
	}
	else {
		if (mulFactor == false) {
			addOppRval = currToken;
			operation = tokens[tokenCount - 3];
			if (sc.isIntConst(addOppLval)) {
				assembly << "\tmov edi,\t" + addOppLval + "\t; move int const into edi" << std::endl;
				ediInuse = true;
			}
			else {
				assembly << "\tmov edi,\tDWORD[_" + addOppLval + "]\t; move int var into edi" << std::endl;
				ediInuse = true;
			}
			if (sc.isIntConst(addOppRval)) {
				if (operation == "+") {
					assembly << "\tadd edi,\t" + addOppRval + "\t;add const to edi" << std::endl;
					ediInuse = true;
				}
				else if (operation == "-") {
					assembly << "\tsub edi,\t" + addOppRval + "\t;subtract const to edi" << std::endl;
					ediInuse = true;
				}
			}
			else {
				if (operation == "+") {
					assembly << "\tadd edi,\tDWORD[_" + addOppRval + "]\t;subtract var from edi" << std::endl;
					ediInuse = true;
				}
				else if (operation == "-") {
					assembly << "\tsub edi,\tDWORD[_" + addOppRval + "]\t;subtract var from edi" << std::endl;
					ediInuse = true;
				}
			}
		}
		else if (mulFactor == true){
			operation = tokens[tokenCount - 5];
			if (operation == "+") {
				assembly << "\tadd edi, esi" << std::endl;
			}
			if (operation == "-") {
				assembly << "\tsub edi, esi" << std::endl;
			}
			assingmentDone = true;
		}
		getNextToken();
		if (addTermRule() != 0) {
			//some errorsd
			return -1;
		}
	}
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
		if (mulFactorRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED MUL FACTOR. Failed arser::termRule" << std::endl;
			return -1;
		}
		if (mulFactor == true && inAddOp == false) {
			/*assembly << "\tmov\tesi, \tDWORD[_" + currToken + "]\t; move op1 into dest register" << std::endl;
			assembly << "\timul\tedi,\tesi\t;op1 = op1 * op2" << std::endl;*/
			assembly << "\tmov\tDWORD[_" + lValue + "],\tedi" << std::endl;
			mulFactor = false;
			assingmentDone = true;
			resetLandRVars();
		}
	}
	//grab rValue;
	rValue = currToken;
	rValueP1 = nextToken;
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
			getNextToken();
			return 0; //it is a well formed string const.
		}
	}
	if (sc.isIntConst(currToken)) {
		//if this is true then we have an int const
		return 0;
	}
	//now check for currtoken for "("
	if (currToken == "(") {
		getNextToken();
		if (expressionRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED EXPRESSION. Failed in parser::factorRule" << std::endl;
			return -1;
		}
		else {
			if (currToken != ")") {
				error << "SYNTAX ERROR! MISSING CLOSING PAREN. Failed in parser::factorRule" << std::endl;
				std::cout << "SYNTAX ERROR! MISSING CLOSING PAREN." << std::endl;
				return -1;
			}
			else {
				return 0;
			}
		}
	}
	//now check for "NOT" expression
	if (currToken == "NOT") {
		getNextToken();
		if (factorRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED FACTOR. Failed in parser::factorRule" << std::endl;
			return -1;
		}
		else {
			return 0;
		}
	}
	//now check for variable
	if (variableRule() != 0) {
		error << "SYNTAX ERROR! INVALID VARIABLE. Failed in parser::factorRule" << std::endl;
		return -1;
	}
	return 0;
}

/*
<mul factor>	::=	<mul op>   <factor>   <mul factor>   |	 <empty-string>
<mul op>	::=	*   |   /   |   and
*/
int parser::mulFactorRule() {
	operation = nextToken;
	assembly << "\n" << std::endl;
	if (nextToken != "*" && nextToken != "/" && nextToken != "AND") {
		//there are no mul ops return 0;
		return 0;
	}
	else {
		mulFactor = true;
		if (ediInuse == false && inAddOp == false) {
			assembly << "\tmov\tedi, \tDWORD[_" + currToken + "]\t; move op1 into dest register" << std::endl;
		}
		else {
			if ((lastOperand != currToken) && inAddOp == false) {
				assembly << "\tmov\tesi, \tDWORD[_" + currToken + "]\t; move op1 into dest register" << std::endl;
			}
		}
		if (ediInuse == true && inAddOp == true) {
			if (sc.isIntConst(currToken)) {
				assembly << "\tmov esi,\t" + currToken << std::endl;
			}
			else {
				assembly << "\tmov\tesi, \tDWORD[_" + currToken + "]" << std::endl;
			}
		}

		getNextToken();
		getNextToken();
		if (factorRule() != 0) {
			error << "SYNTAX ERROR! MALFORMED FACTOR. Failed in parser::mulFactorRule" << std::endl;
			return -1;
		}
		else {
			if (operation == "*") {
				bool constVal = false;
				if (sc.isIntConst(currToken)) {
					constVal = true;
				}
				if (inAddOp == false) {
					if ((ediInuse == false) && (lastOperand != currToken) && (constVal == false)) {
						assembly << "\timul\tedi,\tDWORD[_" + currToken + "]\t;op1 = op1 * op2" << std::endl;
						ediInuse = true;
					}
					else {
						if ((lastOperand != currToken) && (ediInuse == false) && (constVal == false)) {
							assembly << "\timul\tedi,\tDWORD[_" + currToken + "]\t;op1 = op1 * op2" << std::endl;
						}
						else if ((lastOperand != currToken) && (ediInuse == true) && (constVal == false)) {
							assembly << "\timul\tedi,\tDWORD[_" + currToken + "]\t;op1 = op1 * op2" << std::endl;
						}
					}
					if ((ediInuse == false) && (lastOperand != currToken) && (constVal == true)) {
						assembly << "\timul\tedi,\t" + currToken + "\t;op1 = op1 * op2" << std::endl;
						ediInuse = true;
					}
					else {
						if ((lastOperand != currToken) && (ediInuse == false) && (constVal == true)) {
							assembly << "\timul\tedi,\t" + currToken + "\t;op1 = op1 * op2" << std::endl;
						}
						else if ((lastOperand != currToken) && (ediInuse == true) && (constVal == true)) {
							assembly << "\timul\tedi,\t" + currToken + "\t;op1 = op1 * op2" << std::endl;
						}
					}
				}
				else if (inAddOp == true) {
					if ((ediInuse == true) && (lastOperand != currToken) && (constVal == false)) {
						assembly << "\timul\tesi,\tDWORD[_" + currToken + "]\t;op1 = op1 * op2" << std::endl;
						ediInuse = true;
					}
					else {
						if ((lastOperand != currToken) && (ediInuse == true) && (constVal == false)) {
							assembly << "\timul\tesi,\tDWORD[_" + currToken + "]\t;op1 = op1 * op2" << std::endl;
						}
						else if ((lastOperand != currToken) && (ediInuse == true) && (constVal == false)) {
							assembly << "\timul\tesi,\tDWORD[_" + currToken + "]\t;op1 = op1 * op2" << std::endl;
						}
					}
					if ((ediInuse == true) && (lastOperand != currToken) && (constVal == true)) {
						assembly << "\timul\tesi,\t" + currToken + "\t;op1 = op1 * op2" << std::endl;
						ediInuse = true;
					}
					else {
						if ((lastOperand != currToken) && (ediInuse == true) && (constVal == true)) {
							assembly << "\timul\tesi,\t" + currToken + "\t;op1 = op1 * op2" << std::endl;
						}
						else if ((lastOperand != currToken) && (ediInuse == true) && (constVal == true)) {
							assembly << "\timul\tesi,\t" + currToken + "\t;op1 = op1 * op2" << std::endl;
						}
					}
				}
				lastOperand = currToken;
			}
			if (mulFactorRule() != 0) {
				error << "SYNTAX ERROR! MALFORMED MUL OP. Failed in parser::mulFactorRule" << std::endl;
				return -1;
			}
		}
	}
	//there are no more mulFactors now assign the value of EDI to lVar
	return 0;
}

void parser::printTable() {
	st.printSymTable();
}

int isValidAssingmnet() {
	//symTable::symbolInfo* var;
	return 0;
}

int parser::parseFile() {
	getNextToken();
	if (programRule() != 0) {
		error << "SYNTAX ERROR. Call to program rule failed in function parser::parseFile().";
		std::cout << "SYNTAX ERROR! CANNOT PARSE FILE." << std::endl;
		std::cout << "See error log for failure trace." << std::endl;
		return -1;
	}
	else {
		return 0;
	}
}