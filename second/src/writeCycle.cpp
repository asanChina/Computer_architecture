#include "writeCycle.h"


void writeCycle(char* outputfile, int cycle)
{
	FILE *fp = fopen(outputfile, "a");
	fprintf(fp, "Cycle <%d>:\n", cycle);
	fprintf(fp, "IQ:\n");
	for(int i = 0; i < instructionQueue.size(); i++)
	{
		fprintf(fp, "[");
		writeTextCommand(fp, instructionQueue[i].index);
		fprintf(fp, "]\n");
	}

	fprintf(fp, "RS:\n");
	for(int i = 0; i < reservationStation.size(); i++)
	{
		fprintf(fp, "[");
		writeTextCommand(fp, reservationStation[i].ins.index);
		fprintf(fp, "]\n");
	}

	fprintf(fp, "ROB:\n");
	for(int i = 0; i < reorderBuffer.size(); i++)
	{
		fprintf(fp, "[");
		writeTextCommand(fp, reorderBuffer[i].ins.index);
		fprintf(fp, "]\n");
	}

	fprintf(fp, "BTB:\n");
	for(int i = 0; i < branchTargetBuffer.size(); i++)
	{
		fprintf(fp, "[Entry %d]:<%d,%d,", i+1, branchTargetBuffer[i].address, branchTargetBuffer[i].targetAddress);
		switch(branchTargetBuffer[i].prediction)
		{
			case -1: fprintf(fp, "NotSet>\n");break;
			case 0: fprintf(fp, "0>\n"); break;
			case 1: fprintf(fp, "1>\n"); break;
		}	
	}

	fprintf(fp, "Registers:\n");
	for(int i = 0; i < 4; i++)
	{
		switch(i*8)
		{
			case 0: fprintf(fp, "R00:");break;
			case 8: fprintf(fp, "R08:");break;
			case 16: fprintf(fp, "R16:");break;
			case 24: fprintf(fp, "R24:");break;
		}
		int j;
		for(j = 0; j < 7; j++)
			fprintf(fp, "	%d", registers[i*8+j]);
		fprintf(fp, "	%d\n", registers[i*8+j]); 
	}

	fprintf(fp, "Data Segment:\n706:");
	/*
	int i;
	for(i = 0; i < memory.size()/10; i++)
	{
		fprintf(fp, "%d:", dataStartAddress + i*10*4);
		int j;
		for(j = 0; j < 9; j++)
			fprintf(fp, "	%d", memory[i*10+j]);
		fprintf(fp, "	%d\n", memory[i*10+j]);
	}
	if(memory.size() % 10 != 0)
	{
		fprintf(fp, "%d:", dataStartAddress+i*10*4);
		for(int j = 0; i*10+j < memory.size()-1; j++)
			fprintf(fp, "	%d", memory[i*10+j]);	
		fprintf(fp, "	%d\n", memory[memory.size()-1]);
	}*/
	for(int i = 0; i < 10; i++)
		fprintf(fp, "	%d", memory[(706+i*4-dataStartAddress)/4]);
	fprintf(fp, "\n");
	fclose(fp);
}

