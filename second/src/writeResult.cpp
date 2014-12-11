#include "writeResult.h"

void writeResult()
{
	int size = reservationStation.size();
	for(int i = 0; i < size; i++)
	{
		Entry tmp = reservationStation[i];
		if(tmp.stage == 2 && tmp.ready == false) //instruction has been executed, need to write result
		{
			tmp.stage = 3;
			tmp.ready = true;
			switch(tmp.command.kind)
			{
				case 1://SW
				case 3://J
				case 4://BEQ
				case 5://BNE
				case 6://BGEZ
				case 7://BGTZ
				case 8://BLEZ
				case 9://BLTZ
				case 13://BREAK
				case 27://NOP
					break;
				case 2://LW
				case 10://ADDI
				case 11://ADDIU
				case 12://SLTI
				case 14://SLL
				case 15://SRL
				case 16://SRA
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
					tempRegisters[tmp.command.selfWriteRegister] = tmp.command.first;
					tmp.stage = 3;
					break;
			}
		}
		reservationStation[i] = tmp;
		reorderBuffer[tmp.id] = tmp;
		reorderBuffer[tmp.id].id = i;
	}
}
