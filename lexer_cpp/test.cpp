#include "Tools.h"
#include "Tokenizer.h"
#include "ProgramBuilder.h"
#include "Interpreter.h"

int main() {
	buildTools();
	Tokenizer tokenizer;

	const char* str = "a;b=func(c){z=55;a=z+c;};b(12);";
	//str = "a=1;";
	TokenList list;
	tokenizer.tokenize(str, list);

	tokenizer.printList(list);


	ProgramBuilder builder;
	Program program = builder.buildProgram(list);

	builder.printProgram(program);


	Interpreter interpreter;

	interpreter.interprete(program);
	interpreter.printVal(program, "a");
	interpreter.printVal(program, "b");
	interpreter.printVal(program, "c");
	interpreter.printVal(program, "d");
	interpreter.printVal(program, "e");
	interpreter.printVal(program, "f");
	builder.deleteProgram(program);
	system("pause");
	return 0;
}
