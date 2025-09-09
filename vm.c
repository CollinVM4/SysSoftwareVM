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

//print function
void print_state(int PC, int BP, int SP) {
    printf("%d %d %d ", PC, BP, SP);

    int i;
    int arb = BP;
    for (i = PAS_SIZE - 1; i >= SP; i--) {
        printf("%d ", pas[i]);
        if (i == BP) {
            printf("| ");
            if (pas[arb] != 0 && pas[arb - 1] != 0 && pas[arb - 2] != 0) {
                 arb = pas[arb - 1]; 
            }
        }
    }
    printf("\n");
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
    //fetch-execute cycle
    instruction ir;
    int halt = 0;

    //print values
    printf("Initial values: %d %d %d\n", PC, BP, SP);
    
    do{
        //fetch cycle
        ir.op = pas[PC];
        ir.l = pas[PC - 1];
        ir.m = pas[PC - 2];
        PC -= 3;
        
        //print instruction before execution
        printf("%s %d %d ", op_mnemonics[ir.op - 1], ir.l, ir.m);

        //execution
        switch(ir.op){
            case 1: //LIT
                SP--;
                pas[SP] = ir.m;
                break;
            case 2: //OPR
                switch(ir.m){
                    case 0://RTN
                        SP = BP + 1;
                        BP = pas[SP - 2];
                        PC = pas[SP - 3];
                        break;
                    case 1://ADD
                        pas[SP + 1] += pas[SP];
                        SP++;
                        break;
                    case 2://SUB
                        pas[SP + 1] -= pas[SP];
                        SP++;
                        break;
                    case 3://MUL
                        pas[SP + 1] *= pas[SP];
                        SP++;
                        break;
                    case 4://DIV
                        pas[SP + 1] /= pas[SP];
                        SP++;
                        break;
                    case 5: //EQL
                        pas[SP + 1] = (pas[SP + 1] == pas[SP]);
                        SP++;
                        break;
                    case 6: //NEQ
                        pas[SP + 1] = (pas[SP + 1] != pas[SP]);
                        SP++;
                        break;
                    case 7://LSS
                        pas[SP + 1] = (pas[SP + 1] < pas[SP]);
                        SP++;
                        break;
                    case 8: //LEQ
                        pas[SP + 1] = (pas[SP + 1] <= pas[SP]);
                        SP++;
                        break;
                    case 9: //GTR
                        pas[SP + 1] = (pas[SP + 1] > pas[SP]);
                        SP++;
                        break;
                    case 10: //GEQ
                        pas[SP + 1] = (pas[SP + 1] >= pas[SP]);
                        SP++;
                        break;
                }
                break;
            case 3: //LOD
                SP--;
                pas[SP] = pas[base(BP, ir.l) - ir.m];
                break;
            case 4: //STO
                pas[base(BP, ir.l) - ir.m] = pas[SP];
                SP++;
                break;
            case 5: //CAL
                pas[SP - 1] = base(BP, ir.l); //SL
                pas[SP - 2] = BP;             //DL
                pas[SP - 3] = PC;             //RA
                BP = SP - 1;
                PC = ir.m;
                break;
            case 6: //INC
                SP -= ir.m;
                break;
            case 7: //JMP
                PC = ir.m;
                break;
            case 8: //JPC
                if (pas[SP] == 0) {
                    PC = ir.m;
                }
                SP++;
                break;
            case 9: //SYS
                switch (ir.m) {
                    case 1: //output
                        printf("Output result is: %d\n", pas[SP]);
                        SP++;
                        break;
                    case 2: //read
                        printf("Please Enter an Integer: ");
                        SP--;
                        (void)scanf("%d", &pas[SP]);
                        break;
                    case 3: //hlt
                        halt = 1;
                        break;
                }
                break;

        }
        print_state(PC, BP, SP);
    }while(!halt);


    return 0;
}
