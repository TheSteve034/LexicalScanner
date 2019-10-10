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
void symTable::insertSymbol(struct symbolInfo) {

}