void writeLastCycle(char* outputfile, int cycle)
{
	FILE *fp = fopen(outputfile, "a");
	fprintf(fp, "Final Cycle <%d>:\n", cycle);
	fprintf(fp, "IQ:\n");
	fprintf(fp, "RS:\n");
	fprintf(fp, "ROB:\n");
	fprintf(fp, "BTB:\n");
	for(int i = 0; i < branchTargetBuffer.size(); i++)
	{
		fprintf(fp, "[Entry %d]:<%d,%d,", i+1, branchTargetBuffer[i].address, branchTargetBuffer[i].targetAddress);
		switch(branchTargetBuffer[i].prediction)
		{
			case -1: fprintf(fp, "NotSet>\n");break;
			case 0: fprintf(fp, "0>\n"); break;
			case 1: fprintf(fp, "1>\n"); break;
		}	
	}

	fprintf(fp, "Registers:\n");
	for(int i = 0; i < 4; i++)
	{
		switch(i*8)
		{
			case 0: fprintf(fp, "R00:");break;
			case 8: fprintf(fp, "R08:");break;
			case 16: fprintf(fp, "R16:");break;
			case 24: fprintf(fp, "R24:");break;
		}
		int j;
		for(j = 0; j < 7; j++)
			fprintf(fp, "	%d", registers[i*8+j]);
		fprintf(fp, "	%d\n", registers[i*8+j]); 
	}

	fprintf(fp, "Data Segment:\n");
	/*
	int i;
	for(i = 0; i < memory.size()/10; i++)
	{
		fprintf(fp, "%d:", dataStartAddress + i*10*4);
		int j;
		for(j = 0; j < 9; j++)
			fprintf(fp, "	%d", memory[i*10+j]);
		fprintf(fp, "	%d\n", memory[i*10+j]);
	}
	if(memory.size() % 10 != 0)
	{
		fprintf(fp, "%d:", dataStartAddress+i*10*4);
		for(int j = 0; i*10+j < memory.size()-1; j++)
			fprintf(fp, "	%d", memory[i*10+j]);	
		fprintf(fp, "	%d\n", memory[memory.size()-1]);
	}
	*/
	for(int i = 0; i < 10; i++)
	fprintf(fp, "	%d", memory[(706+i*4-dataStartAddress)/4]);
	fprintf(fp, "\n");
	fclose(fp);
}



void writeTextCommand(FILE *fpw, int index)
{
	int first = textCommands[index].first;
	int second = textCommands[index].second;
	int third = textCommands[index].third;

	switch(textCommands[index].kind)
	{
		case 1: fprintf(fpw, "SW R%d, %d(R%d)", first, second, third);break;
		case 2: fprintf(fpw, "LW R%d, %d(R%d)", first, second, third); break;
		case 3: fprintf(fpw, "J #%d", first); break;
		case 4: fprintf(fpw, "BEQ R%d, R%d, #%d", first, second, third); break;
		case 5: fprintf(fpw, "BNE R%d, R%d, #%d", first, second, third); break;
		case 6: fprintf(fpw, "BGEZ R%d, #%d", first, second); break;
		case 7: fprintf(fpw, "BGTZ R%d, #%d", first, second); break;
		case 8: fprintf(fpw, "BLEZ R%d, #%d", first, second); break;
		case 9: fprintf(fpw, "BLTZ R%d, #%d", first, second); break;
		case 10: fprintf(fpw, "ADDI R%d, R%d, #%d", first, second, third); break;
		case 11: fprintf(fpw, "ADDIU R%d, R%d, #%d", first, second, third); break;
		case 12: fprintf(fpw, "SLTI R%d, R%d, #%d", first, second, third); break;
		case 13: fprintf(fpw, "BREAK");break;
		case 14: fprintf(fpw, "SLL R%d, R%d, #%d", first, second, third); break;
		case 15: fprintf(fpw, "SRL R%d, R%d, #%d", first, second, third); break;
		case 16: fprintf(fpw, "SRA R%d, R%d, #%d", first, second, third); break;
		case 17: fprintf(fpw, "SLT R%d, R%d, R%d", first, second, third); break;
		case 18: fprintf(fpw, "SLTU R%d, R%d, R%d", first, second, third); break;
		case 19: fprintf(fpw, "SUB R%d, R%d, R%d", first, second, third); break;
		case 20: fprintf(fpw, "SUBU R%d, R%d, R%d", first, second, third); break;
		case 21: fprintf(fpw, "ADD R%d, R%d, R%d", first, second, third); break;
		case 22: fprintf(fpw, "ADDU R%d, R%d, R%d", first, second, third); break;
		case 23: fprintf(fpw, "AND R%d, R%d, R%d", first, second, third); break;
		case 24: fprintf(fpw, "OR R%d, R%d, R%d", first, second, third); break;
		case 25: fprintf(fpw, "XOR R%d, R%d, R%d", first, second, third); break;
		case 26: fprintf(fpw, "NOR R%d, R%d, R%d", first, second, third); break;
		case 27: fprintf(fpw, "NOP");break;
	}
}

