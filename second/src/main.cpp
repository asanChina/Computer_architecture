#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "disassemble.h"
#include "Kind.h"
#include "instructionFetch.h"
#include "decodeIssue.h"
#include "execute.h"
#include "writeResult.h"
#include "commit.h"
#include "writeCycle.h"
#include "init.h"
#include "restore.h"
using namespace std;

void Usage()
{
	cout << "===========================================================================================================\n";
	cout << "==                                    The usage of this program:                                         ==\n";
	cout << "==          MIPSsim inputfilename outputfilename operation [-Tm:n]                                       ==\n";
	cout << "== 1)inputfilename - the file name of the binary input file                                              ==\n";
	cout << "== 2)outputfilename - the filename to which to print the output                                          ==\n";
	cout << "== 3)operation - either \"dis\" or \"sim\" to specify disassembly or simulation.                             ==\n";
	cout << "== 4)-Tm:n - optional argument to specify the start(m) and end(n) cycles of simulation tracing output.   ==\n";
	cout << "===========================================================================================================\n";
	exit(1);
}


int start, end;

bool parse(char* a)
{
	if(a[0] != '-' || a[1] != 'T')
		return false;
	int i = 2;
	start = 0;
	while(a[i] != ':')
	{
		start *= 10;
		start += a[i]-'0';
		i++;
	}		
	int len = strlen(a);
	i++;
	end = 0;
	while(i < len)
	{
		end *= 10;
		end += a[i]-'0';
		i++;
	}
	return true;
}



int main(int argc, char *argv[])
{
	if(argc < 4 || argc > 5)
	{
		cout << "Command error!\n";
		Usage();
	}
	if(argc == 4)
	{
		if(strcmp(argv[3], "dis") == 0)
		{
			if(readBin(argv[1]))
			{
				disassemble();
				writeDisassemble(argv[2]);
			}
		}
		else if(strcmp(argv[3], "sim") == 0)
		{
			init(argv[1]);
			while(!programEnd)
			{
				commit();
				writeResult();
				execute();
				decodeIssue();
				instructionFetch();
				writeCycle(argv[2], cycle);
				restore();
				cycle++;
			}				
			writeLastCycle(argv[2], cycle-1);
		}
		else
		{
			cout << "Command Error\n";
			Usage();
		}
	}
	else
	{
		
		if(strcmp(argv[3], "sim") != 0 || !parse(argv[4]) || start > end)
			Usage();
						
		init(argv[1]);
			while(!programEnd)
			{
				commit();
				writeResult();
				execute();
				decodeIssue();
				instructionFetch();
				if(cycle >= start && cycle <= end)
				writeCycle(argv[2], cycle);
				restore();
				cycle++;
			}
		if(start == 0 && end == 0)
			writeLastCycle(argv[2], cycle-1);
	}
	return 0;
}
