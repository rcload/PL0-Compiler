//Raymond Cload
//PL/0 Virtual Machine
//Due 9/7/2014

#include <stdlib.h>
#include <stdio.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

int sp = 0;
int bp = 1;
int pc = 0; 
int halt = 1;

int numAR = 0;
int ar[MAX_LEXI_LEVELS];

char *opcodes[12] = { "", "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SIO", "SIO", "SIO" };
//opcodes[1] = "LIT";
//opcodes[2] = "OPR";
//opcodes[3] = "LOD";
//opcodes[4] = "STO";
//opcodes[5] = "CAL";
//opcodes[6] = "INC";
//opcodes[7] = "JMP";
//opcodes[8] = "JPC";
//opcodes[9] = "SIO";
//opcodes[10] = "SIO";
//opcodes[11] = "SIO";

int stack[MAX_STACK_HEIGHT];


typedef struct instruction {
	int op;
	int l;
	int m;
} instruction;

int numInst = 0;
instruction program[MAX_CODE_LENGTH];
instruction ir;

int base(l, base) // l stand for L in the instruction format
{  
  int b1; //find base L levels down
  b1 = base; 
  while (l > 0)
  {
    b1 = stack[b1 + 1];
    l--;
  }

  return b1;
}



LIT(int l, int m)
{
	sp++;
	stack[sp] = m;
}

OPR(int l, int m)
{
	switch( m )
	{
		case 0 :
			sp = bp - 1;
			pc = stack[sp + 4];
			bp = stack[sp + 3];
			numAR--;
			break;
		case 1 :
			stack[sp] = -1 * stack[sp];
			break;
		case 2 :
			sp--;
			stack[sp] = stack[sp] + stack[sp+1];
			break;
		case 3 :
			sp--;
			stack[sp] = stack[sp] - stack[sp+1];
			break;
		case 4 :
			sp--;
			stack[sp] = stack[sp] * stack[sp+1];
			break;
		case 5 :
			sp--;
			stack[sp] = stack[sp] / stack[sp+1];
			break;
		case 6 :
			stack[sp] = stack[sp] % 2;
			break;
		case 7 :
			sp--;
			stack[sp] = stack[sp] % stack[sp + 1];
			break;
		case 8 :
			sp--;
			stack[sp] = (stack[sp] == stack[sp + 1]);
			break;
		case 9 :
			sp--;
			stack[sp] = (stack[sp] != stack[sp + 1]);
			break;
		case 10 :
			sp--;
			stack[sp] = (stack[sp] < stack[sp + 1]);
			break;
		case 11 :
			sp--;
			stack[sp] = (stack[sp] <= stack[sp + 1]);
			break;
		case 12 :
			sp--;
			stack[sp] = (stack[sp] > stack[sp + 1]);
			break;
		case 13 :
			sp--;
			stack[sp] = (stack[sp] >= stack[sp + 1]);
			break;
			
		default :
			break;
	}
}

LOD(int l, int m)
{
	sp++;
	stack[sp] = stack[ base(l, bp) + m ];
}

STO(int l, int m)
{
	stack[ base(l, bp) + m ] = stack[sp];
	sp--;
}
CAL(int l, int m)
{
	stack[sp+1] = 0;
	stack[sp+2] = base(l, bp);
	stack[sp+3] = bp;
	stack[sp+4] = pc;
	bp = sp + 1;
	pc = m;
	
	//Handle ar division
	ar[numAR] = sp;
	numAR++;
	
}

INC(int l, int m)
{
	sp = sp + m;
}

JMP(int l, int m)
{
	pc = m;
}

JPC(int l, int m)
{
	if( stack[sp] == 0)
		pc = m;
	sp--;
}

SIO(int l, int m)
{
	switch( m )
	{
		case 1 :
			printf("%d\n", stack[sp]);
			sp--;
			break;
		case 2 :
			sp++;
			scanf("%d", &stack[sp]);
			break;
		case 3 :
			halt = 0;
			break;
			
		default 	: 
			break;
	}
}


//Fetch cycle
void fetch()
{
	ir = program[pc];
	pc++;
}

//Execute cycle
void execute(FILE *ofp)
{
	fprintf(ofp, "%d\t%s\t%d\t%d\t", pc-1, opcodes[ir.op], ir.l, ir.m);
	switch( ir.op )
	{
		case 1 :
			LIT(ir.l, ir.m);
			break;
		case 2 :
			OPR(ir.l, ir.m);
			break;
		case 3 :
			LOD(ir.l, ir.m);
			break;
		case 4 :
			STO(ir.l, ir.m);
			break;
		case 5 :
			CAL(ir.l, ir.m);
			break;
		case 6 :
			INC(ir.l, ir.m);
			break;
		case 7 :
			JMP(ir.l, ir.m);
			break;
		case 8 :
			JPC(ir.l, ir.m);
			break;
		case 9 :
			SIO(ir.l, ir.m);
			break;
		case 10 :
			SIO(ir.l, ir.m);
			break;
		case 11 :
			SIO(ir.l, ir.m);
			break;
		
		default 	: 
			break;
	}
	fprintf(ofp, "%d\t%d\t%d\t", pc, bp, sp);
	
	int i = 1;
	int countAR = 0;
	for(i = 1; i <= sp; i++)
	{
		if(countAR < numAR && ar[countAR] < i)
		{	
			countAR++;
			fprintf(ofp, " |");
		}
		
		fprintf(ofp, " %d", stack[i]);
	}
	
	fprintf(ofp, "\n");
	
}


//Take an input file and converts into an array of instructions
void parseInput(FILE *ifp, FILE *ofp)
{
	fprintf(ofp, "line\tOP\tL\tM\n");

	while ( !feof(ifp) )
	{
		instruction inst;
		int op, l, m;
		
		if( fscanf(ifp, "%d %d %d", &op, &l, &m) < 3)
			return;
		
		inst.op = op;
		inst.l = l;
		inst.m = m;

		program[numInst] = inst;
		
		fprintf(ofp, "%d\t%s\t%d\t%d\n", numInst, opcodes[program[numInst].op], program[numInst].l, program[numInst].m);		
		
		numInst++;
	}
}

int main(int argc, char **argv)
{
	FILE *ifp = fopen("mcode.txt", "r");
	FILE *ofp = fopen("stacktrace.txt", "w");

	if(!ifp)
		return;

	int toConsole = 0;

	if(argc > 1 && argv[1][1] == 'v')
		toConsole = 1;

	parseInput(ifp, ofp);

	fprintf(ofp, "\n\t\t\t\tPC\tBP\tSP\tstack\n");

	fprintf(ofp, "Initial values\t\t\t%d\t%d\t%d\n", pc, bp, sp);

	while(halt == 1 && bp != 0)
	{	
		fetch();
		execute(ofp);
	}
	
	fclose(ifp);
	fclose(ofp);

	if(toConsole == 1)
	{
		ofp = fopen("stacktrace.txt", "r");

		char c = getc(ofp);
		while( c != EOF)
		{
			printf("%c", c);
			c = getc(ofp);		
		}
		printf("\n");
		fclose(ofp);
	}
	
	return(0);
}
