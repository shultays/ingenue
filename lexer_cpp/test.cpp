#include "Tools.h"
#include "Tokenizer.h"
#include "ProgramBuilder.h"
#include "Interpreter.h"

int main() {
	buildTools();
	Tokenizer tokenizer;

	const char* str = "b = func (){a;};";

	TokenList list;
	tokenizer.tokenize(str, list);

	tokenizer.printList(list);
	system("pause");


	ProgramBuilder builder;
	Program program = builder.buildProgram(list);

	builder.printProgram(program);


	Interpreter interpreter;

	interpreter.interprete(program);
	interpreter.printVal(program, "a");
	interpreter.printVal(program, "b");
	interpreter.printVal(program, "c");
	builder.deleteProgram(program);
	system("pause");
	return 0;
}
