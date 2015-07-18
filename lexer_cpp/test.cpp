#include "Tools.h"
#include "Tokenizer.h"
#include "ProgramBuilder.h"



int main() {
	buildTools();
	Tokenizer tokenizer;

	const char* str = "-c++;";

	const char* str2 = "func asd( ) {\n\tc = a+b ;\n \tif(a>0)\n\t\treturn a; \n\telse \n\t\treturn b;\n};\n\nif(c==0) b=a---c;//hmm\ndo {\n\tx=1; /*asdasd hello worl!*/\n}while(1);for(i=0; i<15; i++)\n\t a=.1+3.0/2*(a+b);//comment\n  ";
	TokenList list;
	printf("%s\n\n", str);
	tokenizer.tokenize(str, list);

	tokenizer.printList(list);

	printf("---------\n");


	ProgramBuilder builder;

	Object* program = builder.buildProgram(list);


	builder.printProgram(program);

	system("pause");
	return 0;
}
