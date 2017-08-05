#include <stdint.h>
#include "instruction.h"

uint8_t instruction::getOpcode(uint32_t instruction) { // 1st

	return (uint8_t)(instruction >> 26);
}
uint8_t instruction::getRSField(uint32_t instruction) { //2nd
	instruction &= 0x3e00000; // Isolating the RS Field
	return (uint8_t)(instruction >> 21);

}
uint8_t instruction::getRTField(uint32_t instruction) { // 3rd
	instruction &= 0x1f0000; // Isolating the RT Field
	return (uint8_t)(instruction >> 16);

}
uint8_t instruction::getRDField(uint32_t instruction) { //4th
	instruction &= 0xf800; // Isolating the RD Field
	return (uint8_t)(instruction >> 11);

}
uint8_t instruction::getShantField(uint32_t instruction) { //5th
	instruction &= 0x7C0; // Isolating the Shant Field
	return (uint8_t)(instruction >> 6);

}
uint8_t instruction::getFunctField(uint32_t instruction) { //6th
	instruction &= 0x3f; // Isolating the Funct Field
	return (uint8_t)instruction;

}
uint16_t instruction::getImmediate(uint32_t instruction) {
	return (uint16_t)instruction & 0xFFFF;
}
uint32_t instruction::getAddress(uint32_t instruction) {
	return (uint32_t)instruction & 0x3ffffff;
}

