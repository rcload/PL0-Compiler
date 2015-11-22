/*	Christopher Beier
	c2718896
	Project 2 - Scanner	
*/

#include <stdio.h>
#include <string.h>

#define MAX_INT_SIZE 32767

int myPow(int n, int p)
{
	if(p == 0)
		return 1;

	return n * myPow(n, p - 1);
}

int main()
{
	int i, j, validInput = 1, end = 0, sum, commCount = 0;
	char c, string[15];

	FILE *ifp = fopen("input.txt", "r");

	if(!ifp)
		return;

	FILE *cleanInput = fopen("cleaninput.txt", "w");
	FILE *tableOut = fopen("lexemetable.txt", "w");
	FILE *listOut = fopen("lexemelist.txt", "w");

	fprintf(tableOut, "lexeme\ttoken type\n");

	while(fscanf(ifp, "%c", &c) != EOF && validInput && !end)
	{
		//Weeds out sections of comments
		if(c == '/')
		{		
			fscanf(ifp, "%c", &c);

			if(c == '*')
			{
				commCount++;

				while(commCount)
				{
					fscanf(ifp, "%c", &c);	

					if(c == '*')
					{

						fscanf(ifp, "%c", &c);

						if(c == '/')
						{
							commCount--;
							fscanf(ifp, "%c", &c);
						}

					}
					else if(c == '/')
					{
						fscanf(ifp, "%c", &c);

						if(c == '*')
						{
							commCount++;
							fscanf(ifp, "%c", &c);
						}
					}
				}
			}
			//Special case for the division symbol
			else
			{
				fprintf(cleanInput, "/");
				fprintf(tableOut, "/\t\t7\n");
				fprintf(listOut, "7 ");
			}
		}

		//Reads in and tokenizes strings and variables
		if((c >= 97 && c <= 122) || (c >= 65 && c <= 90))
		{
			i = 0;

			while(i < 20 && !end && ((c >= 97 && c <= 122) || (c >= 65 && c <= 90) 
												|| (c >= 48 && c <= 57)))
			{
				string[i] = c;
				string[i + 1] = '\0';
				i++;

				//Protects from segfaults if the file ends with a letter or number
				if(fscanf(ifp, "%c", &c) == EOF)
					end = 1;
			}

			fprintf(cleanInput, "%s", string);

			if(!strcmp(string, "null"))
			{
				fprintf(tableOut, "%s\t1\n", string);
				fprintf(listOut, "1 ");
			}
			else if(!strcmp(string, "odd"))
			{
				fprintf(tableOut, "%s\t\t8\n", string);
				fprintf(listOut, "8 ");
			}
			else if(!strcmp(string, "begin"))
			{
				fprintf(tableOut, "%s\t21\n", string);
				fprintf(listOut, "21 ");
			}
			else if(!strcmp(string, "end"))
			{
				fprintf(tableOut, "%s\t\t22\n", string);
				fprintf(listOut, "22 ");
			}
			else if(!strcmp(string, "if"))
			{
				fprintf(tableOut, "%s\t\t23\n", string);
				fprintf(listOut, "23 ");
			}
			else if(!strcmp(string, "then"))
			{
				fprintf(tableOut, "%s\t\t24\n", string);
				fprintf(listOut, "24 ");
			}
			else if(!strcmp(string, "while"))
			{
				fprintf(tableOut, "%s\t\t25\n", string);
				fprintf(listOut, "25 ");
			}
			else if(!strcmp(string, "do"))
			{
				fprintf(tableOut, "%s\t\t26\n", string);
				fprintf(listOut, "26 ");
			}
			else if(!strcmp(string, "call"))
			{
				fprintf(tableOut, "%s\t\t27\n", string);
				fprintf(listOut, "27 ");
			}
			else if(!strcmp(string, "const"))
			{
				fprintf(tableOut, "%s\t\t28\n", string);
				fprintf(listOut, "28 ");
			}
			else if(!strcmp(string, "var"))
			{
				fprintf(tableOut, "%s\t\t29\n", string);
				fprintf(listOut, "29 ");
			}
			else if(!strcmp(string, "procedure"))
			{
				fprintf(tableOut, "%s\t\t30\n", string);
				fprintf(listOut, "30 ");
			}
			else if(!strcmp(string, "write"))
			{
				fprintf(tableOut, "%s\t\t31\n", string);
				fprintf(listOut, "31 ");
			}
			else if(!strcmp(string, "read"))
			{
				fprintf(tableOut, "%s\t\t32\n", string);
				fprintf(listOut, "32 ");
			}
			else if(!strcmp(string, "else"))
			{
				fprintf(tableOut, "%s\t\t33\n", string);
				fprintf(listOut, "33 ");
			}
			else if(i < 12)
			{
				fprintf(tableOut, "%s\t\t2\n", string);
				fprintf(listOut, "2 %s ", string);
			}
			else if(i >= 12)
			{
				printf("%s is too long of a string.\n", string);
				validInput = 0;
			}
			else
			{
				printf("%s is an invalid string\n", string);
				validInput = 0;
			}
		}

		//Reads in and tokenizes numbers
		if(c >= 48 && c <= 57 && validInput && !end)
		{
			i = 0;
			while(!end && (c >= 48 && c <= 57 || (c >= 97 && c <= 122) 
									|| (c >= 65 && c <= 90)))
			{
				string[i] = c;
				string[i + 1] = '\0';
				i++;

				//Protects from segfaults if the file ends with a number
				if(fscanf(ifp, "%c", &c) == EOF)
					end = 1;
			}

			for(j = 0; j < i; j++)
				if(string[j] < 48 || string[j] > 57)
				{
					printf("%s is not a valid variable\n", string);
					validInput = 0;
					break;
				}

			if(i == 5 && validInput)
			{	
				sum = 0;
				int len = i, n;

				for(i = 0; i < len; i++)
				{
					n = string[len - i - 1] - 48;
					sum += (n * myPow(10, i));
				}

				if(sum > MAX_INT_SIZE)
				{
					printf("%d is too large of a number\n", sum);
					validInput = 0;
				}

			}

			if(i > 5 && validInput)
			{
				printf("%s is too large of a number\n", string);
				validInput = 0;
			}
			else
			{
				fprintf(cleanInput, "%s", string);
				fprintf(tableOut, "%s\t\t3\n", string);
				fprintf(listOut, "3 %s ", string);
			}
		}

		if(!validInput || end)
			continue;

		//The following lines reads in and tokenizes the characters
		fprintf(cleanInput, "%c", c);

		if(c == '+')
		{
			fprintf(tableOut, "%c\t\t4\n", c);
			fprintf(listOut, "4 ");
		}
		else if(c == '-')
		{
			fprintf(tableOut, "%c\t\t5\n", c);
			fprintf(listOut, "5 ");
		}
		else if(c == '*')
		{
			fprintf(tableOut, "%c\t\t6\n", c);
			fprintf(listOut, "6 ");
		}  
		else if(c == '=')
		{
			fprintf(tableOut, "%c\t\t9\n", c);
			fprintf(listOut, "9 ");
		}
		else if(c == '<')
		{
			fscanf(ifp, "%c", &c);

			if(c == '=')
			{
				fprintf(cleanInput, "%c", c);
				fprintf(tableOut, "<%c\t\t12\n", c);
				fprintf(listOut, "12 ");
			}
			else if(c == '>')
			{
				fprintf(cleanInput, "%c", c);
				fprintf(tableOut, "<%c\t\t10\n", c);
				fprintf(listOut, "10 ");
			}
			else 
			{
				ungetc(c, ifp);
				fprintf(tableOut, "<\t\t11\n");
				fprintf(listOut, "11 ");
			}
		}
		else if(c == '>')
		{
			fscanf(ifp, "%c", &c);

			if(c == '=')
			{
				fprintf(cleanInput, "%c", c);
				fprintf(tableOut, ">%c\t\t14\n", c);
				fprintf(listOut, "14 ");
			}
			else
			{
				ungetc(c, ifp);
				fprintf(tableOut, ">\t\t13\n");
				fprintf(listOut, "13 ");
			}
		}
		else if(c == '(')
		{
			fprintf(tableOut, "%c\t\t15\n", c);
			fprintf(listOut, "15 ");
		}
		else if(c == ')')
		{
			fprintf(tableOut, "%c\t\t16\n", c);
			fprintf(listOut, "16 ");
		}
		else if(c == ',')
		{
			fprintf(tableOut, "%c\t\t17\n", c);
			fprintf(listOut, "17 ");
		}
		else if(c == ';')
		{
			fprintf(tableOut, "%c\t\t18\n", c);
			fprintf(listOut, "18 ");
		}
		else if(c == '.')
		{
			fprintf(tableOut, "%c\t\t19\n", c);
			fprintf(listOut, "19 ");
		}
		else if(c == ':')
		{
			fscanf(ifp, "%c", &c);

			if(c == '=')
			{
				fprintf(cleanInput, "%c", c);
				fprintf(tableOut, ":%c\t\t20\n", c);
				fprintf(listOut, "20 ");
			}
			else 
			{
				ungetc(c, ifp);
				printf(": is not a valid symbol\n");
				validInput = 0;
			}
		}
		else if(c != '\n' && c != ' ' && c != '\t' && c!= '\r' && c != '/')
		{
			printf("%c is an invalid symbol\n", c);
			validInput = 0;
		}
	}

	close(ifp);
	close(cleanInput);
	close(tableOut);
	close(listOut);

	if(!validInput)
	{
		unlink("cleaninput.txt");
		unlink("lexemelist.txt");
		unlink("lexemetable.txt");
	}

	//Deletes any existing machine code file
	unlink("mcode.txt");
	unlink("stacktrace.txt");
	return 0;
}