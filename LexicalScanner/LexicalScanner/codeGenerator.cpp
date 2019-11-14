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
	std::string line = "\t_" + varName + "\t\tresb\t128\t;a string";
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
		line = "\n\tmov\tedi, _" + lVal +"\t\t;move lval address into edi" + "\n\tmov\tDWORD[edi]," + rVal + "\t\t;assing the value of rVal to edi";
	}
	else {
		line = "\n\tmov\tedi, _" + rVal + " " + "\t\t;move rval into edi";
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
std::string codeGen::writeCode(std::string var, int numFlag, int varNumFlag, std::string procName, int compCount, bool inProc, std::string currToken) {
	//case 2
	std::string line = "";
	if (var != "NONE" && numFlag == 0) {
		line = "\n\tpush\t_" + var + "\t;adding var to be printed onto stack";
		line += "\n\tpush\tstringPrinter\t;add format string to stack";
		line += "\n\tcall\t_printf\t;print to console";
		line += "\n\tadd\tesp, 0x8\t;clear the stack";
		return line;
	}
	//case 1
	else if(numFlag == 0) {
		//need to create a var for this inline string
		if (inProc == true) {
			//this is the case where we are adding an inline var from a proc
			iVar.push_back("\t_" + procName + "_" + std::to_string(compCount) + "_string\t\tdb\t\"" + currToken + "\",0\t;this is a string var that is not decalred in the var block");
			line = "\n\tpush\t_" + procName + "_" + std::to_string(compCount) + "_string\t;adding var to be printed onto stack";
		}
		else {
			//this is the case where we are using an inline var from main
			iVar.push_back("\t_" + std::to_string(compGenVarCount) + "_string" + "_string\t\tdb\t\"" + currToken + "\",0\t;this is a string var that is not decalred in the var block");
			line = "\n\tpush\t_" + std::to_string(compGenVarCount) + "_string" + "\t;adding var to be printed onto stack";
		}
		line += "\n\tpush\tstringPrinter\t;add format string to stack";
		line += "\n\tcall\t_printf\t;print to console";
		line += "\n\tadd\tesp, 0x8\t;clear the stack";
		compGenVarCount++;
		return line;
	}
	//case 3
	if (numFlag != 0 && varNumFlag == 0) {
		line = "\n\tpush\tDWORD[_" + var + "]\t;adding var to be printed onto stack";
		line += "\n\tpush\tnumberPrinter\t;add format string to stack";
		line += "\n\tcall\t_printf\t;print to console";
		line += "\n\tadd\tesp, 0x8\t;clear the stack";
		return line;
	}
	//case 4
	if (numFlag != 0 && varNumFlag != 0) {
		line = "\n\tpush\t" + var + "\t;adding var to be printed onto stack";
		line += "\n\tpush\tnumberPrinter\t;add format string to stack";
		line += "\n\tcall\t_printf\t;print to console";
		line += "\n\tadd\tesp, 0x8\t;clear the stack";
		return line;
	}
}

/*
This method returns the assembly code needed to handle a call to read. Handles the following cases
1 - read is called with an int var
2 - read is called with a string var
read may not be called with a constant
*/
std::string codeGen::readCode(std::string var, std::string type) {
	std::string line;
	//case 1
	if (type == "INT") {
		line = "\n\tpush\tDWORD[_" + var + "]\t;adding var to be read into to the stack";
		line += "\n\tpush\tnumberReader\t;add format string to stack";
		line += "\n\tcall\t_scanf\t;call to scanf";
		line += "\n\tadd\tesp, 0x8\t;clear the stack";
	}
	else if (type == "STRING") {
		//case 2
		line = "\n\tpush\t_" + var + "\t;adding var to be read into to the stack";
		line += "\n\tpush\tstringReader\t;add format string to stack";
		line += "\n\tcall\t_scanf\t;call to scanf";
		line += "\n\tadd\tesp, 0x8\t;clear the stack";
	}
	return line;
}

/*
This method is used to genereate lables when a process is encountered.
*/
std::string codeGen::generateLable(std::string labelName) {
	std::string line = "_"+labelName + ":";
	return line;
}
