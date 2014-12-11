#include "execute.h"
#include <iostream>
using namespace std;


void execute()
{
	int size = reservationStation.size();
	bool flag;
	bool branch;
	for(int i = 0; i < size; i++)
	{
		Entry tmp = reservationStation[i];
	
		if(tmp.stage == 1 && tmp.ready == false)
		{
			switch(tmp.command.kind)
			{
				case 1://SW
					//first check whether there are LW/SW command before this SW which haven't generate its address
					flag = false;
					for(int j = 0; j < i && !flag; j++)
						if((reservationStation[j].command.kind == 1 || reservationStation[j].command.kind == 2) && reservationStation[j].command.loadStoreAddress == -1)
						{
							flag = true;
							break;
						}
					if(flag)
						break;
					//if(tmp.loadStoreCount == 0)
					//{
						if(tmp.command.thirdReady)
						{
							tmp.command.loadStoreAddress = tmp.command.second + tmp.command.third;
							tmp.loadStoreCount = 1;
						}
						else if(tmp.previousWriteRegister[tmp.command.selfReadRegister2] == 0)
						{
							tmp.command.third = tempRegisters[tmp.command.selfReadRegister2];
							tmp.command.thirdReady = true;
							tmp.command.loadStoreAddress = tmp.command.second + tmp.command.third;
							tmp.loadStoreCount = 1;	
						}
					//}
					if(tmp.loadStoreCount == 1)
					{
						if(tmp.command.firstReady)
						{
							tmp.loadStoreCount = 2;
							tmp.stage = 2;
							tmp.ready = true;
						}
						else if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
						{
							tmp.command.first = tempRegisters[tmp.command.selfReadRegister1];
							tmp.command.firstReady = true;
							tmp.loadStoreCount = 2;
							tmp.stage = 2;
							tmp.ready = true;
						}
					}
					break;
				case 2://LW, the same as SW
					//first check whether there are LW/SW command before this LW which haven't generate its address
					flag = false;
					for(int j = 0; j < i && !flag; j++)
						if((reservationStation[j].command.kind == 1 || reservationStation[j].command.kind == 2) && reservationStation[j].command.loadStoreAddress == -1)
						{
							flag = true;
							break;
						}
					if(flag)
						break;
					if(tmp.loadStoreCount == 0)
					{
						if(tmp.command.thirdReady)
						{
							tmp.command.loadStoreAddress = tmp.command.second + tmp.command.third;
							tmp.loadStoreCount = 1;
						}
						else if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
						{
							tmp.command.third = tempRegisters[tmp.command.selfReadRegister1];
							tmp.command.thirdReady = true;
							tmp.command.loadStoreAddress = tmp.command.second + tmp.command.third;
							tmp.loadStoreCount = 1;
						}
					}
					else if(tmp.loadStoreCount == 1)
					{
						flag = true;
						for(int j = i-1; j >= 0; j--)
						{
							if(reservationStation[j].command.kind == 1 && reservationStation[j].command.loadStoreAddress == tmp.command.loadStoreAddress)
							{
								if(reservationStation[j].stage != 4)
								{
									flag = false;
									break;
								}
							}
						}	
						if(flag)
						{
							tmp.command.first = memory[(tmp.command.loadStoreAddress-dataStartAddress)/4];
							tmp.command.firstReady = true;
							tmp.loadStoreCount = 2;
							tmp.stage = 2;
						}
					}
					break;
				case 3://J
					tmp.stage = 2;
					tmp.ready = true;	
					tmp.ins.realAction = 1;
					break;
				case 4://BEQ
				case 5://BNE
					if(tmp.command.firstReady && tmp.command.secondReady)
					{
						tmp.stage = 2;
						tmp.ready = true;
						switch(tmp.command.kind)
						{
							case 4: if(tmp.command.first == tmp.command.second)
								{
									tmp.ins.realAction = 1;
									//updateBranchTargetBuffer(tmp.command.address, 1);
								}
								else
								{
									tmp.ins.realAction = 0;
									//updateBranchTargetBuffer(tmp.command.address, 0);
								}
								break;
							case 5: if(tmp.command.first != tmp.command.second)
								{
									tmp.ins.realAction = 1;
									//updateBranchTargetBuffer(tmp.command.address, 1);
								}
								else
								{
									tmp.ins.realAction = 0;
									//updateBranchTargetBuffer(tmp.command.address, 0);
								}
								break;
						}
					}
					else if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0 && tmp.previousWriteRegister[tmp.command.selfReadRegister2] == 0)
					{
						tmp.stage = 2;
						tmp.ready = true;
						tmp.command.first = tempRegisters[tmp.command.selfReadRegister1];
						tmp.command.firstReady = true;
						tmp.command.second = tempRegisters[tmp.command.selfReadRegister2];
						tmp.command.secondReady = true;
						switch(tmp.command.kind)
						{
							case 4: if(tmp.command.first == tmp.command.second)
								{
									tmp.ins.realAction = 1;
									//updateBranchTargetBuffer(tmp.command.address, 1);
								}
								else
								{
									tmp.ins.realAction = 0;
									//updateBranchTargetBuffer(tmp.command.address, 0);
								}
								break;
							case 5: if(tmp.command.first != tmp.command.second)
								{
									tmp.ins.realAction = 1;
									//updateBranchTargetBuffer(tmp.command.address, 1);
								}
								else
								{
									tmp.ins.realAction = 0;
									//updateBranchTargetBuffer(tmp.command.address, 0);
								}
								break;
						}
					}
					break;
				case 6://BGEZ
				case 7://BGTZ
				case 8://BLEZ
				case 9://BLTZ
					if(tmp.command.firstReady)
					{
						tmp.stage = 2;
						tmp.ready = true;
						branch = false;
						switch(tmp.command.kind)
						{
							case 6: if(tmp.command.first >= 0)
									branch = true;
								break;
							case 7: if(tmp.command.first > 0)
									branch = true;
								break;
							case 8: if(tmp.command.first <= 0)
									branch = true;
								break;
							case 9: if(tmp.command.first < 0)
									branch = true;
								break;
						}
						if(branch)
							tmp.ins.realAction = 1;
						else 
							tmp.ins.realAction = 0;
						//updateBranchTargetBuffer(tmp.command.address, tmp.ins.realAction);
					}
					else if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
					{
						tmp.command.first = tempRegisters[tmp.command.selfReadRegister1];
						tmp.command.firstReady = true;
						tmp.stage = 2;
						tmp.ready = true;
						branch = false;
						switch(tmp.command.kind)
						{
							case 6: if(tmp.command.first >= 0)
									branch = true;
								break;
							case 7: if(tmp.command.first > 0)
									branch = true;
								break;
							case 8: if(tmp.command.first <= 0)
									branch = true;
								break;
							case 9: if(tmp.command.first < 0)
									branch = true;
								break;
						}
						if(branch)
							tmp.ins.realAction = 1;
						else 
							tmp.ins.realAction = 0;
						//updateBranchTargetBuffer(tmp.command.address, tmp.ins.realAction);
					}
					break;
				case 10://ADDI
				case 11://ADDIU
				case 12://SLTI
					if(tmp.command.secondReady)
					{
						switch(tmp.command.kind)
						{
							case 10: tmp.command.first = tmp.command.second + tmp.command.third;
								break;			
							case 11: tmp.command.first = tmp.command.second + tmp.command.third;
								break;
							case 12: tmp.command.first = (tmp.command.second<tmp.command.third?1:0);
								break;
						}
						tmp.command.firstReady = true;
						tmp.stage = 2;
					}
					else if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
					{
						tmp.command.second = tempRegisters[tmp.command.selfReadRegister1];
						tmp.command.secondReady = true;
						switch(tmp.command.kind)
						{
							case 10: tmp.command.first = tmp.command.second + tmp.command.third;
								break;
							case 11: tmp.command.first = tmp.command.second + tmp.command.third;
								break;
							case 12: tmp.command.first = (tmp.command.second<tmp.command.third?1:0);
								break;
						}
						tmp.command.firstReady = true;
						tmp.stage = 2;
					}
					break;
				case 13://BREAK
					break;
				case 14://SLL
				case 15://SRL
				case 16://SRA
					if(tmp.command.secondReady)
					{
						switch(tmp.command.kind)
						{
							case 14: tmp.command.first = tmp.command.second << tmp.command.third;
								break;
							case 15: shiftRightLogical(tmp.command.first, tmp.command.second, tmp.command.third);
								break;
							case 16: shiftRightArithmetic(tmp.command.first, tmp.command.second, tmp.command.third);
								break;
						}
						tmp.command.firstReady = true;
						tmp.stage = 2;
					}
					else if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
					{
						tmp.command.second = tempRegisters[tmp.command.selfReadRegister1];
						tmp.command.secondReady = true;
						switch(tmp.command.kind)
						{
							case 14: tmp.command.first = tmp.command.second << tmp.command.third;
								break;
							case 15: shiftRightLogical(tmp.command.first, tmp.command.second, tmp.command.third);
								break;
							case 16: shiftRightArithmetic(tmp.command.first, tmp.command.second, tmp.command.third);
								break;
						}
						tmp.command.firstReady = true;
						tmp.stage = 2;
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
					if(tmp.command.secondReady && tmp.command.thirdReady)
					{}
					if(tmp.command.secondReady)
					{
						if(tmp.previousWriteRegister[tmp.command.selfReadRegister2] == 0)
						{
							tmp.command.third = tempRegisters[tmp.command.selfReadRegister2];
							tmp.command.thirdReady = true;
						}
					}
					else if(tmp.command.thirdReady)
					{
						if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0)
						{
							tmp.command.second = tempRegisters[tmp.command.selfReadRegister1];
							tmp.command.secondReady = true;
						}
						
					}
					else if(tmp.previousWriteRegister[tmp.command.selfReadRegister1] == 0 && tmp.previousWriteRegister[tmp.command.selfReadRegister2] == 0)
					{
						tmp.command.second = tempRegisters[tmp.command.selfReadRegister1];
						tmp.command.secondReady = true;
						tmp.command.third= tempRegisters[tmp.command.selfReadRegister2];
						tmp.command.thirdReady = true;
					}
					if(tmp.command.secondReady && tmp.command.thirdReady)
					{
						switch(tmp.command.kind)
						{
							case 17: tmp.command.first = (tmp.command.second<tmp.command.third?1:0);
								break;
							case 18: tmp.command.first = (tmp.command.second<tmp.command.third?1:0);
								break;
							case 19: tmp.command.first = tmp.command.second - tmp.command.third;
								break;
							case 20: tmp.command.first = tmp.command.second - tmp.command.third;
								break;
							case 21: tmp.command.first = tmp.command.second + tmp.command.third;
								break;
							case 22: tmp.command.first = tmp.command.second + tmp.command.third;
								break;
							case 23: tmp.command.first = tmp.command.second & tmp.command.third;
								break;
							case 24: tmp.command.first = tmp.command.second | tmp.command.third;
								break;
							case 25: tmp.command.first = tmp.command.second ^ tmp.command.third;
								break;
							case 26: tmp.command.first = ~(tmp.command.second | tmp.command.third);
								break;
						}
						tmp.stage = 2;
					}
					break;
				case 27://NOP
					break;
			}
			//we should make changes on reservation station entries
			reservationStation[i] = tmp;
			reorderBuffer[reservationStation[i].id] = reservationStation[i];
			reorderBuffer[reservationStation[i].id].id = i;
		}
	}
}


void shiftRightLogical(int& result, int operand, int shiftAmount)
{
	operand >>= shiftAmount;
	result = operand & (1<<(32-shiftAmount)-1);
}

void shiftRightArithmetic(int& result, int operand, int shiftAmount)
{
	if(operand < 0)
	{
		for(int i = 0; i < shiftAmount; i++)
		{
			operand >>= 1;
			operand |= 0x80000000;
		}
		result = operand;
	}
	else
		shiftRightLogical(result, operand, shiftAmount);
}


