#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "Tools.h"
#include "TokenLib.h"
#include "ValueLib.h"
#include "FlowLib.h"

Token *root;

int main(){
	float a = 1;
	float b = 1;
	float c;
	float i;
	float d;

	a = 11;
	b = 1;
	c = 3;
	d = 5;
	while (b <= a)
	{
		d = d + b;
		b += c;
	}




	printf("%f %f %f %f\n", a, b, c, d);
	system("pause");


	
	return 0;
	loadFile("input.txt");
	buildProgram(&root);
	print(root, 0, 0);


	interpreteFlow(root);
	for (int i = 'a'; i <= 'z'; i++)
	{
		char buf[] = "a";
		buf[0] = i;
		getVar(buf);
	}
	system("pause");
	return 0; 
}
