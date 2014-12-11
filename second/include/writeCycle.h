#ifndef _WRITE_CYCLE_H_
#define _WRITE_CYCLE_H_

#include "global.h"
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>

void writeCycle(char* outputfile, int cycle);
void writeTextCommand(FILE *fp, int index);
void writeLastCycle(char* outputfile, int cycle);

#endif
