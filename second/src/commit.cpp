#include "commit.h"
using namespace std;

void commit()
{
	int size = reorderBuffer.size();
	if(size <= 0)
		return;

	if(reorderBuffer[0].ready || reorderBuffer[0].stage == 3)
	{
		reorderBuffer[0].stage = 4;
		if(reorderBuffer[0].id != -1)
			reservationStation[reorderBuffer[0].id].stage = 4;
	}
}
