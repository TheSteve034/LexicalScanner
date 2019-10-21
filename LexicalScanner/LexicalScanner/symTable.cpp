#include "symTable.h"

/*
This method finds the bucket that a symbol should be in.
*/
int symTable::generateHash(std::string name) {
	int idx = 0;
	for (int i = 0; name[i]; ++i) {
		idx = idx + name[i];
	}
	return (idx % MAXSYMBOLCOUNT);
}

/*
This method adds a symbol to it's appropirate bucket. If the bucket is empty then it jsut adds a

*/
void symTable::insertSymbol(symbolInfo* var) {
	int pos = generateHash(var->name);

	if (block[pos] == NULL) {
		block[pos] = new symbolInfo();
		block[pos]->name = var->name;
		block[pos]->storageType = var->storageType;
		block[pos]->baseType = var->baseType;
		block[pos]->scope = var->scope;
		block[pos]->arrayDimensions = var->arrayDimensions;
		block[pos]->passType = var->passType;
		block[pos]->next = NULL;
	}
	else {
		//here we need to check if there are mulDefs. Meaning that the same symbol is defined twice or more in the same scope. 
		symbolInfo* newNode = new symbolInfo();
		newNode->name = var->name;
		newNode->storageType = var->storageType;
		newNode->baseType = var->baseType;
		newNode->scope = var->scope;
		newNode->passType = var->passType;
		newNode->arrayDimensions = var->arrayDimensions;

		// pointer swap
		symbolInfo* nextNode = block[pos];
		block[pos] = newNode;
		newNode->next = nextNode;
	}
}

/*
this method allows us to search a given bucket for a symbol. We make sure that we match names, types, and scopes
*/
int symTable::searchForSymbol(std::string name, int scope, std::string sType, std::string type) {
	int chain = generateHash(name);

	symbolInfo* temp = block[chain];

	while (temp != NULL) {
		if (std::strcmp(temp->name.c_str(), name.c_str()) && std::strcmp(temp->storageType.c_str(), sType.c_str()) && std::strcmp(temp->baseType.c_str(), type.c_str()) && temp->scope == scope) {
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int symTable::deleteSymbol(std::string name, int scope, std::string sType, std::string type) {
	return 0;
}

int symTable::updateSymbol(std::string name, int scope, std::string sType, std::string type) {
	//int position = generateHash(name);

	//symbolInfo* temp = block[position];

	//while (temp != NULL) {
	//	if (std::strcmp(temp->name.c_str(), name.c_str()) && std::strcmp(temp->storageType.c_str(), sType.c_str()) && std::strcmp(temp->baseType.c_str(), type.c_str()) && temp->scope == scope) {
	//		return 0;
	//	}
	//	temp = temp->next;
	//}
	return 0;
}

int symTable::getLevel() {
	return level;
}

/*
we need to use the scope vector to make sure we move into the correct scope.  
*/
void symTable::setLevel() {	
	if (this->level != 0) {
		for (int i = 1; i < MAXSCOPECOUNT; i++) {
			if (scope[i] != false) {
				continue;
			}
			else {
				level = i;
				scope[i] = true;
				break;
			}
		}
		return;
	}
	if (this->level == 0 && scope[1] == false) {
		this->level = 1;
		scope[1] = true;
		return;
	}
	else {
		for (int i = 1; i < MAXSCOPECOUNT; i++) {
			if (scope[i] != false) {
				continue;
			}
			else {
				level = i;
				scope[i] = true;
				break;
			}
		}
	}
	return;
}

void symTable::setFirstProc(bool val) {
	firstProc = val;
}

bool symTable::getFirstProc() {
	return firstProc;
}

void symTable::moveLevelDown() {
	if (level > 0) {
		level = level - 1;
	}
}

/*
This method is used to insert the following var types:
INT STRING and BOOLEANS
this method can be used to to insert a list of vars with the same type.
*/
void symTable::insertSimpleSyms(std::vector<std::string> Ids, std::string sType, std::string bType, std::string passType) {
	if (Ids.empty()) {
		return;
	}
	for (auto& symbol : Ids) {
		struct symTable::symbolInfo* var = new symbolInfo();
		var->name = symbol;
		var->storageType = sType;
		var->baseType = bType;
		var->scope = level;
		var->passType = passType;
		var->arrayDimensions = arraySize;
		insertSymbol(var);
		printInsertedVar(var);
	}
}

/*
add a procedure definition to the sym table.
*/
void symTable::insertProc(std::vector<std::string> Ids) {
	if (Ids.empty()) {
		return;
	}
	struct symTable::symbolInfo* var = new symbolInfo();
	var->name = Ids[0];
	var->storageType = "Procedure";
	var->baseType = "Procedure";
	if (level - 1 == 0) {
		var->scope = 0;
	}
	else {
		var->scope = level - 1;
	}
	insertSymbol(var);
	printInsertedVar(var);
}

int symTable::calucalteIndexRange(int start, int end) {
	//we need to handle three cases here
	//1 - both end points are positive
	//2 - both end point are negative
	//3 - one end point is negative and the other is positive.
	//4 - case where one or both of the end points are 0.

	//case 4
	if (start == 0 && end == 0) {
		return 0;
	}
	if (start > 0 && end == 0) {
		return start;
	}
	if (start < 0 && end == 0) {
		return -1 * start;
	}
	if (start == 0 && end > 0) {
		return end;
	}
	if (start == 0 && end < 0) {
		return -1 * end;
	}
	//case 1
	if (start > 0 && end > 0) {
		return end - start;
	}
	//case 2
	if (start < 0 && end < 0) {
		start = start * -1;
		end = end * -1;
		return end - start;
	}
	//case 3
	if( start < 0 && end >= 0) {
 		return end + -1*start;
	}
	else if (start >= 0 && end < 0) {
		return start + -1*end;
	}
}

void symTable::setArraySize(int size) {
	arraySize = size;
}

void symTable::printInsertedVar(struct symbolInfo* var) {
	std::cout << "INSERTED SYMBOL" << std::endl;
	std::cout << "Name: " + var->name << std::endl;
	std::cout << "Base Type: " + var->baseType << std::endl;
	std::cout << "Storage Type: " +var->storageType << std::endl;
	std::cout << "Scope: " + std::to_string(var->scope) << std::endl;
	std::cout << "Array Dimensions: " + std::to_string(var->arrayDimensions) << std::endl;
	std::cout << "Pass type: " + var->passType << std::endl;
	std::cout << std::endl;
}