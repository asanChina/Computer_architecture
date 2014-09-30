#include "disassemble.h"
#include <stdio.h>
#include <iostream>
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
		tmp.kind = 27;
	else if(opcode == "101011")//SW
	{
		tmp.kind = 1;
		string rt = str.substr(11, 5);
		string offset = str.substr(16, 16);
		extendString(offset);
		string base = str.substr(6, 5);
		tmp.first = getBinaryValue(rt);
		tmp.second = getBinaryValue(offset);
		tmp.third = getBinaryValue(base);
	}
	else if(opcode == "100011") //LW
	{
		tmp.kind = 2;
		string rt = str.substr(11, 5);
		string offset = str.substr(16, 16);
		extendString(offset);
		string base = str.substr(6, 5);
		tmp.first = getBinaryValue(rt);
		tmp.second = getBinaryValue(offset);
		tmp.third = getBinaryValue(base);
	}
	else if(opcode == "000010") //J
	{
		tmp.kind = 3;
		string ins = str.substr(6, 26);
		int value = getBinaryValue(ins);
		value <<= 2;
		int comp = 0xf0000000;
		int a = (comp&(tmp.address+4));
		int b = (a|value);
		tmp.first = b;		
	}
	else if(opcode == "000100") //BEQ
	{
		tmp.kind = 4;
		string rs = str.substr(6, 5);
		string rt = str.substr(11, 5);
		string offset = str.substr(16, 16);
		offset += "00";
		extendString(offset);
		tmp.first = getBinaryValue(rs);
		tmp.second = getBinaryValue(rt);
		tmp.third =  getBinaryValue(offset);
	}
	else if(opcode == "000101") //BNE
	{
		tmp.kind = 5;
		string rs = str.substr(6, 5);
		string rt = str.substr(11, 5);
		string offset = str.substr(16, 16);
		offset += "00";
		extendString(offset);
		tmp.first = getBinaryValue(rs);
		tmp.second = getBinaryValue(rt);
		tmp.third = getBinaryValue(offset);
	}
	else if(opcode == "000001") //BGEZ or BLTZ
	{
		string secondOpcode = str.substr(11, 5);
		if(secondOpcode == "00001") //BGEZ
		{
			tmp.kind = 6;
			string rs = str.substr(6, 5);
			string offset = str.substr(16, 16);
			offset += "00";
			extendString(offset);
			tmp.first = getBinaryValue(rs);
			tmp.second = getBinaryValue(offset);	
		}
		else //BLTZ
		{
			tmp.kind = 9;
			string rs = str.substr(6, 5);
			string offset = str.substr(16, 16);
			offset += "00";
			extendString(offset);
			tmp.first = getBinaryValue(rs);
			tmp.second = getBinaryValue(offset);
		}
	}
	else if(opcode == "000111") //BGTZ
	{
		tmp.kind = 7;
		string rs = str.substr(6, 5);
		string offset = str.substr(16, 16);
		offset += "00";
		extendString(offset);
		tmp.first = getBinaryValue(rs);
		tmp.second = getBinaryValue(offset);
	}
	else if(opcode == "000110") //BLEZ
	{
		tmp.kind = 8;
		string rs = str.substr(6, 5);	
		string offset = str.substr(16, 16);
		offset += "00";
		extendString(offset);
		tmp.first = getBinaryValue(rs);
		tmp.second = getBinaryValue(offset);
	}
	else if(opcode == "001000") //ADDI
	{
		tmp.kind = 10;
		string rt = str.substr(11, 5);
		string rs = str.substr(6, 5);
		string immediate = str.substr(16, 16);
		extendString(immediate);
		tmp.first = getBinaryValue(rt);
		tmp.second = getBinaryValue(rs);
		tmp.third = getBinaryValue(immediate);
	}
	else if(opcode == "001001") //ADDIU
	{
		tmp.kind = 11;
		string rt = str.substr(11, 5);
		string rs = str.substr(6, 5);
		string immediate = str.substr(16, 16);
		extendString(immediate);
		tmp.first = getBinaryValue(rt);
		tmp.second = getBinaryValue(rs);
		tmp.third = getBinaryValue(immediate);
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
			tmp.kind = 12;
			isData = true;
			dataStartAddress = tmp.address+4;
		}
		else if(sa == "00000" && secondOpcode == "101010")//SLT
		{
			tmp.kind = 13;
			tmp.first = rdvalue;
			tmp.second = rsvalue;
			tmp.third = rtvalue;
		}
		else if(sa == "00000" && secondOpcode == "101011") //SLTU
		{
			tmp.kind = 15;
			tmp.first = rdvalue;
			tmp.second = rsvalue; 
			tmp.third = rtvalue;
		}
		else if(rs == "00000" && secondOpcode == "000000") //SLL
		{
			tmp.kind = 16;
			tmp.first = rdvalue;
			tmp.second = rtvalue;
			tmp.third = savalue;
		}
		else if(rs == "00000" && secondOpcode == "000010") //SRL
		{
			tmp.kind = 17;
			tmp.first = rdvalue;
			tmp.second = rtvalue;
			tmp.third = savalue;
		}
		else if(rs == "00000" && secondOpcode == "000011") //SRA
		{
			tmp.kind = 18;
			tmp.first = rdvalue;
			tmp.second = rtvalue;
			tmp.third = savalue;
		}
		else
		{
			switch(secondOpcodevalue)
			{
				case 34: tmp.kind = 19; break;
				case 35: tmp.kind = 20; break;
				case 32: tmp.kind = 21; break;
				case 33: tmp.kind = 22; break;
				case 36: tmp.kind = 23; break;
				case 37: tmp.kind = 24; break;
				case 38: tmp.kind = 25; break;
				case 39: tmp.kind = 26; break;
			}
			tmp.first = rdvalue;
			tmp.second = rsvalue;
			tmp.third = rtvalue;
		}
	}
	else if(opcode == "001010") // SLTI
	{
		tmp.kind = 14;
		string rt = str.substr(11, 5);
		string rs = str.substr(6, 5);
		string immediate = str.substr(16, 16);
		extendString(immediate);
		tmp.first = getBinaryValue(rt);
		tmp.second = getBinaryValue(rs);
		tmp.third = getBinaryValue(immediate);
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
			case 12: fprintf(fpw, "BREAK\n");break;
			case 13: fprintf(fpw, "SLT R%d, R%d, R%d\n", first, second, third); break;
			case 14: fprintf(fpw, "SLTI R%d, R%d, #%d\n", first, second, third); break;
			case 15: fprintf(fpw, "SLTU R%d, R%d, R%d\n", first, second, third); break;
			case 16: fprintf(fpw, "SLL R%d, R%d, #%d\n", first, second, third); break;
			case 17: fprintf(fpw, "SRL R%d, R%d, #%d\n", first, second, third); break;
			case 18: fprintf(fpw, "SRA R%d, R%d, #%d\n", first, second, third); break;
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
