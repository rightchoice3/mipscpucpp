#include "Registers.h"
#include <stdlib.h>


Registers::Registers()
{
	for (int i = 0; i < 32; i++) {
		theregisters[i] = 0;
	}
	HI = LO = pc = npc = 0;
}


Registers::~Registers()
{
}
