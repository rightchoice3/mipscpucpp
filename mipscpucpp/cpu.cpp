#pragma once
#include "instruction.h"
#include "Memory.h"
#include "Registers.h"
#define MEMORY_SIZE_IN_BYTES (1 << 20)

#define CODE_STARTING_ADDRESS 100
#define DATA_STARTING_ADDRESS (1 << 16)

#define STACK_STARTING_ADDRESS (MEMORY_SIZE_IN_BYTES - 4)  //highest full word address

#define CODE_STARTING_ADDRESS_IN_WORDS (CODE_STARTING_ADDRESS >> 2)
class CPU {
public:
	Registers reg;
	Memory mem;
	instruction b;
	void InitMemory() {
		mem = Memory(MEMORY_SIZE_IN_BYTES);
	}
	void Execute() {
		uint32_t instructions;
		uint8_t arr[4];
		reg.pc = CODE_STARTING_ADDRESS;
		reg.npc = CODE_STARTING_ADDRESS;
		while (reg.npc < 0 || reg.npc < MEMORY_SIZE_IN_BYTES) {
			reg.pc = reg.npc;
			reg.npc = reg.pc + 4;
			/* arr[3] = mem.memory[reg.pc];
			arr[2] = mem.memory[reg.pc+1];
			arr[1] = mem.memory[reg.pc+2];
			arr[0] = mem.memory[reg.pc+3];
			instructions = *(uint32_t *)arr; */
			instructions = *(uint32_t *)&(mem.memory[reg.pc]);
			b= instruction(instructions, reg, mem);
			if (b.exitflag) {
				break;
			}
		}
	//	free(mem.memory);
	}

	void movchartoCode(uint8_t * sr, int  length) { // 1-counted length
		int b = CODE_STARTING_ADDRESS;
		for (int i = 0; i < length; i++) {
			mem.memory[b] = sr[i];
			b++;
		}
	}
	
};