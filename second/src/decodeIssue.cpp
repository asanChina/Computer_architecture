#include "decodeIssue.h"

void decodeIssue()
{
	int robSize = reorderBuffer.size();
	int rsSize = reservationStation.size();

	if( robSize < ROBslots && rsSize < RSslots)
	{
		int size = instructionQueue.size();
		if(size == 0) 
			return;
		
		Entry tmp;
		tmp.ins= instructionQueue[0]; 
		int index = tmp.ins.index;
		if(rsSize == 0)
		{
			tmp.command = textCommands[index];
		}
		else
		{
			Entry last = reservationStation.back();
			tmp = last;
			tmp.ins = instructionQueue[0];
			tmp.loadStoreCount = 0;
			tmp.stage = 1;
			tmp.ready = false;
			tmp.command = textCommands[index];
			tmp.hasRestoreWriteResult = false;
			tmp.hasUpdateBranchTargetBuffer = false;
		
			int readRegister1 = last.command.selfReadRegister1;
			int readRegister2 = last.command.selfReadRegister2;
			int writeRegister = last.command.selfWriteRegister;

			if(readRegister1 != -1)
				tmp.previousReadRegister[readRegister1]++;
			if(readRegister2 != -1)
				tmp.previousReadRegister[readRegister2]++;
			if(writeRegister != -1)
				tmp.previousWriteRegister[writeRegister]++;

		}
		instructionQueue.erase(instructionQueue.begin());

		//now fetch register values from pipeline registers into tmp.command corresponding registers if possible
		switch(tmp.command.kind)
		{
			case 1://SW
				if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
				{
					tmp.command.first = tempRegisters[tmp.command.selfReadRegister1];
					tmp.command.firstReady = true;
				}
				if(tmp.previousWriteRegister[tmp.command.selfReadRegister2] == 0)
				{
					tmp.command.third = tempRegisters[tmp.command.selfReadRegister2];
					tmp.command.thirdReady = true;
				}
				break;
			case 2://LW
				if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
				{
					tmp.command.third = tempRegisters[tmp.command.selfReadRegister1];
					tmp.command.thirdReady = true;
				}
				break;
			case 3://J
				break;
			case 4://BEQ
			case 5://BNE
				if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
				{
					tmp.command.first = tempRegisters[tmp.command.selfReadRegister1];
					tmp.command.firstReady = true;
				}
				if(tmp.previousWriteRegister[tmp.command.selfReadRegister2] == 0)
				{
					tmp.command.second = tempRegisters[tmp.command.selfReadRegister2];
					tmp.command.secondReady = true;
				}
				break;
			case 6://BGEZ
			case 7://BGTZ
			case 8://BLEZ
			case 9://BLTZ
				if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
				{
					tmp.command.first = tempRegisters[tmp.command.selfReadRegister1];
					tmp.command.firstReady = true;
				}
				break;
			case 10://ADDI
			case 11://ADDIU
			case 12://SLTI
				if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
				{
					tmp.command.second = tempRegisters[tmp.command.selfReadRegister1];	
					tmp.command.secondReady = true;
				}
				break;
			case 13://BREAK
				break;
			case 14://SLL
			case 15://SRL
			case 16://SRA
				if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
				{
					tmp.command.second = tempRegisters[tmp.command.selfReadRegister1];
					tmp.command.secondReady = true;
				}
				break;
			case 17://SLT
			case 18://SLTU
			case 19://SUB
			case 20://SUBU
			case 21://ADD
			case 22://ADDU
			case 23://AND
			case 24://OR
			case 25://XOR
			case 26://NOR
				if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
				{
					tmp.command.second = tempRegisters[tmp.command.selfReadRegister1];
					tmp.command.secondReady = true;
				}
				if(tmp.previousWriteRegister[tmp.command.selfReadRegister2] == 0)
				{
					tmp.command.third= tempRegisters[tmp.command.selfReadRegister2];
					tmp.command.thirdReady = true;
				}
				break;
			case 27://NOP
				break;
		}
		//if is NOP or BREAK, only put the command in the tail of the reorder buffer, ready to be commit
		if(textCommands[index].kind == 13 || textCommands[index].kind == 27)
		{
			tmp.ready = true;
			reorderBuffer.push_back(tmp);
		}//else get put the command into both reservation station and reorder buffer, make the connections
		else
		{
			reservationStation.push_back(tmp);
			reorderBuffer.push_back(tmp);
			reservationStation[reservationStation.size()-1].id = reorderBuffer.size() - 1;
			reorderBuffer[reorderBuffer.size()-1].id = reservationStation.size()-1;
		}
	}	
}
