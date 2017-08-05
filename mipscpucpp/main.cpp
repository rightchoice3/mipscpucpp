#include "Registers.h"
#include "instruction.h"
#include "asmdef.h"
#include "cpu.cpp"
void fac_iter()
{
	instruction s;
	CPU d;
	d.InitMemory();
	int code_addr = 0;
	uint32_t *code_segment = (uint32_t *) &(d.mem.memory[CODE_STARTING_ADDRESS]);
	int label1;
	code_segment[code_addr++] = ADDI_INSTRUCTION(T0, ZERO, 7); // N: $T0 = 0 + 7
	code_segment[code_addr++] = ADDI_INSTRUCTION(T1, ZERO, 1); // Running product: $T1 = 1

	label1 = code_addr;  //saving the word offset for this label (only works for backwards jumps)
	code_segment[code_addr++] = BEQ_INSTRUCTION(T0, ZERO, 4); // if ($T0(N)==0) jump 4 instructions ahead of next one
	code_segment[code_addr++] = MULT_INSTRUCTION(T1, T0); // LO = $T1 * $T0
	code_segment[code_addr++] = MFLO_INSTRUCTION(T1);  // $T1 = LO  Running product *= $T0 (N)
	code_segment[code_addr++] = ADDI_INSTRUCTION(T0, T0, -1); //$T0 = $T0-1  (N--)
	code_segment[code_addr++] = J_INSTRUCTION((CODE_STARTING_ADDRESS_IN_WORDS)+label1); //jump to instruction at memory[CODE_STARTING_ADDRESS+(2*4)]

	code_segment[code_addr++] = ADD_INSTRUCTION(A0, ZERO, T1); // A0 = $T1, the result to print
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = $0 + 1, for print_int syscall.
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 10); // $v0 = $0 + 10, for exit syscall.
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;
	d.Execute();
	free(d.mem.memory);
}

//This is a general test of as many instructions (not tested by
//the other programs) as possible.

