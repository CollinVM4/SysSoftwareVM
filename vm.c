/*
Assignment:
vm.c - Implement a P-machine virtual machine

Authors: Collin Van Meter, Jadon Milne

Language: C ( only )

To Compile:
    gcc -O2 -Wall -std=c11 -o vm vm.c

To Execute ( on Eustis ):
    ./vm input.txt

where:
    input.txt is the name of the file containing PM/0 instructions;
    each line has three integers (OP, L, M)

Notes:
    - Implements the PM/0 virtual machine described in the homework
    instructions.
    - No dynamic memory allocation or pointer arithmetic.
    - Does not implement any VM instruction using a separate function.
    - Runs on Eustis.

Class: COP3402 - Systems Software - Fall 2025

Instructor: Dr. Jie Lin

Due Date: Friday, September 12th, 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants 
#define PAS_SIZE 500 // as defined by section 3 (instructions file)

// global program address space
int pas[PAS_SIZE] = {0};

// Instruction Register (IR)
typedef struct instruction {
    int op;
    int l;
    int m;
} instruction;

const char* op_mnemonics[] = {"LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SYS"};

// this is written by professor
/* Find base L levels down from the current activation record */
int base(int BP, int L) 
{
    int arb = BP;          // activation record base
    while (L > 0) 
    {
        arb = pas[arb];    // follow static link
        L--;
    }
    return arb;
}


int main(int argc, char *argv[]) 
{

    // open input file
    FILE *input = fopen(argv[1], "r");
    if (!input) 
    {
        perror("error w/ input file");
        return 1;
    }

    // variables for reading instructions
    int op; // operation code
    int L;  // level
    int M;  // modifier
    int addr = PAS_SIZE - 1;
    int instructionCount = 0;
    int lowestUsed = PAS_SIZE;    // last "M" loaded (track to set SP later)

    // read from file, load instructions into PAS
    while (fscanf(input, "%d %d %d", &op, &L, &M) == 3) 
    {
        pas[addr--] = op;  // OP
        pas[addr--] = L;   // L
        pas[addr--] = M;   // M
        instructionCount++;

        if (addr + 1 < lowestUsed) lowestUsed = addr + 1; // last written M address
    }
    fclose(input);

    // init registers per assignment details in section 3:
    int PC = PAS_SIZE - 1;   
    int SP = lowestUsed;    
    int BP = SP - 1;


    // further execution loop will go under here


    return 0;
}
