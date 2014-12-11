#include "global.h"
using namespace std;


int programStartAddress = 600;
vector<string> binaryCommands;
vector<Cmd> textCommands;
int dataStartAddress;


int PC = 600;//PC will denote which command we are going to do
vector<instructionEntry> instructionQueue;
vector<Entry> reservationStation;
int RSslots = 10;
int ROBslots = 6;
vector<Entry> reorderBuffer;
vector<btb> branchTargetBuffer;

int registers[32] = {0};
int tempRegisters[32] = {0};
vector<int> memory;

bool programEnd = false;
int cycle = 1;
