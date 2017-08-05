#pragma once
#include <stdint.h>
#include <stdlib.h>
class Memory
{
public:
	uint8_t * memory;
	Memory();
	Memory(uint32_t amountofbytes);
	~Memory();
};

