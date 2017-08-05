#include "instruction.h"
#include <stdio.h>
#include <iostream>
using namespace std;
#pragma warning(disable:4996)
void instruction::syscall(Registers &reg,Memory & mem) {
	int32_t sdd = (int32_t)reg.theregisters[4];
	switch (reg.theregisters[2])
	{
		case 1:
	//	printf("print_int: %d\n", reg.theregisters[4]);
			std::cout << "print_int: " << (int32_t)sdd << std::endl;
		break;
		case 5: {
			int32_t s;
			printf("Please enter an number:");
			scanf("%d", &s);
			reg.theregisters[2] = s;
				break;
		}
		case 4: {
			char * string = (char *)(mem.memory + 4);
			//printf("%s", string);
			cout << string << endl;

			break;
		}
		case 8: {
			uint8_t lengthsf = 0;
			uint8_t maxval = *(mem.memory + 5);
			char chars = 0;
			while (chars != '\n' && maxval >= lengthsf) {
				chars = getchar();
				if (chars == '\n' || chars == '\0' || chars == EOF) {
					*(mem.memory + 4 + lengthsf) = '\0';
					break;
				}
				*(mem.memory + 4 + lengthsf) = chars;
				lengthsf++;
			
			}
		}
	default:
		/// printf("Error: not supported.\n"); // call cpu_exit
		exitflag = true;
		break;
	}

}