#ifndef _DISASSEMBLE_H_
#define _DISASSEMBLE_H_

#include "global.h"

int getBinaryValue(std::string str);
void disassemble();
void disassembleCommand(int index, Cmd& tmp, bool& isData);
bool readBin(char *inputfile);
bool writeDisassemble(char* outputfile);
void extendString(std::string& str);


#endif
