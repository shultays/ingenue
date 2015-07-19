#include "Tools.h"
#include "Tokenizer.h"
#include "ProgramBuilder.h"
#include "Interpreter.h"


int main() {
	buildTools();
	Tokenizer tokenizer;

	const char* str = "1+3;";

	TokenList list;
	printf("%s\n\n", str);
	tokenizer.tokenize(str, list);

	tokenizer.printList(list);

	printf("---------\n");


	ProgramBuilder builder;

	Object* program = builder.buildProgram(list);


	builder.printProgram(program);


	Interpreter interpreter;

	interpreter.interprete(program);

	builder.deleteProgram(program);
	system("pause");
	return 0;
}