void instruction_test()
{
	instruction s;
	CPU d;
	d.InitMemory();
	int code_addr = 0;
	uint32_t *code_segment = (uint32_t *) &(d.mem.memory[CODE_STARTING_ADDRESS]);
	uint32_t data_addr = DATA_STARTING_ADDRESS;
	data_addr += 50; //test using location other than the start of the data section
					 //Testing LH and ORI
	uint32_t x_addr = data_addr;
	uint32_t *x_ptr = (uint32_t *) &(d.mem.memory[data_addr]);
	data_addr += 4;  //made room for x

	*x_ptr = 0x110111; // x = 1114385

	code_segment[code_addr++] = LUI_INSTRUCTION(T0, DATA_STARTING_ADDRESS >> 16);  //loads top 16 bits
	code_segment[code_addr++] = ORI_INSTRUCTION(T0, T0, DATA_STARTING_ADDRESS);  //lower 16 bits

																				 // Loads x = 1114385 into A0
	code_segment[code_addr++] = LW_INSTRUCTION(A0, T0, x_addr - DATA_STARTING_ADDRESS);

	//This code prints the int that is in A0
	//SHOULD PRINT 1114385 
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = LUI_INSTRUCTION(T0, DATA_STARTING_ADDRESS >> 16);  //loads top 16 bits
	code_segment[code_addr++] = ORI_INSTRUCTION(T0, T0, DATA_STARTING_ADDRESS);  //lower 16 bits

																				 //Loads bottom half of x, namely 0x111 = 273 into A0
	code_segment[code_addr++] = LH_INSTRUCTION(A0, T0, x_addr - DATA_STARTING_ADDRESS);

	//This code prints the int that is in A0
	//SHOULD PRINT 273
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing LB
	code_segment[code_addr++] = LUI_INSTRUCTION(T0, DATA_STARTING_ADDRESS >> 16);  //loads top 16 bits
	code_segment[code_addr++] = ORI_INSTRUCTION(T0, T0, DATA_STARTING_ADDRESS);  //lower 16 bits

																				 //Loads bottom byte of x, namely 0x11 = 17 into A0
	code_segment[code_addr++] = LB_INSTRUCTION(A0, T0, x_addr - DATA_STARTING_ADDRESS);

	//This code prints the int that is in A0
	//SHOULD PRINT 17
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing NOR, ANDI
	code_segment[code_addr++] = NOR_INSTRUCTION(T1, ZERO, ZERO);  //sets T1 to all 1's

	code_segment[code_addr++] = ANDI_INSTRUCTION(A0, T1, 345);  //since T1 is all 1's, T0 = 345

																//This code prints the int that is in A0
																//SHOULD PRINT 345
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing XORI (Note: A xor B xor A => B)
	code_segment[code_addr++] = ADDI_INSTRUCTION(A0, ZERO, 125);  //A0 = 125
	code_segment[code_addr++] = XORI_INSTRUCTION(A0, A0, 99);  //A0 = A0 XOR 99
	code_segment[code_addr++] = XORI_INSTRUCTION(A0, A0, 125);  //A0 = A0 XOR 125, should be 99

																//This code prints the int that is in A0
																//SHOULD PRINT 99
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SLL, by shifting A0 left by 2. If A0 contains 345, the result should be 1380
	code_segment[code_addr++] = ADDI_INSTRUCTION(A0, ZERO, 345);  //A0 = 345
	code_segment[code_addr++] = SLL_INSTRUCTION(A0, A0, 2);

	//This code prints the int that is in A0
	//SHOULD PRINT 1380
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SRL, by shifting A0 right by 3. Since A0 contains 1380, the result should be 172
	code_segment[code_addr++] = SRL_INSTRUCTION(A0, A0, 3);

	//This code prints the int that is in A0
	//SHOULD PRINT 172
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SUB, and SRA. Put -25 into A0 and shift right by 2. The result should be -7

	code_segment[code_addr++] = ADDI_INSTRUCTION(T2, ZERO, 25);
	code_segment[code_addr++] = SUB_INSTRUCTION(A0, ZERO, T2); //A0 = -25

	code_segment[code_addr++] = SRA_INSTRUCTION(A0, A0, 2); //Now A0 = -7 (rounds down)

															//This code prints the int that is in A0
															//SHOULD PRINT -7
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = ADDI_INSTRUCTION(A0, ZERO, 7); // A0 = 7
	code_segment[code_addr++] = ADDI_INSTRUCTION(T0, ZERO, 3); // T = 3, the shift amount
															   //Testing SLLV, by shifting A0 left by T0 (3). Since A0 contains 7, the result should be 56
	code_segment[code_addr++] = SLLV_INSTRUCTION(A0, A0, T0);

	//This code prints the int that is in A0
	//SHOULD PRINT 56
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SRL, by shifting A0 right by 3. Since A0 contains 56, the result should be 14
	code_segment[code_addr++] = ADDI_INSTRUCTION(T0, ZERO, 2); // T = 2, the shift amount
	code_segment[code_addr++] = SRLV_INSTRUCTION(A0, A0, T0);

	//This code prints the int that is in A0
	//SHOULD PRINT 14
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SUB, and SRAV. Put -25 into A0 and shift right by 2. The result should be -7

	code_segment[code_addr++] = ADDI_INSTRUCTION(T2, ZERO, 25);
	code_segment[code_addr++] = SUB_INSTRUCTION(A0, ZERO, T2); //A0 = -25

	code_segment[code_addr++] = SRAV_INSTRUCTION(A0, A0, T0); //Now A0 = -7 (rounds down)

															  //This code prints the int that is in A0
															  //SHOULD PRINT -7
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;


	//Testing SH, writing 0x0020 to low half-word of x
	//x should then be 0x110020 = 1114144 decimal
	code_segment[code_addr++] = LUI_INSTRUCTION(T0, DATA_STARTING_ADDRESS >> 16);  //loads top 16 bits
	code_segment[code_addr++] = ORI_INSTRUCTION(T0, T0, DATA_STARTING_ADDRESS);  //lower 16 bits

	code_segment[code_addr++] = ADDI_INSTRUCTION(T1, ZERO, 0x20);
	code_segment[code_addr++] = SH_INSTRUCTION(T1, T0, x_addr - DATA_STARTING_ADDRESS);

	//Now loading all of x and printing it.
	code_segment[code_addr++] = LW_INSTRUCTION(A0, T0, x_addr - DATA_STARTING_ADDRESS);

	//This code prints the int that is in A0
	//SHOULD PRINT 1114144
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;


	//Testing SB, writing 0x00 to low byte of x (although the register will contain 0xff20
	//x should then be 0x110000 = 1114112 decimal
	code_segment[code_addr++] = LUI_INSTRUCTION(T0, DATA_STARTING_ADDRESS >> 16);  //loads top 16 bits
	code_segment[code_addr++] = ORI_INSTRUCTION(T0, T0, DATA_STARTING_ADDRESS);  //lower 16 bits

	code_segment[code_addr++] = ADDI_INSTRUCTION(T1, ZERO, 0xff00);
	code_segment[code_addr++] = SB_INSTRUCTION(T1, T0, x_addr - DATA_STARTING_ADDRESS);

	//Now loading all of x and printing it.
	code_segment[code_addr++] = LW_INSTRUCTION(A0, T0, x_addr - DATA_STARTING_ADDRESS);

	//This code prints the int that is in A0
	//SHOULD PRINT 1114112
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;


	//Testing LH and LHU.  Set x = 0x0000ffff and perform LH and then LHU. LH should
	//load a negative number (-1), LHU should load a positive number (65535)

	code_segment[code_addr++] = LUI_INSTRUCTION(T0, DATA_STARTING_ADDRESS >> 16);  //loads top 16 bits
	code_segment[code_addr++] = ORI_INSTRUCTION(T0, T0, DATA_STARTING_ADDRESS);  //lower 16 bits

	code_segment[code_addr++] = ADDI_INSTRUCTION(T3, ZERO, 0xffff); //just enough bits (16) in the constant field

	code_segment[code_addr++] = SW_INSTRUCTION(T3, T0, x_addr - DATA_STARTING_ADDRESS);

	code_segment[code_addr++] = LH_INSTRUCTION(A0, T0, x_addr - DATA_STARTING_ADDRESS);

	//This code prints the int that is in A0
	//SHOULD PRINT -1
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = LUI_INSTRUCTION(T0, DATA_STARTING_ADDRESS >> 16);  //loads top 16 bits
	code_segment[code_addr++] = ORI_INSTRUCTION(T0, T0, DATA_STARTING_ADDRESS);  //lower 16 bits

	code_segment[code_addr++] = LHU_INSTRUCTION(A0, T0, x_addr - DATA_STARTING_ADDRESS);

	//This code prints the int that is in A0
	//SHOULD PRINT 65535
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing LB and LBU.  Set x = 0x000011ff and perform LH and then LHU. LH should
	//load a negative number (-1), LBU should load a positive number (255)

	code_segment[code_addr++] = LUI_INSTRUCTION(T0, DATA_STARTING_ADDRESS >> 16);  //loads top 16 bits
	code_segment[code_addr++] = ORI_INSTRUCTION(T0, T0, DATA_STARTING_ADDRESS);  //lower 16 bits

	code_segment[code_addr++] = ADDI_INSTRUCTION(T3, ZERO, 0x11ff); //just enough bits (16) in the constant field

	code_segment[code_addr++] = SW_INSTRUCTION(T3, T0, x_addr - DATA_STARTING_ADDRESS);

	code_segment[code_addr++] = LB_INSTRUCTION(A0, T0, x_addr - DATA_STARTING_ADDRESS);

	//This code prints the int that is in A0
	//SHOULD PRINT -1
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = LUI_INSTRUCTION(T0, DATA_STARTING_ADDRESS >> 16);  //loads top 16 bits
	code_segment[code_addr++] = ORI_INSTRUCTION(T0, T0, DATA_STARTING_ADDRESS);  //lower 16 bits

	code_segment[code_addr++] = LBU_INSTRUCTION(A0, T0, x_addr - DATA_STARTING_ADDRESS);

	//This code prints the int that is in A0
	//SHOULD PRINT 255
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing LUI.  Load 2 into the upper word of A0, printing 2^17 = 131072

	code_segment[code_addr++] = LUI_INSTRUCTION(A0, 2);

	//This code prints the int that is in A0
	//SHOULD PRINT 131072
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SLT.  Setting A0 = (T5 (-1) < T6(200)) = 1 

	code_segment[code_addr++] = ADDI_INSTRUCTION(T5, ZERO, 0xffff); //sets T5 = -1 (due to sign extension of 0xffff)
	code_segment[code_addr++] = ORI_INSTRUCTION(T6, ZERO, 200);
	code_segment[code_addr++] = SLT_INSTRUCTION(A0, T5, T6);

	//This code prints the int that is in A0
	//SHOULD PRINT 1
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Now reverse order of operands to SLT, should print 0

	code_segment[code_addr++] = SLT_INSTRUCTION(A0, T6, T5);

	//This code prints the int that is in A0
	//SHOULD PRINT 0
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SLTI.  Setting A0 = (T5 (-1) < 200) = 1 

	code_segment[code_addr++] = ADDI_INSTRUCTION(T5, ZERO, 0xffff); //sets T5 = -1 (due to sign extension of 0xffff)
	code_segment[code_addr++] = SLTI_INSTRUCTION(A0, T5, 200);

	//This code prints the int that is in A0
	//SHOULD PRINT 1
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SLTI.  Setting A0 = (T5 (-1) < -2) = 0

	code_segment[code_addr++] = ADDI_INSTRUCTION(T5, ZERO, 0xffff); //sets T5 = -1 (due to sign extension of 0xffff)
	code_segment[code_addr++] = SLTI_INSTRUCTION(A0, T5, 0xfffe); //0xfffe = -2

																  //This code prints the int that is in A0
																  //SHOULD PRINT 0
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SLTIU.  Setting A0 = (T5 (-1) < 200) = 0  (because -1 is interpreted as a large positive number)

	code_segment[code_addr++] = ADDI_INSTRUCTION(T5, ZERO, 0xffff); //sets T5 = -1 (due to sign extension of 0xffff)
	code_segment[code_addr++] = SLTIU_INSTRUCTION(A0, T5, 200);

	//This code prints the int that is in A0
	//SHOULD PRINT 0
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SLTIU.  Setting A0 = (T5 (200) < -1) = 1  (because -1 is interpreted as a large positive number)

	code_segment[code_addr++] = ADDI_INSTRUCTION(T5, ZERO, 200); //sets T5 = 200
	code_segment[code_addr++] = SLTIU_INSTRUCTION(A0, T5, 0xffff); // A0 = (200 < -1) =  1

																   //This code prints the int that is in A0
																   //SHOULD PRINT 1
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing SUBU -- does the same thing as SUB for our purposes (normally doesn't set overflow flag)
	//T5 holds 200, T6 holds 200, so should get 0 as a result.
	code_segment[code_addr++] = SUBU_INSTRUCTION(A0, T5, T6);

	//This code prints the int that is in A0
	//SHOULD PRINT 0
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Test MULT (with large result) and MFHI.

	code_segment[code_addr++] = LUI_INSTRUCTION(T1, 0x4000);  //sets the top 16 bits to 0x4000 (i.e. bit 30 is 1)
	code_segment[code_addr++] = ORI_INSTRUCTION(T1, T1, 10);  //T1 = T1 | 10, SO T1 = 0X40000000 + 10 = 1073741834 (decimal)

	code_segment[code_addr++] = ADD_INSTRUCTION(A0, T1, ZERO);

	//This code prints the int that is in A0
	//SHOULD PRINT 1073741834
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = ADDI_INSTRUCTION(T2, ZERO, 4);  //T2 = 4
	code_segment[code_addr++] = MULT_INSTRUCTION(T1, T2); //HI:LO = T1 * T2,  should set HI to 1 and LO to 40
	code_segment[code_addr++] = MFHI_INSTRUCTION(A0);

	//This code prints the int that is in A0
	//SHOULD PRINT 1
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = MFLO_INSTRUCTION(A0);

	//This code prints the int that is in A0
	//SHOULD PRINT 40
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Test MULTU (with negative number) and MFHI.

	code_segment[code_addr++] = LUI_INSTRUCTION(T1, -1);  //sets the top 16 bits to 0xffff 
	code_segment[code_addr++] = ORI_INSTRUCTION(T1, T1, -1);  //T1 = T1 -1, SO T1 = -1

	code_segment[code_addr++] = ADD_INSTRUCTION(A0, T1, ZERO);

	//This code prints the int that is in A0
	//SHOULD PRINT -1
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = ADDI_INSTRUCTION(T2, ZERO, 2);  //T2 = 2
	code_segment[code_addr++] = MULTU_INSTRUCTION(T1, T2); //HI:LO = T1 * T2,  should set HI to 1, LO to -2
	code_segment[code_addr++] = MFHI_INSTRUCTION(A0);

	//This code prints the int that is in A0
	//SHOULD PRINT 1
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = MFLO_INSTRUCTION(A0);

	//This code prints the int that is in A0
	//SHOULD PRINT -2
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;


	//Testing DIV
	code_segment[code_addr++] = ADDI_INSTRUCTION(T2, ZERO, 237);  //T2 = 237
	code_segment[code_addr++] = ADDI_INSTRUCTION(T1, ZERO, -10);  //T2 = -10
	code_segment[code_addr++] = DIV_INSTRUCTION(T2, T1);  //LO = T2/T1 = -23, HI = T2%T1 = 7

	code_segment[code_addr++] = MFLO_INSTRUCTION(A0); //printing LO, should be -23

													  //This code prints the int that is in A0
													  //SHOULD PRINT -23
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = MFHI_INSTRUCTION(A0); //printing HI, should be 7

													  //This code prints the int that is in A0
													  //SHOULD PRINT 7
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;


	//Testing DIVU

	code_segment[code_addr++] = ADDI_INSTRUCTION(T2, ZERO, 237);  //T2 = 237
	code_segment[code_addr++] = ADDI_INSTRUCTION(T1, ZERO, -10);  //T2 = -10 (which is interpreted as large postive number)
	code_segment[code_addr++] = DIVU_INSTRUCTION(T2, T1);  //LO = T2/T1 = 0, HI = T2%T1 = 237

	code_segment[code_addr++] = MFLO_INSTRUCTION(A0); //printing LO, should be 0

													  //This code prints the int that is in A0
													  //SHOULD PRINT 0
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = MFHI_INSTRUCTION(A0); //printing HI, should be 237

													  //This code prints the int that is in A0
													  //SHOULD PRINT 237
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//Testing AND, OR, and XOR

	code_segment[code_addr++] = ADDI_INSTRUCTION(T1, ZERO, 0xff); //T1 = 0xff
	code_segment[code_addr++] = ADDI_INSTRUCTION(T2, ZERO, 0x34); //T2 = 0x34
	code_segment[code_addr++] = AND_INSTRUCTION(T1, T1, T2); //T1 = T1 & T2 = 0x34
	code_segment[code_addr++] = ADDI_INSTRUCTION(T2, ZERO, 0x70); //NOW T2 = 0x70
	code_segment[code_addr++] = OR_INSTRUCTION(T1, T1, T2); //T1 = T1 OR T2 = 0x74

	code_segment[code_addr++] = ADD_INSTRUCTION(A0, T1, ZERO); //A0 = T1 = 0x74 = 116 for printing

															   //This code prints the int that is in A0
															   //SHOULD PRINT 116
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	code_segment[code_addr++] = ADDI_INSTRUCTION(T2, ZERO, 0xf); //NOW T2 = 0xf 

	code_segment[code_addr++] = XOR_INSTRUCTION(A0, T1, T2); //A0 = T1 ^ T2 = 0x74 ^ 0xf = 0x7n = 123

															 //This code prints the int that is in A0
															 //SHOULD PRINT 123
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 1); // $v0 = 1, for print_int,
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;

	//---- EXIT ------
	code_segment[code_addr++] = ADDI_INSTRUCTION(V0, ZERO, 10); // $v0 = $0 + 10, for exit syscall.
	code_segment[code_addr++] = SYSCALL_INSTRUCTION;
	d.Execute();
	free(d.mem.memory);

}



int main() {
//	uint8_t f[4] = { 0x78,0x56,0x34,0x12};
//	uint32_t * b = (uint32_t *)f;
	

	instruction_test();
	/*	Registers sd = Registers();
		Memory f = Memory(100);
		f.memory[5] = 90;
		f.memory[6] = 100;
		f.memory[7] = 200;
		f.memory[8] = 300;
		f.memory[9] = 400;
	sd.theregisters[5] = 5;
	sd.theregisters[6] = 5;
	s.opcode = 0;
	s.rs = 5;
	s.immediate = 0;
	s.rt = 6;
	s.rd = 7;
	//s.addi(sd);
	s.sw(sd, f); */
	return 0;
}