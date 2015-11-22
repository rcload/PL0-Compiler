Tiny PL/0 Parser-CodeGen
Assignment #4
COP3402 – Fall 2014
Christopher Beier
Raymond Cload
Due: 11/16/2014 @ 11:59pm


To compile and run this PL/0 compiler in a Linux environment, simply run the command:

	sh CompileDriver.sh

This will create programs called 'scanner', 'parser', and 'vm' and take a PL/0 program from 'input.txt', compile, and run it. 

The output of this program can be found in their respective output files ('cleanInput.txt', 'lemxemelist.txt', 'lexemetable.txt', 'mcode.txt', and 'stacktrace.txt'). 

Output can also be sent to the terminal window by compiling with the following flags.

	-l : print a list of lexemes/tokens (scanner output) to the screen
	-a : print the generated assembly code (parser/codegen output) to the screen
	-v : print the virtual machine execution trace (virtual machine output) to the screen  

Included is a text file called ExampleOutput.txt with an example program that contains the input file to test nested procedures, the output of the scanner, the output of the parser, whether or not the program is syntactically correct and the stack output of the virtual machine. 