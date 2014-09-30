#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <vector>
#include <string>

struct Cmd{
	//the address of the instruction/data
	int address;
	//1->command; 0->data
	bool instructionOrData;
	//I assigned a number to each required command
	int kind;
	int first;
	int second;
	int third;
	Cmd(){
		kind = first = second = third = -1;
	}
};

extern int programStartAddress;
extern std::vector<std::string> binaryCommands;
extern std::vector<Cmd> textCommands;
extern int dataStartAddress;

#endif
