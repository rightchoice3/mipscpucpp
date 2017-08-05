#include "Memory.h"




Memory::Memory() {

}
Memory::Memory(uint32_t amountofbytes) {
	memory = (uint8_t *)malloc(sizeof(uint8_t) * amountofbytes);
	//for (int i = 0; i < amountofbytes; i++) {
	//	memory[i] = 0;
	// }

}


Memory::~Memory()
{
	// free(memory);
}
