#include "disassemble.h"
#include <stdio.h>
#include <iostream>
#include "Kind.h"
using namespace std;

void disassemble()
{
	int size = binaryCommands.size();
	bool isData = false;
	for(int i = 0; i < size; i++)
	{
		Cmd tmp;
		tmp.address = i*4+programStartAddress;
		if(isData)
		{
			tmp.first = getBinaryValue(binaryCommands[i]);
			tmp.kind = 0;
		}
		else
			disassembleCommand(i, tmp, isData);
		textCommands.push_back(tmp);
	}			
	return;
}


void disassembleCommand(int index, Cmd& tmp, bool& isData)
{
	string str = binaryCommands[index];
	string opcode = str.substr(0, 6);
	if(str == "00000000000000000000000000000000") //NOP
	{
		tmp.kind = NOP;
	}
	else if(opcode == "101011")//SW
	{
		tmp.kind = SW;
		string rt = str.substr(11, 5);
		string offset = str.substr(16, 16);
		extendString(offset);
		string base = str.substr(6, 5);
		tmp.first = getBinaryValue(rt);
		tmp.second = getBinaryValue(offset);
		tmp.third = getBinaryValue(base);
		tmp.selfReadRegister1 = tmp.first;
		tmp.selfReadRegister2 = tmp.third;
	}
	else if(opcode == "100011") //LW
	{
		tmp.kind = LW;
		string rt = str.substr(11, 5);
		string offset = str.substr(16, 16);
		extendString(offset);
		string base = str.substr(6, 5);
		tmp.first = getBinaryValue(rt);
		tmp.second = getBinaryValue(offset);
		tmp.third = getBinaryValue(base);
		tmp.selfReadRegister1 = tmp.third;
		tmp.selfWriteRegister = tmp.first;
	}
	else if(opcode == "000010") //J
	{
		tmp.kind = J;
		string ins = str.substr(6, 26);
		int value = getBinaryValue(ins);
		value <<= 2;
		int comp = 0xf0000000;
		int a = (comp&(tmp.address+4));
		int b = (a|value);
		tmp.first = b;		
		tmp.jumpAddress = b;
	}
	else if(opcode == "000100") //BEQ
	{
		tmp.kind = BEQ;
		string rs = str.substr(6, 5);
		string rt = str.substr(11, 5);
		string offset = str.substr(16, 16);
		offset += "00";
		extendString(offset);
		tmp.first = getBinaryValue(rs);
		tmp.second = getBinaryValue(rt);
		tmp.third =  getBinaryValue(offset);
		tmp.selfReadRegister1 = tmp.first;
		tmp.selfReadRegister2 = tmp.second;
		tmp.jumpAddress = tmp.third + tmp.address + 4;
	}
	else if(opcode == "000101") //BNE
	{
		tmp.kind = BNE;
		string rs = str.substr(6, 5);
		string rt = str.substr(11, 5);
		string offset = str.substr(16, 16);
		offset += "00";
		extendString(offset);
		tmp.first = getBinaryValue(rs);
		tmp.second = getBinaryValue(rt);
		tmp.third = getBinaryValue(offset);
		tmp.selfReadRegister1 = tmp.first;
		tmp.selfReadRegister2 = tmp.second;
		tmp.jumpAddress = tmp.third + tmp.address + 4;
	}
	else if(opcode == "000001") //BGEZ or BLTZ
	{
		string secondOpcode = str.substr(11, 5);
		if(secondOpcode == "00001") //BGEZ
		{
			tmp.kind = BGEZ;
			string rs = str.substr(6, 5);
			string offset = str.substr(16, 16);
			offset += "00";
			extendString(offset);
			tmp.first = getBinaryValue(rs);
			tmp.second = getBinaryValue(offset);	
			tmp.selfReadRegister1 = tmp.first;
			tmp.jumpAddress = tmp.second + tmp.address + 4;
		}
		else //BLTZ
		{
			tmp.kind = BLTZ;
			string rs = str.substr(6, 5);
			string offset = str.substr(16, 16);
			offset += "00";
			extendString(offset);
			tmp.first = getBinaryValue(rs);
			tmp.second = getBinaryValue(offset);
			tmp.selfReadRegister1 = tmp.first;
			tmp.jumpAddress = tmp.second + tmp.address + 4;
		}
	}
	else if(opcode == "000111") //BGTZ
	{
		tmp.kind = BGTZ;
		string rs = str.substr(6, 5);
		string offset = str.substr(16, 16);
		offset += "00";
		extendString(offset);
		tmp.first = getBinaryValue(rs);
		tmp.second = getBinaryValue(offset);
		tmp.selfReadRegister1 = tmp.first;
		tmp.jumpAddress = tmp.second + tmp.address + 4;
	}
	else if(opcode == "000110") //BLEZ
	{
		tmp.kind = BLEZ;
		string rs = str.substr(6, 5);	
		string offset = str.substr(16, 16);
		offset += "00";
		extendString(offset);
		tmp.first = getBinaryValue(rs);
		tmp.second = getBinaryValue(offset);
		tmp.selfReadRegister1 = tmp.first;
		tmp.jumpAddress = tmp.second + tmp.address + 4;
	}
	else if(opcode == "001000") //ADDI
	{
		tmp.kind = ADDI;
		string rt = str.substr(11, 5);
		string rs = str.substr(6, 5);
		string immediate = str.substr(16, 16);
		extendString(immediate);
		tmp.first = getBinaryValue(rt);
		tmp.second = getBinaryValue(rs);
		tmp.third = getBinaryValue(immediate);
		tmp.selfReadRegister1 = tmp.second;
		tmp.selfWriteRegister = tmp.first;
	}
	else if(opcode == "001001") //ADDIU
	{
		tmp.kind = ADDIU;
		string rt = str.substr(11, 5);
		string rs = str.substr(6, 5);
		string immediate = str.substr(16, 16);
		extendString(immediate);
		tmp.first = getBinaryValue(rt);
		tmp.second = getBinaryValue(rs);
		tmp.third = getBinaryValue(immediate);
		tmp.selfReadRegister1 = tmp.second;
		tmp.selfWriteRegister = tmp.first;
	}
	else if(opcode == "000000") //all others
	{
		string rs = str.substr(6, 5);
		string rt = str.substr(11, 5);
		string rd = str.substr(16, 5);
		string sa = str.substr(21, 5);
		string secondOpcode = str.substr(26, 6);
		int rsvalue = getBinaryValue(rs);
		int rtvalue = getBinaryValue(rt);
		int rdvalue = getBinaryValue(rd);
		int savalue = getBinaryValue(sa);
		int secondOpcodevalue = getBinaryValue(secondOpcode);
		if(secondOpcode == "001101")//BREAK
		{
			tmp.kind = BREAK;
			isData = true;
			dataStartAddress = tmp.address+4;
		}
		else if(sa == "00000" && secondOpcode == "101010")//SLT
		{
			tmp.kind = SLT;
			tmp.first = rdvalue;
			tmp.second = rsvalue;
			tmp.third = rtvalue;
			tmp.selfWriteRegister = tmp.first;
			tmp.selfReadRegister1 = tmp.second;
			tmp.selfReadRegister2 = tmp.third;
		}
		else if(sa == "00000" && secondOpcode == "101011") //SLTU
		{
			tmp.kind = SLTU;
			tmp.first = rdvalue;
			tmp.second = rsvalue; 
			tmp.third = rtvalue;
			tmp.selfWriteRegister = tmp.first;
			tmp.selfReadRegister1 = tmp.second;
			tmp.selfReadRegister2 = tmp.third;
		}
		else if(rs == "00000" && secondOpcode == "000000") //SLL
		{
			tmp.kind = SLL;
			tmp.first = rdvalue;
			tmp.second = rtvalue;
			tmp.third = savalue;
			tmp.selfWriteRegister = tmp.first;
			tmp.selfReadRegister1 = tmp.second;
		}
		else if(rs == "00000" && secondOpcode == "000010") //SRL
		{
			tmp.kind = SRL;
			tmp.first = rdvalue;
			tmp.second = rtvalue;
			tmp.third = savalue;
			tmp.selfWriteRegister = tmp.first;
			tmp.selfReadRegister1 = tmp.second;
		}
		else if(rs == "00000" && secondOpcode == "000011") //SRA
		{
			tmp.kind = SRA;
			tmp.first = rdvalue;
			tmp.second = rtvalue;
			tmp.third = savalue;
			tmp.selfWriteRegister = tmp.first;
			tmp.selfReadRegister1 = tmp.second;
		}
		else
		{
			switch(secondOpcodevalue)
			{
				case 34: tmp.kind = SUB; break;
				case 35: tmp.kind = SUBU; break;
				case 32: tmp.kind = ADD; break;
				case 33: tmp.kind = ADDU; break;
				case 36: tmp.kind = AND; break;
				case 37: tmp.kind = OR; break;
				case 38: tmp.kind = XOR; break;
				case 39: tmp.kind = NOR; break;
			}
			tmp.first = rdvalue;
			tmp.second = rsvalue;
			tmp.third = rtvalue;
			tmp.selfWriteRegister = tmp.first;
			tmp.selfReadRegister1 = tmp.second;
			tmp.selfReadRegister2 = tmp.third;
		}
	}
	else if(opcode == "001010") // SLTI
	{
		tmp.kind = SLTI;
		string rt = str.substr(11, 5);
		string rs = str.substr(6, 5);
		string immediate = str.substr(16, 16);
		extendString(immediate);
		tmp.first = getBinaryValue(rt);
		tmp.second = getBinaryValue(rs);
		tmp.third = getBinaryValue(immediate);
		tmp.selfWriteRegister = tmp.first;
		tmp.selfReadRegister1 = tmp.second;
	}
	return;
}

