#include "codeGenerator.h"

std::string codeGen::getExportHeader() {
	return exports;
}
std::string codeGen::getImportHeader() {
	return imports;
}
std::string codeGen::getiDataHeader() {
	return iDataHeader;
}
std::string codeGen::getuDataHeader() {
	return uDataHeader;
}
std::string codeGen::getCodeDataHeader() {
	return codeHeader;
}

/*
this method returns the assemby code needed to add an integer variable. This is used to add 
varaibles from a var section of the source code.
*/
std::string codeGen::addIntVar(std::string varName) {
	std::string line = "\t_" + varName + "\t\tresd\t1\t;an int";
	return line;
}

/*
This method returns the assembly code needed to handle an integer assingment. Handles the following cases
1 - lVal is a var and rVal is a var
2 - lVal is a var and rVal is a int const
*/
std::string codeGen::integerAssingment(std::string lVal, std::string rVal, int constFlag) {
	//the case where rVal is a constant
	std::string line;
	if (constFlag == 1) {
		line = "\n\tmov\tedi, _" + lVal + "\t\t;move lval into edi" + "\n\tmov\tedi," + rVal + "\t\t;assing the value of rVal to edi";
	}
	return line;
}
