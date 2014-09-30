#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "disassemble.h"
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
			cout << "Currently not implement simulator\n";
		}
		else
		{
			cout << "Command Error\n";
			Usage();
		}
	}
	else
		cout << "Currently not implement simulator\n";	
}