void instruction::ParseInstruction(uint32_t instruction) {
	instructcode = instruction;
	opcode = getOpcode(instruction);
	if (opcode == 0) {
		type = 0;
		rs = getRSField(instruction);
		rt = getRTField(instruction);
		rd = getRDField(instruction);
		shamt = getShantField(instruction);
		funct = getFunctField(instruction);
	}
	 else if (opcode == 0x2 || opcode == 0x3) {
		 type = 2;
		 address = getAddress(instruction);
	 }
	 else {
		 type = 1;
		 rs = getRSField(instruction);
		 rt = getRTField(instruction);
		 immediate = getImmediate(instruction);
	 
	 }

	return;
}
void instruction::j(Registers &reg) {
	reg.npc = ((reg.pc + 4) & 0xf0000000) | (address << 2);
}
void instruction::jal(Registers & reg) {
	reg.theregisters[31] = reg.pc + 4;
	reg.npc = ((reg.pc + 4) & 0xf0000000) | (address << 2);

}
void instruction::mult(Registers &reg) {
	int32_t b = reg.theregisters[rs];
	int32_t sh = reg.theregisters[rt];
	int32_t h = 0;;
	int32_t l = 0;
	//uint32_t here = 0;
	_asm {
		push eax
		push ecx
		mov ecx, sh
		mov eax, b
		imul ecx
		mov h,edx
		mov l,eax
		pop ecx
		pop eax
	}
	reg.HI = h;
	reg.LO = l;
	// reg.theregisters[rd] = b;
}
void instruction::multu(Registers &reg) {
	uint32_t b = reg.theregisters[rs];
	uint32_t sh = reg.theregisters[rt];
	uint32_t h = 0;;
	uint32_t l = 0;
	//uint32_t here = 0;
	_asm {
		push eax
		push ecx
		mov ecx, sh
		mov eax, b
		mul ecx
		mov h, edx
		mov l, eax
		pop ecx
		pop eax
	}
	reg.HI = h;
	reg.LO = l;
	//reg.theregisters[rd] = b;
}
void instruction::add(Registers &reg) {
	int32_t  v1 = (int32_t)reg.theregisters[rs];
	int32_t  v2 = (int32_t)reg.theregisters[rt];
	int32_t b = v1 + v2;
	reg.theregisters[rd] = *(uint32_t *)&b;

}
void instruction::addu(Registers &reg) {
	uint32_t  v1 =reg.theregisters[rs];
	uint32_t  v2 = reg.theregisters[rt];
	reg.theregisters[rd] = (v1 + v2);

}
void instruction::sub(Registers &reg) {
	int32_t  v1 = (int32_t)reg.theregisters[rs];
	int32_t  v2 = (int32_t)reg.theregisters[rt];
	int32_t b = v1 - v2;
	reg.theregisters[rd] = *(uint32_t *)&b;

}
void instruction::subu(Registers &reg) {
	uint32_t  v1 = reg.theregisters[rs];
	uint32_t  v2 = reg.theregisters[rt];
	reg.theregisters[rd] = (v1 - v2);

}
uint32_t ExtendS(uint16_t s) {
	uint32_t rets;
	_asm {
		push eax
		movsx eax,s
		mov rets,eax
		pop eax
	}
	return rets;
}
uint16_t ExtendS8(uint8_t s) {
	uint16_t rets;
	_asm {
		push eax
		movsx ax, s
		mov rets, ax
		pop eax
	}
	return rets;
}
uint32_t ExtendU(uint16_t s) {
	uint32_t rets;
	_asm {
		push eax
		movzx eax, s
		mov rets, eax
		pop eax
	}
	return rets;
}
void instruction::ori(Registers &reg) {
	reg.theregisters[rt] = reg.theregisters[rs] | (uint32_t)immediate;
}
void instruction::andi(Registers &reg) {
	reg.theregisters[rt] = reg.theregisters[rs] & (uint32_t)immediate;
}
void instruction::addi(Registers &reg) {
	reg.theregisters[rt] = reg.theregisters[rs] + ExtendS(immediate);
}
void instruction::addiu(Registers &reg) {
	reg.theregisters[rt] = reg.theregisters[rs] + ExtendS(immediate);
}
void instruction::lw(Registers &reg,Memory & s) {
	uint16_t sd;
	//uint8_t arr[4];
//	arr[3] = s.memory[reg.theregisters[rs] + ExtendS(immediate)];
//	arr[2] = s.memory[reg.theregisters[rs] + ExtendS(immediate) + 1];
	//arr[1] = s.memory[reg.theregisters[rs] + ExtendS(immediate) + 2];
//	arr[0] = s.memory[reg.theregisters[rs] + ExtendS(immediate) + 3];
	uint32_t * bb = (uint32_t *) &(s.memory[reg.theregisters[rs] + ExtendS(immediate)]);
	reg.theregisters[rt] = *bb;
	return;
	//sd = s.memory[reg.theregisters[rs] + ExtendS(immediate)];
	//sd = sd & 0x00FF;
	//sd = sd | ( s.memory[reg.theregisters[rs] + ExtendS(immediate) + 1] << 8);
	// reg.theregisters[rt] = s.memory[reg.theregisters[rs] + ExtendS(immediate)];
}
void instruction::lh(Registers &reg, Memory & s) {
	//uint8_t arr[2];
	//arr[1] = s.memory[reg.theregisters[rs] + ExtendS(immediate)];
	//arr[0] = s.memory[reg.theregisters[rs] + ExtendS(immediate) + 1];
	uint16_t * bbd = (uint16_t *) &(s.memory[reg.theregisters[rs] + ExtendS(immediate)]);

	reg.theregisters[rt] = ExtendS(*bbd);
	return;

	// reg.theregisters[rt] = ExtendS(s.memory[reg.theregisters[rs] + ExtendS(immediate)]);
}
void instruction::lhu(Registers &reg, Memory & s) {
	//uint8_t arr[2];
	//arr[1] = s.memory[reg.theregisters[rs] + ExtendS(immediate)];
//	arr[0] = s.memory[reg.theregisters[rs] + ExtendS(immediate) + 1];
	uint16_t * bbd = (uint16_t *) &(s.memory[reg.theregisters[rs] + ExtendS(immediate)]);
	*bbd = *bbd & 0x00FF;
	reg.theregisters[rt] = *bbd;
	uint8_t b, d;
	reg.theregisters[rt] = s.memory[reg.theregisters[rs]];
}
void instruction::lb(Registers &reg, Memory & s) {
	reg.theregisters[rt] = ExtendS(ExtendS8(s.memory[reg.theregisters[rs] + ExtendS(immediate)]));
}
void instruction::lbu(Registers &reg, Memory & s) {
	reg.theregisters[rt] = s.memory[reg.theregisters[rs] + ExtendS(immediate)];
	reg.theregisters[rt] = reg.theregisters[rt] & 0x000000FF;
}
void instruction::sw(Registers &reg, Memory & s) {
	uint8_t first, second, th, fo;
	first = (reg.theregisters[rt] & 0xFF000000) >> 32;
	second = (reg.theregisters[rt] & 0x00FF0000) >> 16;
	th = (reg.theregisters[rt] & 0x0000FF00) >> 8;
	fo = reg.theregisters[rt] & 0x000000FF;
	s.memory[reg.theregisters[rs] + ExtendS(immediate)] = first;
	s.memory[reg.theregisters[rs] + ExtendS(immediate) + 1] = second;
	s.memory[reg.theregisters[rs] + ExtendS(immediate) + 2] = th;
	s.memory[reg.theregisters[rs] + ExtendS(immediate) + 3] = fo;

}
void instruction::sh(Registers &reg, Memory & s) {
	uint8_t first, second;
	first = (reg.theregisters[rt] & 0x0000FF00) >> 8;
	second = reg.theregisters[rt] & 0x000000FF;
	s.memory[reg.theregisters[rs] + ExtendS(immediate)] = first;
	s.memory[reg.theregisters[rs] + ExtendS(immediate) + 1] = second;
}
void instruction::sb(Registers &reg, Memory & s) {
	s.memory[reg.theregisters[rs] + ExtendS(immediate)] = reg.theregisters[rt] && 0x000000FF;
}
void instruction::beq(Registers &reg) {
	if (reg.theregisters[rs] == reg.theregisters[rt])
		reg.npc = reg.pc + 4 + (ExtendS(immediate) << 2);
}
void instruction::bne(Registers &reg) {
	if (reg.theregisters[rs] != reg.theregisters[rt])
		reg.npc = reg.pc + 4 + (ExtendS(immediate) << 2);
}
void instruction::xori(Registers &reg) {
	reg.theregisters[rt] = reg.theregisters[rs] ^ (uint32_t)immediate;
}
void instruction::div(Registers &reg) {
	int32_t  v1 = (int32_t)reg.theregisters[rs];
	int32_t  v2 = (int32_t)reg.theregisters[rt];
	int32_t a = v1 / v2;
	int32_t b = v1 % v2;
	reg.LO = *(uint32_t *)&a;
	reg.HI = *(uint32_t *)&b;

}
void instruction::divu(Registers &reg) {
	uint32_t  v1 = reg.theregisters[rs];
	uint32_t  v2 = reg.theregisters[rt];
	reg.LO = (v1 / v2);
	reg.HI = (v1 % v2);

}
void instruction::mfhi(Registers &reg) {
	reg.theregisters[rd] = reg.HI;


}
void instruction::mflo(Registers &reg) {
	reg.theregisters[rd] = reg.LO;
}
void instruction::and(Registers &reg) {
	reg.theregisters[rd] = reg.theregisters[rs] & reg.theregisters[rt];
}
void instruction::or(Registers &reg) {
	reg.theregisters[rd] = reg.theregisters[rs] | reg.theregisters[rt];
}
void instruction::xor(Registers &reg) {
	reg.theregisters[rd] = reg.theregisters[rs] ^ reg.theregisters[rt];
}
void instruction::nor(Registers &reg) {
	reg.theregisters[rd] = ~(reg.theregisters[rs] | reg.theregisters[rt]);
}
void instruction::slt(Registers &reg) {
	int32_t  v1 = *(int32_t *)&(reg.theregisters[rs]);
	int32_t  v2 = *(int32_t *)&(reg.theregisters[rt]);
	if (v1 < v2) {
		reg.theregisters[rd] = 1;
	}
	else {
		reg.theregisters[rd] = 0;
	}
}
void instruction::slti(Registers &reg) {
	if (reg.theregisters[rs] < ExtendS(immediate)) {
		reg.theregisters[rt] = 1;
	}
	else {
		reg.theregisters[rt] = 0;
	}


}
void instruction::sltiu(Registers &reg) {
	if (reg.theregisters[rs] < ExtendU(immediate)) {
		reg.theregisters[rt] = 1;
	}
	else {
		reg.theregisters[rt] = 0;
	}


}
void instruction::sltu(Registers &reg) {
	if (reg.theregisters[rs] < reg.theregisters[rt]) {
		reg.theregisters[rd] = 1;
	}
	else {
		reg.theregisters[rd] = 0;
	}


}
void instruction::sll(Registers &reg) {
	 reg.theregisters[rd] = reg.theregisters[rt] << shamt;
	
}
void instruction::sllv(Registers &reg) {
	reg.theregisters[rd] = reg.theregisters[rs] << reg.theregisters[rt];


}
// shift right log
void instruction::srl(Registers &reg) {
	uint32_t b = reg.theregisters[rt];
	uint32_t sh = shamt;
	//uint32_t here = 0;
	_asm {
		push eax
		push ecx
		mov ecx, sh
		mov eax, b
		shr eax, cl
		mov b, eax
		pop ecx
		pop eax
	}
	reg.theregisters[rd] = b;

}
void instruction::lui(Registers &reg) {
	reg.theregisters[rt] = ((uint32_t)immediate << 16);
}
void instruction::srlv(Registers &reg) {
	uint32_t b = reg.theregisters[rs];
	uint32_t sh = reg.theregisters[rt];
	//uint32_t here = 0;
	_asm {
		push eax
		push ecx
		mov ecx, sh
		mov eax, b
		shr eax, cl
		mov b, eax
		pop ecx
		pop eax
	}
	reg.theregisters[rd] = b;

}
// shift right arithmetic
void instruction::sra(Registers &reg) {	
	uint32_t b = reg.theregisters[rt];
	uint32_t sh = shamt;
	//uint32_t here = 0;
	_asm {
		push eax
		push ecx
		mov ecx, sh
		mov eax, b
		sar eax, cl
		mov b,eax
		pop ecx
		pop eax
	}
	reg.theregisters[rd] = b;
	// reg.theregisters[rd] = reg.theregisters[rt] >> shamt; //figure it out (OR?)

}
void instruction::srav(Registers &reg) {
	uint32_t b = reg.theregisters[rs];
	uint32_t sh = reg.theregisters[rt];
	//uint32_t here = 0;
	_asm {
		push eax
		push ecx
		mov ecx, sh
		mov eax, b
		sar eax, cl
		mov b, eax
		pop ecx
		pop eax
	}
	reg.theregisters[rd] = b;
	//reg.theregisters[rd] = reg.theregisters[rs] >> reg.theregisters[rt]; // figure it out (OR?)

}
void instruction::jr(Registers &reg) {
	reg.npc = reg.theregisters[rs];

}
void instruction::jalr(Registers &reg) {
	reg.theregisters[31] = reg.pc + 4;
	reg.npc = reg.theregisters[rs];
}





