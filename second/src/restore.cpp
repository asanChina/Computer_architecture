#include "restore.h"
#include <iostream>
using namespace std;

void restore()
{
	int size = reorderBuffer.size();
	if(size <= 0)
		return;

	for(int i = 0; i < reservationStation.size(); i++)
	{
		if(reservationStation[i].stage == 2 && !reservationStation[i].hasUpdateBranchTargetBuffer)
		{
			switch(reservationStation[i].command.kind)
			{
				case 3: 
				case 4:
				case 5: 
				case 6: 
				case 7: 
				case 8: 
				case 9: updateBranchTargetBuffer(reservationStation[i].command.address, reservationStation[i].ins.realAction);
					reservationStation[i].hasUpdateBranchTargetBuffer = true;
					if(reservationStation[i].ins.realAction == 1)
					{
						if(reservationStation[i].ins.prediction == 0 || reservationStation[i].ins.prediction == -1)
						{
							PC = reservationStation[i].command.jumpAddress;
/*
						if(reservationStation[i].command.kind == 4)
						{
						 cout << "PC=" << PC << "in restore(): BEQ "<< reservationStation[i].ins.prediction << " " << reservationStation[i].command.jumpAddress << "   " << reservationStation[i].ins.realAction << "   "  << reservationStation[i].command.address << " cycle: " << cycle <<endl; 
						}
*/
						}
						removeEntry(i);
					}
					else
					{
						if(reservationStation[i].ins.prediction == 1)
							PC = reservationStation[i].command.address + 4;
						removeEntry(i);
					}
					
					//cout << "hahhaahhahahahahahah" << reservationStation[i].command.address << endl;
					break;
			}
		}

	
		if(reservationStation[i].stage == 3 && !reservationStation[i].hasRestoreWriteResult)
		{
			Entry tmp = reservationStation[i];
			int readRegister1 = tmp.command.selfReadRegister1;
			int readRegister2 = tmp.command.selfReadRegister2;
			int writeRegister = tmp.command.selfWriteRegister;
		
					for(int j = i+1; j < reservationStation.size(); j++)
					{
						if(readRegister1 != -1)
							reservationStation[j].previousReadRegister[readRegister1]--;
						if(readRegister2 != -1)
							reservationStation[j].previousReadRegister[readRegister2]--;
						if(writeRegister != -1)
							reservationStation[j].previousWriteRegister[writeRegister]--;
						if(reservationStation[j].stage == 1 && reservationStation[j].ready == false)
						{
							Entry haha = reservationStation[j];
							switch(haha.command.kind)
							{
							case 1://SW
								if(!haha.command.firstReady && haha.previousWriteRegister[haha.command.selfReadRegister1] == 0)
								{
									haha.command.firstReady = true;
									haha.command.first = tempRegisters[haha.command.selfReadRegister1];
								}
								if(!haha.command.thirdReady && haha.previousWriteRegister[haha.command.selfReadRegister2] == 0)
								{
									haha.command.thirdReady = true;
									haha.command.third = tempRegisters[haha.command.selfReadRegister2];
								}
								break;	
							case 2://LW
								if(!haha.command.thirdReady && haha.previousWriteRegister[haha.command.selfReadRegister1] == 0)
								{
									haha.command.thirdReady = true;
									haha.command.third = tempRegisters[haha.command.selfReadRegister1];
								}
								break;
							case 3://J
								break;
							case 4://BEQ
							case 5://BNE
								if(!haha.command.firstReady && haha.previousWriteRegister[haha.command.selfReadRegister1] == 0)
								{
									haha.command.firstReady = true;
									haha.command.first = tempRegisters[haha.command.selfReadRegister1];
								}
								if(!haha.command.secondReady && haha.previousWriteRegister[haha.command.selfReadRegister2] == 0)
								{
									haha.command.secondReady = true;
									haha.command.second = tempRegisters[haha.command.selfReadRegister2];
								}
								break;
							case 6://BGEZ
							case 7://BGTZ
							case 8://BLEZ
							case 9://BLTZ
								if(!haha.command.firstReady && haha.previousWriteRegister[haha.command.selfReadRegister1] == 0)
								{
									haha.command.firstReady = true;
									haha.command.first = tempRegisters[haha.command.selfReadRegister1];
								}
								break;
							case 10://ADDI
							case 11://ADDIU
							case 12://SLTI
								if(!haha.command.secondReady && haha.previousWriteRegister[haha.command.selfReadRegister1] == 0)
								{
									haha.command.secondReady = true;
									haha.command.second = tempRegisters[haha.command.selfReadRegister1];
								}
								break;
							case 13://BREAK
								break;
							case 14://SLL
							case 15://SRL
							case 16://SRA
								if(!haha.command.secondReady && haha.previousWriteRegister[haha.command.selfReadRegister1] == 0)
								{
									haha.command.secondReady = true;
									haha.command.second = tempRegisters[haha.command.selfReadRegister1];
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
								if(!haha.command.secondReady && haha.previousWriteRegister[haha.command.selfReadRegister1] == 0)
								{
									haha.command.secondReady = true;
									haha.command.second = tempRegisters[haha.command.selfReadRegister1];
								}
								if(!haha.command.thirdReady && haha.previousWriteRegister[haha.command.selfReadRegister2] == 0)
								{
									haha.command.thirdReady = true;
									haha.command.third = tempRegisters[haha.command.selfReadRegister2];
								}
								break;
							case 27://NOP
								break;
							}
							reservationStation[j] = haha;
						}
					}
			reservationStation[i].hasRestoreWriteResult = true;
		}
	}

	if(reorderBuffer.size() > 0 && reorderBuffer[0].stage == 4)
	{
		Entry tmp = reorderBuffer[0];
		switch(tmp.command.kind)
		{
			case 1://SW
				memory[(tmp.command.loadStoreAddress-dataStartAddress)/4] = tmp.command.first;
				removeEntry();
				break;
			case 2://LW
				registers[tmp.command.selfWriteRegister] = tmp.command.first;				
				removeEntry();
				break;
			case 3://J
			case 4://BEQ
			case 5://BNE
			case 6://BGEZ
			case 7://BGTZ
			case 8://BLEZ
			case 9://BLTZ
				//removeEntry(tmp.ins.realAction, tmp.ins.prediction);
				removeEntry();
				break;
			case 10://ADDI
			case 11://ADDIU
			case 12://SLTI
				registers[tmp.command.selfWriteRegister] = tmp.command.first;
				removeEntry();
				break;
			case 13://BREAK
				removeEntry();
				programEnd = true;
				break;
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
				registers[tmp.command.selfWriteRegister] = tmp.command.first;
				removeEntry();
				break;
			case 27://NOP 
				removeEntry();
				break;
		}
	}
}


void removeEntry(int index)
{
	Entry tmp = reservationStation[index];
	int rsid = tmp.id;
	int realAction = tmp.ins.realAction;
	int prediction = tmp.ins.prediction;

	//rsid==-1 shouldn't happen in reality if code is written correctly
	//but for safety, I add this check
	if(rsid == -1)
		return;

	vector<Entry>::iterator ite = reorderBuffer.begin();
	for(int i = 0; i < rsid; i++)
		ite++;
	ite++;
	vector<Entry>::iterator ite1 = reservationStation.begin();
	for(int j = 0; j < index; j++)
		ite1++;
	ite1++;

	if(realAction == prediction)
	{
/*
		reservationStation.erase(ite);
		reorderBuffer.erase(reorderBuffer.begin());
		for(int j = rsid; j < reservationStation.size(); j++)
			reservationStation[j].id -= 1;
		for(int j = 0; j < reorderBuffer.size(); j++)
			reorderBuffer[j].id -= 1;
*/
	}
	else if(realAction == 0 && prediction == 1)
	{
//		PC = reorderBuffer[0].command.address + 4;
		reorderBuffer.erase(ite, reorderBuffer.end());
		reservationStation.erase(ite1, reservationStation.end());
		instructionQueue.clear();
	}
	else if(realAction == 1 && (prediction == 0 || prediction == -1))
	{
/*
		if(reservationStation[index].command.kind == 4)
			cout << "in removeEntry(...) BEQ\n";
*/
		reorderBuffer.erase(ite, reorderBuffer.end());
		reservationStation.erase(ite1, reservationStation.end());
		instructionQueue.clear();
	}
}

void removeEntry()
{
	int rsid = reorderBuffer[0].id;
	if(rsid == -1)
	{
		reorderBuffer.erase(reorderBuffer.begin());
		for(int j = 0; j < reorderBuffer.size(); j++)
			reorderBuffer[j].id -= 1;
		return;
	}
	vector<Entry>::iterator ite = reservationStation.begin();
	for(int i = 0; i < rsid; i++)
		ite++;

	reservationStation.erase(ite);
	reorderBuffer.erase(reorderBuffer.begin());
	for(int j = rsid; j < reservationStation.size(); j++)
		reservationStation[j].id -= 1;
	for(int j = 0; j < reorderBuffer.size(); j++)
		reorderBuffer[j].id -= 1;
}



void updateBranchTargetBuffer(int add, int pre)
{
	int size = branchTargetBuffer.size();
	for(int i = 0; i < size; i++)
		if(branchTargetBuffer[i].address == add)
		{
			branchTargetBuffer[i].prediction = pre;
			break;
		}
	return;
}
