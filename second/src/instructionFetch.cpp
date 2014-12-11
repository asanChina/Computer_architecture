#include "instructionFetch.h"
#include <iostream>
using namespace std;

void instructionFetch()
{
	if(PC >= dataStartAddress)
		return;

	int index = (PC - programStartAddress)/4;

	exitIfLess(index, 0);

	//instructionQueue.push_back(instructionEntry(index));
	instructionEntry tmp(index);

	int size = branchTargetBuffer.size();
	for(int i = 0; i < size; i++)
	{
		if(branchTargetBuffer[i].address == PC)
		{
			PC = (branchTargetBuffer[i].prediction == 1?branchTargetBuffer[i].targetAddress:(PC+4));
			tmp.prediction = branchTargetBuffer[i].prediction;	
/*
			if(textCommands[index].kind == 4)
				cout << "in instructionFetch(), BEQ:" << tmp.prediction << "cycle:" << cycle << endl;
*/
			instructionQueue.push_back(tmp);
			return;
		}
	}

	//if this command is a Jump or branch command, and we haven't found a match in btb, then we must insert it into btb 
	switch(textCommands[index].kind)
	{
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			{
				btb tmp(PC, textCommands[index].jumpAddress, -1);
				branchTargetBuffer.insert(branchTargetBuffer.begin(), tmp);
				tmp.prediction = -1;
				//need to delete least recently used entry if branch target buffer holds more than 16 entries
				if(branchTargetBuffer.size() > 16)
					branchTargetBuffer.pop_back();
			}
			break;
	}
	instructionQueue.push_back(tmp);
	PC += 4;
	return;
}

