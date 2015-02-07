#include "Tools.h"
#include <string.h>
FILE *input;

char buff[128];


void loadFile(char *c){
	input = fopen("input.txt", "r");
}

void getToken(char *c){
	fscanf(input, "%s", c);
}

void getString(char *c){
	char t;
	do{
		t = fgetc(input);
	} while (t != '\"' && t != '\'');
	int i = 0;
	do{
		c[i++] = fgetc(input);
	} while (c[i-1] != t);
	c[i - 1] = '\0';

}
void getToken(){
	getToken(buff);
}

void getString(){
	getString(buff);
}

bool isDigit(char c)
{
	return c >= '0' && c <= '9';
}

bool isValidInt(char *str)
{
	if (*str == '-' || *str == '+')
		++str;

	if (!*str)
		return false;

	while (*str)
	{
		if (isDigit(*str))
		{
			++str;
		}
		else
		{
			return false;
		}
	}

	return true;
}


bool isValidFloat(char *str)
{
	if (*str == '-' || *str == '+')
		++str;

	if (!*str)
		return false;

	bool hasDot = false;
	while (*str)
	{
		if (isDigit(*str))
		{
			++str;
		}
		else if (*str == '.' && !hasDot)
		{
			++str;
			hasDot = true;
		}
		else
		{
			return false;
		}
	}

	return true;
}