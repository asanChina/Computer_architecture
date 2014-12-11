#include "init.h"
#include <iostream>
using namespace std;


void init(char* inputfile)
{
	readBin(inputfile);
	disassemble();
	for(int i = 0; i < textCommands.size() - (dataStartAddress-programStartAddress)/4; i++)
		memory.push_back(0);
	return;	
}