int getBinaryValue(string str)
{
	int size = str.size();
	int value = 0;
	for(int i = size-1; i >= 0; i--)
		value += ((str[i]-'0')<<(size-1-i));
	return value;
}


bool readBin(char *inputfile)
{
	FILE *fp = fopen(inputfile, "rb");
	if(fp == NULL) 
	{
		cout << "Error in \"readBin(...)\", cannot open file \"" << inputfile << "\"\n";
		return false;
	}
	char arr[4];
	fread(arr, 4, 1, fp);
	while(!feof(fp))
	{
		string tmp(32, '0');
		for(int i = 0; i < 4; i++)
		{
			int comp = 128;
			for(int j = 0; j < 8; j++)
			{
				if((arr[i]&comp) == 0)
					tmp[i*8+j] = '0';
				else
					tmp[i*8+j] = '1';
				comp >>= 1;
			}
		}
		binaryCommands.push_back(tmp);
		fread(arr, 4, 1, fp);
	}
	fclose(fp);
	return true;
}

bool writeDisassemble(char *outputfile)
{
	FILE *fpw = fopen(outputfile, "w");
	if(fpw == NULL)
	{
		cout << "Error in \"writeDisassemble(...)\", cannot open file \"" << outputfile << "\"\n";
		return false;
	}	

	int size = textCommands.size();
	for(int i = 0; i < size; i++)
	{
		string str = binaryCommands[i];
		if(textCommands[i].kind == 0)
		{
			fprintf(fpw, "%s %d %d\n", str.c_str(), textCommands[i].address, textCommands[i].first);
			continue;
		}
		fprintf(fpw, "%s %s %s %s %s %s %d ", str.substr(0, 6).c_str(), str.substr(6, 5).c_str(), str.substr(11, 5).c_str(), str.substr(16, 5).c_str(), str.substr(21, 5).c_str(), str.substr(26, 6).c_str(), textCommands[i].address);
		int first = textCommands[i].first;
		int second = textCommands[i].second;
		int third = textCommands[i].third;
		switch(textCommands[i].kind)
		{
			case 1: fprintf(fpw, "SW R%d, %d(R%d)\n", first, second, third);break;
			case 2: fprintf(fpw, "LW R%d, %d(R%d)\n", first, second, third); break;
			case 3: fprintf(fpw, "J #%d\n", first); break;
			case 4: fprintf(fpw, "BEQ R%d, R%d, #%d\n", first, second, third); break;
			case 5: fprintf(fpw, "BNE R%d, R%d, #%d\n", first, second, third); break;
			case 6: fprintf(fpw, "BGEZ R%d, #%d\n", first, second); break;
			case 7: fprintf(fpw, "BGTZ R%d, #%d\n", first, second); break;
			case 8: fprintf(fpw, "BLEZ R%d, #%d\n", first, second); break;
			case 9: fprintf(fpw, "BLTZ R%d, #%d\n", first, second); break;
			case 10: fprintf(fpw, "ADDI R%d, R%d, #%d\n", first, second, third); break;
			case 11: fprintf(fpw, "ADDIU R%d, R%d, #%d\n", first, second, third); break;
			case 12: fprintf(fpw, "SLTI R%d, R%d, #%d\n", first, second, third); break;
			case 13: fprintf(fpw, "BREAK\n");break;
			case 14: fprintf(fpw, "SLL R%d, R%d, #%d\n", first, second, third); break;
			case 15: fprintf(fpw, "SRL R%d, R%d, #%d\n", first, second, third); break;
			case 16: fprintf(fpw, "SRA R%d, R%d, #%d\n", first, second, third); break;
			case 17: fprintf(fpw, "SLT R%d, R%d, R%d\n", first, second, third); break;
			case 18: fprintf(fpw, "SLTU R%d, R%d, R%d\n", first, second, third); break;
			case 19: fprintf(fpw, "SUB R%d, R%d, R%d\n", first, second, third); break;
			case 20: fprintf(fpw, "SUBU R%d, R%d, R%d\n", first, second, third); break;
			case 21: fprintf(fpw, "ADD R%d, R%d, R%d\n", first, second, third); break;
			case 22: fprintf(fpw, "ADDU R%d, R%d, R%d\n", first, second, third); break;
			case 23: fprintf(fpw, "AND R%d, R%d, R%d\n", first, second, third); break;
			case 24: fprintf(fpw, "OR R%d, R%d, R%d\n", first, second, third); break;
			case 25: fprintf(fpw, "XOR R%d, R%d, R%d\n", first, second, third); break;
			case 26: fprintf(fpw, "NOR R%d, R%d, R%d\n", first, second, third); break;
			case 27: fprintf(fpw, "NOP\n");break;
		}
	}	
	fclose(fpw);
	return true;
}


void extendString(string& str)
{
	int size = str.size();
	string prefix;
	if(str[0] == '1')
		prefix = string(32-size, '1');
	else
		prefix = string(32-size, '0');
	str = prefix+str;
	return;	
}
