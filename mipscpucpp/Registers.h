#pragma once
#include <stdint.h>
class Registers
{
public:
	uint32_t theregisters[32] = { 0 }; // added a extra register because of naming disparencies between documentation and arrays;
	uint32_t HI = 0;
	uint32_t LO = 0;
	uint32_t pc = 0;
	uint32_t npc = 0;
	
	Registers();
	~Registers();
};

