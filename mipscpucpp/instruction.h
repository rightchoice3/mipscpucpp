#pragma once
#include <stdint.h>
#include "Memory.h"
#include "Registers.h"
#include <stdio.h>
class instruction {
public:
	uint8_t opcode, rs, rt, rd, shamt, funct;
	uint16_t immediate;
	bool exitflag = false;
	int type = 0; // 0 - rinstruction, 1-Iinstruction, 2 - J-instructoon
	uint32_t instructcode;
	uint32_t address;
	uint8_t getOpcode(uint32_t instruction);
	uint8_t getRSField(uint32_t instruction);
	uint8_t getRTField(uint32_t instruction);
	uint8_t getRDField(uint32_t instruction);
	uint8_t getShantField(uint32_t instruction);
	uint8_t getFunctField(uint32_t instruction);
	uint16_t getImmediate(uint32_t instruction);
	uint32_t getAddress(uint32_t instruction);
	void ParseInstruction(uint32_t instruction);
	void j(Registers & reg);
	void jal(Registers & reg);
	void add(Registers &reg);
	void addu(Registers &reg);
	void sub(Registers &reg);
	void mult(Registers &reg);
	void multu(Registers &reg);
	void subu(Registers &reg);
	void ori(Registers & reg);
	void andi(Registers & reg);
	void addi(Registers & reg);
	void addiu(Registers & reg);
	void lw(Registers & reg, Memory & s);
	void lh(Registers & reg, Memory & s);
	void lhu(Registers & reg, Memory & s);
	void lb(Registers & reg, Memory & s);
	void lbu(Registers & reg, Memory & s);
	void sw(Registers & reg, Memory & s);
	void sh(Registers & reg, Memory & s);
	void sb(Registers & reg, Memory & s);
	void beq(Registers & reg);
	void bne(Registers & reg);
	void xori(Registers & reg);
	void div(Registers &reg);
	void divu(Registers &reg);
	void mfhi(Registers &reg);
	void mflo(Registers & reg);
	void and(Registers & reg);
	void or (Registers & reg);
	void xor (Registers & reg);
	void nor(Registers & reg);
	void slt(Registers & reg);
	void slti(Registers & reg);
	void sltiu(Registers & reg);
	void sltu(Registers & reg);
	void sll(Registers &reg);
	void sllv(Registers & reg);
	void srl(Registers & reg);
	void lui(Registers & reg);
	void srlv(Registers & reg);
	void sra(Registers & reg);
	void srav(Registers & reg);
	void jr(Registers & reg);
	void jalr(Registers & reg);
	void syscall(Registers & reg,Memory & mem);
	instruction() {

	}
	 instruction(uint32_t instruction, Registers & s, Memory & m) {
		 instructcode = instruction;
		 ParseInstruction(instruction);
		 if (type == 0) {
			 switch (funct) {
			 case 0x20:
				 add(s);
				 break;
			 case 0x21:
				 addu(s);
				 break;
			 case 0x22:
				 sub(s);
				 break;
			 case 0x23:
				 subu(s);
				 break;
			 case 0x18:
				 mult(s);
				 break;
			 case 0x19:
				 multu(s);
				 break;
			 case 0x1a:
				 div(s);
				 break;
			 case 0x1b:
				 divu(s);
				 break;
			 case 0x10:
				 mfhi(s);
				 break;
			 case 0x12:
				 mflo(s);
				 break;
			 case 0x24:
				 and(s);
				 break;
			 case 0x25:
				 or (s);
				 break;
			 case 0x26:
				 xor (s);
				 break;
			 case 0x27:
				 nor(s);
				 break;
			 case 0x2a:
				 slt(s);
				 break;
			 case 0x2b:
				 sltu(s);
				 break;
			 case 0x00:
				 sll(s);
				 break;
			 case 0x04:
				 sllv(s);
				 break;
			 case 0x06:
				 srlv(s);
				 break;
			 case 0x03:
				 sra(s);
				 break;
			 case 0x02:
				 srl(s);
				 break;
			 case 0x07:
				 srav(s);
				 break;
			 case 0x08:
				 jr(s);
				 break;
			 case 0x09:
				 jalr(s);
				 break;
			 case 0xff:
				 exitflag = true;
				 break;
			 case 0x0c:
				 syscall(s, m);
				 break;
			 default:
			//	 printf("Invalid!\n");
				 exitflag = true;
				 _asm {int 3}
				 break;
			}
		 }
		 if (type == 1) {
			 switch (opcode) {
			 case 0x08:
				 addi(s);
				 break;
			 case 0x09:
				 addiu(s);
				 break;
			 case 0x23:
				 lw(s, m);
				 break;
			 case 0x21:
				 lh(s, m);
				 break;
			 case 0x25:
				 lhu(s, m);
				 break;
			 case 0x20:
				 lb(s, m);
				 break;
			 case 0x24:
				 lbu(s, m);
				 break;
			 case 0x2b:
				 sw(s, m);
				 break;
			 case 0x29:
				 sh(s, m);
				 break;
			 case 0x28:
				 sb(s, m);
				 break;
			 case 0x0f:
				 lui(s);
				 break;
			 case 0x0d:
				 ori(s);
				 break;
			 case 0x0c:
				 andi(s);
				 break;
			 case 0x0e:
				 xori(s);
				 break;
			 case 0x0a:
				 slti(s);
				 break;
			 case 0x0b:
				 sltiu(s);
				 break;
			 case 0x04:
				 beq(s);
 				 break;
			 case 0x05:
				 bne(s);
				 break;
			 default:
			//	 printf("Invalid!\n");
				 exitflag = true;
				 _asm {int 3}
				 break;
			 }
		}
		 if (type == 2) {
			 if (opcode == 0x2) {
				 j(s);
			 }
			 else if (opcode == 0x3){
				 jal(s);
			 }
			 else {
	
			//	 printf("Invalid!\n");
				 exitflag = true;
				 _asm {int 3}
			 }
		 }
	}
};