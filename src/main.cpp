/*  
Name: Whitelist command limiter
File: main.cpp
Description: This program allows the restriction of process argument or parameter with the use of standard operating system features.
Copyright: 2016 Vertex Technologies Pty Ltd (ABN: 67 611 787 029)

License:
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

For a copy of the GNU General Public License see <http://www.gnu.org/licenses/>.


*/


// basic file operations
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define ARGUMENT_SIZE 20
#define WHITELIST_SIZE 20
#define MAXIMUM_STRING_LENGTH 2000

int main(int argc, char *argv[], char* envp[])
{

string whitelist[ARGUMENT_SIZE][WHITELIST_SIZE];
string arguments[ARGUMENT_SIZE];
int whitelist_size[ARGUMENT_SIZE];
int no_whitelists = 0;

//Check the argc value is valid
//-------------------------------------------
if (argc < 0 || argc > ARGUMENT_SIZE)
{
	return 1;
}


FILE * whitelist_file;
#ifdef linux
    whitelist_file = fopen ("/etc/whitelist.txt", "r");
#elif _WIN32
    whitelist_file = fopen ("C:\\Program Files\\whitelist\\whitelist.txt", "r");
#else
    #error Platform not supported
#endif

//TODO: Add a permission checker to ensure whitelist and this executable can only be modified by administrators


//Check the whitelist file opened successfully
//-------------------------------------------
if (whitelist_file == NULL)
{
  	 printf("File could not be opened\n");
  	 return 1;
}


//read the whitelist file to an array
//read character by character to allow filtering inappropriate characters
//-------------------------------------------
string tmp_word;
tmp_word = "";
char tmp_char;
int i = 0;
int j = 0;
int string_length=0;

do
{
	tmp_char = getc(whitelist_file);


	if (tmp_char == 32)
	{
		whitelist[i][j] = tmp_word;
		
		i++;
		tmp_word = "";
	}
	else if (tmp_char == '\n')
	{
		whitelist[i][j] = tmp_word;
		whitelist_size[j]=i;
		j++;
		i=0;
		tmp_word = "";
		string_length=0;
	}
	else if (tmp_char > 31 && tmp_char < 123 && tmp_char != '&' && tmp_char !='$' && tmp_char !='!' && tmp_char !=';' && tmp_char !='#' && tmp_char !='?')
	{
		tmp_word+=tmp_char;
	}  
	//ignore all other characters
	string_length++;
} while (tmp_char != EOF && j < WHITELIST_SIZE && string_length < MAXIMUM_STRING_LENGTH);

//close file
fclose(whitelist_file);

if (string_length >= MAXIMUM_STRING_LENGTH)
{
	printf("ERROR: Arguments too long\n");
  	return 1;
}



//check the size of whitelist is not empty
//-------------------------------------------
if (j > 0)
{ 
	no_whitelists = j;
}
else
{
	no_whitelists = -1;
	printf("ERROR: No whitelist, entries\n");
  	return 1;
}



//compare the command arguments against the whitelist
//-------------------------------------------
int whitelisted_command = -1;
int whitelisted_continue = 0;
char** arg;

for (j=0; j < WHITELIST_SIZE && j < no_whitelists && whitelisted_command == -1; j++)
{
	i=0;
	arg = argv;
	do
	{
		whitelisted_continue = 0;
		if (whitelist[i+1][j] == *arg)
		{
			whitelisted_continue = 1;
		}
		//allow whitelist confirmed if wildcard
		else if (whitelist[i+1][j] == "*")
		{
			whitelisted_continue = 1;
		}
		else
		{
			whitelisted_continue = 0;
		}
		i++;
		arg++;
	} while (whitelisted_continue == 1 && i <= whitelist_size[j] && i < argc);
	
	//check the command matches and is the same length
	if (whitelisted_continue == 1 && i == whitelist_size[j])
	{
		whitelisted_command = j;
		printf("Whitelist command found.\n");
	}
}


//exit if not a whitelist, otherwise execute command
if (whitelisted_command == -1)
{
	printf("Command not whitelisted.\n");
	return 1;
}
else if (whitelisted_command >= 0 && whitelisted_command < WHITELIST_SIZE)
{
	int string_size = whitelist[0][whitelisted_command].length();
	if ((argv[0] = (char *) malloc(string_size + 1)) == NULL)
	{
		printf("ERROR: Failed to create data\n");
		return 1;
	}
	strcpy(argv[0], whitelist[0][whitelisted_command].c_str());
	

	if (execve(argv[0], argv, *&envp) < 0)
	{
		printf("ERROR: exec failed\n");
		return 1;
	}

}


printf("Unknown error\n");
return(1);

}
