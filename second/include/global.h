#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <vector>
#include <string>

struct Cmd{
	//the address of the instruction/data, first instruction is 600
	int address;
	//I assigned a number to each MIPS command, but if it is data, then kind = 0
	int kind;
	//belowing three integers record corresponding values for related registers.
	// if the MIPS command only involve 2 registers, then "third" is left unused
	//if the MIPS command only involve 1 register, then "second" and "third" will be left unused 
	int first;
	int second;
	int third;
	bool firstReady;
	bool secondReady;
	bool thirdReady;

	int selfReadRegister1;
	int selfReadRegister2;
	int selfWriteRegister;
	int loadStoreAddress;
	int jumpAddress;

	Cmd(){
		kind = first = second = third = -1;
		selfReadRegister1 = selfReadRegister2 = selfWriteRegister = -1;
		firstReady = secondReady = thirdReady = false;
		loadStoreAddress = jumpAddress = -1;
	}
};

//programStartAddress will be set to 600
extern int programStartAddress;

//store the binary command, i.e. like "101010001000100001111000010001100"
extern std::vector<std::string> binaryCommands;

extern std::vector<Cmd> textCommands;

//dataStartAddress will be set when we transform binaryCommands to textCommands
extern int dataStartAddress;

struct instructionEntry{
	//"index" denote the position of the command in the textCommands
	int index;
	///"-1" not set, "0" not taken, "1" taken
	int prediction; 
	///"1" means after we execute, the real action is branch taken
	int realAction;
	instructionEntry(){
		prediction = realAction = -1;
	}
	instructionEntry(int idn)		
	{
		index = idn;
		prediction = realAction = -1;
	}
	instructionEntry(int i, int p)
	{
		index = i;
		prediction = p;
		realAction = -1;
	}
};

extern std::vector<instructionEntry> instructionQueue;

extern int PC;

struct Entry{
	Cmd command;
	instructionEntry ins;

	int previousReadRegister[32];
	int previousWriteRegister[32];
	int loadStoreCount;
	int id;
	bool hasRestoreWriteResult;
	bool hasUpdateBranchTargetBuffer;
	//"1" means has been issued,"2" means has been executed
	//"3" means has been "write result", "4" means has been commited
	int stage;
	//"true" means ready to be committed
	bool ready;
	Entry(){
		for(int i = 0; i < 32; i++)
			previousReadRegister[i] = previousWriteRegister[i] = 0;
		stage = 1;
		loadStoreCount = 0;
		ready = false;
		id = -1;
		hasUpdateBranchTargetBuffer = hasRestoreWriteResult = false;
	}
};

extern std::vector<Entry> reservationStation;
extern int RSslots;
extern int ROBslots;
extern std::vector<Entry> reorderBuffer;

struct btb{
	int address;
	int targetAddress;
	int prediction;
	btb(int a, int t)
	{
		address = a;
		targetAddress = t;
		prediction = -1;
	}
	btb(int a, int t, int  p){
		address = a;
		targetAddress = t;
		prediction = p;
	}
	btb(){
		prediction = -1;
	}
};

extern std::vector<btb> branchTargetBuffer;

extern int registers[32];
extern int tempRegisters[32]; //act as pipeline registers
extern std::vector<int> memory; //act as the memory, start address is 716


extern bool programEnd;
extern int cycle;
#endif
