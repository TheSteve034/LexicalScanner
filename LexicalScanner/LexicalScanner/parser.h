#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "scanner.h"


class parser {
private:
	scanner sc = scanner();
	std::vector<std::string> tokens;
	int tokenCount = 0;
	std::string currToken = "";
	std::string nextToken = "";
	std::fstream assembly = {};
	std::fstream error = {};

	int programRule();
	int blockRule();
	int varDecRule();
	int moreDeclsRule();
	int moreVarsRule();
	int arrayDefRule();
	int indexRangeRule();
	int indexListRule();
	int procDeclPartRule();
	int procDeclRule();
	int paramListsRule();
	int paramPassingRule();
	int passByValue();
	int passByReference();
	int moreParamsRule();
	int statmentPartRule();
	int compoundStatmentRule();
	int statmentRule();
	int simpleStatmentRule();
	int assingmentStatmentRule();
	int variableRule();
	int expressionRule();
	int simpleExpressionRule();
	int relExpressionRule();
	int termRule();
	int factorRule();
	int mulFactorRule();
	int moreStatmentsRule();
	int indexedVarRule();
	int arrayIdxRule();
	int addTermRule();
	int structuredStatmentRule();
	int whileStatmentRule();
	int ifStatmentRule();
	int elseRule();
	int writeRule();
	int readRule();


public:
	//gathers the tokens from the scanner.
	void getTokenList(std::vector<std::string> scanedTokens);
	//sets the value of currToken and nexToken using the tokenCount number;
	void getNextToken();
	//creates .asm and .err files
	void createFiles(std::string progName);
	int parseFile();
};
