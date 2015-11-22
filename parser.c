/*	Christopher Beier
	Raymond Cload
	Project 4
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NULSYM "1"
#define IDENTSYM "2"
#define NUMBERSYM "3"
#define PLUSSYM "4"
#define MINUSSYM "5"
#define MULTSYM "6"
#define SLASHSYM "7"
#define ODDSYM "8"
#define EQLSYM "9"
#define NEQSYM "10"
#define LESSYM "11"
#define LEQSYM "12"
#define GTRSYM "13"
#define GEQSYM "14"
#define LPARENTSYM "15"
#define RPARENTSYM "16"
#define COMMASYM "17"
#define SEMICOLONSYM "18"
#define PERIODSYM "19"
#define BECOMESSYM "20"
#define BEGINSYM "21"
#define ENDSYM "22"
#define IFSYM "23"
#define THENSYM "24"
#define WHILESYM "25"
#define DOSYM "26"
#define CALLSYM "27"	
#define CONSTSYM "28"
#define VARSYM "29"
#define PROCSYM "30"
#define WRITESYM "31"
#define READSYM "32"
#define ELSESYM "33"

#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_CODE_SIZE 500

typedef struct Symbol
{
	int kind;
	char name[12];
	int val;
	int level;
	int addr;
	int symAddr;
} Symbol;

typedef struct Instruction
{
	int op, l, m;
} Instruction;

FILE *tokenInput;
char token[15];
int arSize = 0, codeSize = 0, printTokens = 0, toConsole = 0, curIns2;
int curLevel = -1, stackSize = 0, symbolCount = 0;
Symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];
Instruction codeList[MAX_CODE_SIZE];

void ERROR(int n);
void getToken();
int getVal();
void block();
void statement();
void expression();
void condition();
void factor();
void term();
void startProgram();

void addSymbol(int kind, char name[], int val, int level);
void removeSymbol(int symCount, int count);
void addConst(int kind, char name[], int val);

void emitCode();
void addInstruction(int op, int l, int m);
int findSym(char symbol[]);
int symbolType(int tokenId);
int symbolLevel(int tokenId);
int symbolAddress(int tokenId);

int main(int argc, char **argv)
{
	int i;

	for(i = 1; i < argc; i++)
		if(argv[i][1] == 'a')
			toConsole = 1;
		else if(argv[i][1] == 'l')
			printTokens = 1;

	tokenInput = fopen("lexemelist.txt", "r");

	if(!tokenInput)
		return;

	startProgram();

	fclose(tokenInput);

	emitCode();

	printf("\n");

	if(toConsole == 1)
	{
		FILE* ofp = fopen("mcode.txt", "r");

		char c = getc(ofp);
		while( c != EOF)
		{
			printf("%c", c);
			c = getc(ofp);		
		}
		printf("\n");
		fclose(ofp);
	}

	printf("Program is syntactically correct.\n");

	return 0;
}

void ERROR(int n)
{
	if(n == 1)
		printf("ERROR 1, Use = instead of :=.\n");
	else if(n == 2)
		printf("ERROR 2, = must be followed by a number.\n");
	else if(n == 3)
		printf("ERROR 3, Identifier must be followed by =.\n");
	else if(n == 4)
		printf("ERROR 4, const, var, procedure must be followed by identifier.\n");
	else if(n == 5)
		printf("ERROR 5, Semicolon or comma missing.\n");
	else if(n == 6)
		printf("ERROR 6, Incorrect symbol after procedure declaration.\n");
	else if(n == 7)
		printf("ERROR 7, Statement expected.\n");
	else if(n == 8)
		printf("ERROR 8, Incorrect symbol after statement part in block.\n");
	else if(n == 9)
		printf("ERROR 9, Period expected.\n");
	else if(n == 10)
		printf("ERROR 10, Semicolon between statements missing.\n");
	else if(n == 11)
		printf("ERROR 11, %s is an undeclared identifier.\n", token);
	else if(n == 12)
		printf("ERROR 12, Assignment to constant or procedure is not allowed.\n");
	else if(n == 13)
		printf("ERROR 13, Assignment operator expected.\n");
	else if(n == 14)
		printf("ERROR 14, call must be followed by an identifier.\n");
	else if(n == 15)
		printf("ERROR 15, Call of a constant or variable is meaniningless.\n");
	else if(n == 16)
		printf("ERROR 16, then expected.\n");
	else if(n == 17)
		printf("ERROR 17, } expected.\n");
	else if(n == 18)
		printf("ERROR 18, do expected.\n");
	else if(n == 19)
		printf("ERROR 19, Incorrect symbol following statement.\n");
	else if(n == 20)
		printf("ERROR 20, Relational operator expected.\n");
	else if(n == 21)
		printf("ERROR 21, Expression must not contain a procedure identifier.\n");
	else if(n == 22)
		printf("ERROR 22, Right parenthesis missing.\n");
	else if(n == 23)
		printf("ERROR 23, The preceding factor cannot begin with this symbol.\n");
	else if(n == 24)
		printf("ERROR 24, An expression cannot begin with this symbol.\n");
	else if(n == 25)
		printf("ERROR 25, This number is too large.\n");
	else if(n == 26)
		printf("ERROR 26, := expected.\n");
	else if(n == 27)
		printf("ERROR 27, Expected variable, number, (, or missing ;.\n");
	else if(n == 28)
		printf("ERROR 28, %s, end symbol expected.\n", token);
	else if(n == 29)
		printf("ERROR 29, begin symbol expected.\n");
	else if(n == 30)
		printf("ERROR 30, Semicolon missing.\n");
	else if(n == 31)
		printf("ERROR 31, Identifier expected.\n");
	else
		printf("ERROR Placeholder error\n");

	//exit(1);
}

void getToken()
{
	if(fscanf(tokenInput, "%s", token) != EOF)
		if(printTokens)
			printf("%s\n", token);
}

int getVal()
{
	int val;
	fscanf(tokenInput, "%d", &val);
	return val;
}

void startProgram()
{
	getToken();
	block();

	if(strcmp(token, PERIODSYM))
		ERROR(9);
}

//Pretty sure block is finished
void block()
{
	char name[12];
	int space = 4;
	int jmpAddr = codeSize;
	addInstruction(7, 0, 0);
	curLevel++;
	int curSymSize = symbolCount;

	arSize = 4;

	if(!strcmp(token, CONSTSYM))
	{
		while(1)
		{
			getToken();

			if(strcmp(token, IDENTSYM))
				ERROR(4);

			//Grabs the variable
			getToken();

			strcpy(name, token);

			getToken();

			if(strcmp(token, EQLSYM))
				ERROR(3);

			getToken();

			if(strcmp(token, NUMBERSYM))
				ERROR(2);

			addSymbol(1, name, getVal(), 0);

			getToken();

			if(strcmp(token, COMMASYM))
				break;
		}

		if(strcmp(token, SEMICOLONSYM))
			ERROR(5);

		getToken();
	}

	if(!strcmp(token, VARSYM))
	{
		while(1)
		{
			getToken();

			if(strcmp(token, IDENTSYM))
				ERROR(4);

			//Grabs the variable
			getToken();

			addSymbol(2, token, 0, curLevel);

			getToken();

			space++;

			if(strcmp(token, COMMASYM))
				break;
		}

		if(strcmp(token, SEMICOLONSYM))
			ERROR(5);

		getToken();
	}

	while(!strcmp(token, PROCSYM))
	{
		getToken();

		if(strcmp(token, IDENTSYM))
			ERROR(31);

		getToken();

		addSymbol(3, token, codeSize, curLevel);

		getToken();

		if(strcmp(token, SEMICOLONSYM))
			ERROR(30);

		getToken();

		block();

		if(strcmp(token, SEMICOLONSYM))
			ERROR(30);

		getToken();
	}

	codeList[jmpAddr].m = codeSize;
	addInstruction(6, 0, space);

	statement();

	removeSymbol(symbolCount, symbolCount - curSymSize);
	symbolCount = curSymSize;

	arSize -= 4;
	curLevel--;
	addInstruction(2, 0, 0);

	return;
}

//Need to add read/write 
void statement()
{
	if(!strcmp(token, IDENTSYM))
	{
		//gets the variable
		getToken();

		int i = findSym(token);

		if(i == -1)
			ERROR(11);

		if(symbolType(i) != 2)
			ERROR(12);

		getToken();

		if(strcmp(token, BECOMESSYM))
			ERROR(26);

		getToken();

		expression();

		addInstruction(4, curLevel - symbolLevel(i), symbolAddress(i));

		//if(strcmp(token, SEMICOLONSYM))
		//	ERROR(10);
	}

	if(!strcmp(token, CALLSYM))
	{
		getToken();

		if(strcmp(token, IDENTSYM))
			ERROR(0);

		getToken();

		int i = findSym(token);

		if(i == -1)
			ERROR(11);

		if(symbolType(i) != 3)
			ERROR(0);

		addInstruction(5, curLevel - symbolLevel(i), symbolTable[i].val);

		getToken();
	}
	if(!strcmp(token, BEGINSYM))
	{
		getToken();

		statement();

		while(!strcmp(token, SEMICOLONSYM))
		{
			getToken();
			statement();
		}

		if(strcmp(token, ENDSYM))
			ERROR(28);

		getToken();
	}
	
		if(!strcmp(token, IFSYM))
	{

		getToken();

		
		condition();

		int curIns = codeSize;
		addInstruction(8, 0, 0);

		if(strcmp(token, THENSYM))
			ERROR(16);

		getToken();

		statement();

		
		if(!strcmp(token, ELSESYM))
		{			
			int curIns2 = codeSize;
			addInstruction(7, 0, 0);

			codeList[curIns].m = codeSize;


			getToken();

			statement();

			codeList[curIns2].m = codeSize;

		}
		else
		{
			codeList[curIns].m = codeSize;
		}
		

		

		
	}
		
	
	if(!strcmp(token, WHILESYM))
	{
		int cx1 = codeSize;

		getToken();

		condition();

		int cx2 = codeSize;

		int curIns = codeSize;
		addInstruction(8, 0, 0);

		if(strcmp(token, DOSYM))
			ERROR(18);

		getToken();

		statement();

		addInstruction(7, 0, cx1);
		codeList[cx2].m = codeSize;

		codeList[curIns].m = codeSize;
	}

	if(!strcmp(token, READSYM))
	{
		getToken();

		if(strcmp(token, IDENTSYM))
			ERROR(31);

		getToken();

		int i = findSym(token);

		if(i == -1)
			ERROR(11);

		addInstruction(10, 0, 2); //Reads in input

		addInstruction(4, curLevel - symbolLevel(i), symbolAddress(i)); //Stores input


		getToken();	//grabs semicolon
	}

	if(!strcmp(token, WRITESYM))
	{
		getToken();

		if(strcmp(token, IDENTSYM))
			ERROR(31);

		getToken();

		int i = findSym(token);

		if(i == -1)
			ERROR(11);

		if(symbolType(i) == 2)
			addInstruction(3, curLevel - symbolLevel(i), symbolAddress(i));	//Loads output
		else if(symbolType(i) == 1)
			addInstruction(1, 0, symbolTable[i].val);

		addInstruction(9, 0, 1);	//Prints output

		getToken(); //grabs semicolon
	}

	if(!strcmp(token, ENDSYM))
		return;
}

void condition()
{
	int ins;

	if(!strcmp(token, ODDSYM))
	{
		getToken();

		expression();

		addInstruction(2, 0, 6);
	}

	else 
	{
		expression();

		if(!strcmp(token, EQLSYM))
			ins = 8;
		else if(!strcmp(token, NEQSYM))
			ins = 9;
		else if(!strcmp(token, LESSYM))
			ins = 10;
		else if(!strcmp(token, LEQSYM))
			ins = 11;
		else if(!strcmp(token, GTRSYM))
			ins = 12;
		else if(!strcmp(token, GEQSYM))
			ins = 13;
		else
			ERROR(20);

		getToken();

		expression();

		addInstruction(2, 0, ins);
	}
}

void expression()
{
	char addop[12];

	if(!strcmp(token, PLUSSYM) || !strcmp(token, MINUSSYM))
	{
		strcpy(addop, token);
		getToken();
		term();

		if(!strcmp(addop, MINUSSYM))
			addInstruction(2, 0, 1);
	}
	else
		term();

	while(!strcmp(token, PLUSSYM) || !strcmp(token, MINUSSYM))
	{
		strcpy(addop, token);

		getToken();
		term();

		if(!strcmp(addop, PLUSSYM))
			addInstruction(2, 0, 2);
		else
			addInstruction(2, 0, 3);
	}
}

void term()
{
	char mulop[12];

	factor();

	while(!strcmp(token, MULTSYM) || !strcmp(token, SLASHSYM))
	{
		strcpy(mulop, token);

		getToken();
		factor();

		if(!strcmp(mulop, MULTSYM))
			addInstruction(2, 0, 4);
		else
			addInstruction(2, 0, 5);
	}
}

void factor()
{
	if(!strcmp(token, IDENTSYM))
	{
		getToken();
		int i = findSym(token);

		if(i == -1)
			ERROR(0);

		if(symbolType(i) == 2)
			addInstruction(3, curLevel - symbolLevel(i), symbolAddress(i));
		else if(symbolType(i) == 1)
			addInstruction(1, 0, symbolTable[i].val);
		else
			ERROR(0);
	}
	else if(!strcmp(token, NUMBERSYM))
		addInstruction(1, 0 , getVal());

	else if(!strcmp(token, LPARENTSYM))
	{
		getToken();
		expression();
		if(strcmp(token, RPARENTSYM))
			ERROR(17);
	}
	else ERROR(27);

	getToken();
}

void addSymbol(int kind, char name[], int val, int level)
{	
	symbolTable[symbolCount].kind = kind;
	strcpy(symbolTable[symbolCount].name, name);
	symbolTable[symbolCount].val = val;
	symbolTable[symbolCount].level = level;
	symbolTable[symbolCount].addr = arSize;
	symbolTable[symbolCount].symAddr = symbolCount;
	if(kind == 2)
		arSize++;
	symbolCount++;
}

void removeSymbol(int symCount, int count)
{
	int i;

	for(i = symCount - 1; i > symbolCount - count; i--)
	{
		symbolTable[symCount].kind = 0;
		strcpy(symbolTable[symCount].name, "");
		symbolTable[symCount].val = 0;
		symbolTable[symCount].level = 0;
		symbolTable[symCount].addr = 0;
		symbolTable[symCount].symAddr = 0;
	}

}

void emitCode()
{
	int i;
	FILE *codeOutput = fopen("mcode.txt", "w");

	for(i = 0; i < codeSize; i++)
		fprintf(codeOutput, "%d %d %d\n", codeList[i].op, codeList[i].l, codeList[i].m);

	fclose(codeOutput);
}

void addInstruction(int op, int l, int m)
{
	codeList[codeSize].op = op;
	codeList[codeSize].l = l;
	codeList[codeSize].m = m;
	codeSize++;
}


int findSym(char symbol[])
{
	int i;

	for(i = symbolCount - 1; i >= 0; i--)
		if(!strcmp(symbolTable[i].name, symbol))
			return symbolTable[i].symAddr;

	return -1;
}

int symbolType(int position)
{
	return symbolTable[position].kind;
}

int symbolLevel(int position)
{
	return symbolTable[position].level;
}

int symbolAddress(int position)
{
	return symbolTable[position].addr;
}
