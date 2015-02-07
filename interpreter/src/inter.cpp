#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "Tools.h"
#include "TokenLib.h"
#include "ValueLib.h"
#include "FlowLib.h"

Token *root;

int main(){
	int a, b, c, d, e;

	
	loadFile("input.txt");
	buildProgram(&root);
	print(root, 0, 0);


	interpreteFlow(root);
	printf("\n\n----------------\n");
	for (int i = 'a'; i <= 'z'; i++)
	{
		char buf[] = "a";
		buf[0] = i;
		getVar(buf);
	}
	system("pause");
	return 0; 
}
