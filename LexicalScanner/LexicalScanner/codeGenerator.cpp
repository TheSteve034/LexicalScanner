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
this method returns the assembly code needed to add an uninitlaized string var.
*/
std::string codeGen::addSringVar(std::string varName) {
	std::string line = "\t_" + varName + "\t\tresd\t127\t;a string";
	return line;
}

/*
This method returns the assembly needed to add a string to the initilized data section.
*/
std::string codeGen::addIStringVar(std::string varName, int varNum) {
	std::string line = "\t_" + std::to_string(varNum) + "_string\t\tdb\t\"" + varName + "\",0\t;this is a string var that is not decalred in the var block";
	return line;
}

std::string codeGen::getExitHeader() {
	return exitHeader;
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
	else {
		line = "\n\tmov\tedi, DWORD[_" + rVal + "] " + "\t\t;move rval into edi";
		line += "\n\tmov\tDWORD[_" + lVal + "], edi " + "\t\t;assign rval to lval";
	}
	return line;
}

/*
This method returns the assembly code needed to handle a call to write. Handles the following cases
1 - write is called with a already defined string var
2 - write is called with an inline declared string we know this is the case if our var param = "NONE"
in this case we use the value of compGenVarCount to determine which var to use from out iVar data set.
3 - write is called with an int var
4 - write is called with an inline int
*/
std::string codeGen::writeCode(std::string var, int numFlag, int varNumFlag) {
	//case 2
	std::string line = "";
	if (var != "NONE" && numFlag == 0) {
		line = "\n\tpush\t_" + var + "\t;adding var to be printed onto stack";
		line += "\n\tpush\tstringPrinter\t;add format string to stack";
		line += "\n\tcall\t_printf\t;print to console";
		line += "\n\tadd\tesp, 0x88\t;clear the stack";
		return line;
	}
	//case 1
	else if(numFlag == 0) {
		line = "\n\tpush\t_" + std::to_string(compGenVarCount)+"_string" + "\t;adding var to be printed onto stack";
		line += "\n\tpush\tstringPrinter\t;add format string to stack";
		line += "\n\tcall\t_printf\t;print to console";
		line += "\n\tadd\tesp, 0x88\t;clear the stack";
		compGenVarCount++;
		return line;
	}
	//case 3
	if (numFlag != 0 && varNumFlag == 0) {
		line = "\n\tpush\t_" + var + "\t;adding var to be printed onto stack";
		line += "\n\tpush\tnumberPrinter\t;add format string to stack";
		line += "\n\tcall\t_printf\t;print to console";
		line += "\n\tadd\tesp, 0x88\t;clear the stack";
		return line;
	}
	//case 4
	if (numFlag != 0 && varNumFlag != 0) {
		line = "\n\tpush\t" + var + "\t;adding var to be printed onto stack";
		line += "\n\tpush\tnumberPrinter\t;add format string to stack";
		line += "\n\tcall\t_printf\t;print to console";
		line += "\n\tadd\tesp, 0x88\t;clear the stack";
		return line;
	}
}
