#include "assert.h"
#include <iostream>
#include <stdlib.h>

void exitIfLess(int first, int second)
{
	if(first < second)
	{
		std::cout << "error in " << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ << std::endl;
		exit(-1);
	}
}
