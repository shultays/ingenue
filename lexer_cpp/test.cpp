#include "Tools.h"
#include "Tokenizer.h"
#include "ProgramBuilder.h"
#include "Interpreter.h"

int main() {
	buildTools();
	Tokenizer tokenizer;
	char str[1024] = "p = func(a, b){ return a + b;};";
	//char str[1024] = "a;c;b = func(a){a=3;c=5;};if(a){b=3;}for(a=0;a<10;a++){b=3;}while(a>3)a--;do{a++;}while(a<10);";
	//str[0] = '\0';
	//str = "a=1;";


	Interpreter interpreter;
	ProgramBuilder builder;

	int length = 0;
	std::vector<Object*> programs;
	do 
	{
		TokenList list;
		tokenizer.tokenize(str, list);
		//tokenizer.printList(list);
		Object* program = builder.buildProgram(list);

		if(program)
		{
			programs.push_back(program);

			builder.printProgram(program);

			const Object* object = program->firstChild;
			interpreter.interprete(program);

			if(object->nextSibling == nullptr)
			{
				if(object->firstChild && object->firstChild->objectType == Tt_value)
				{
					interpreter.printStackTop(builder);
				}
			}
			length = 0;

		}
		else
		{
			if(str[length] == '\0' || (str[length] == '\n' && str[length+1] == '\0')){
				length = 0;
			} else{
				length = strlen(str);
				str[length] = ' ';
				str[length+1] = '\0';
				length++;
			}
		}
		if(length == 0)
		{
			printf(">> ");
		}
		else
		{
			printf(">>>> ");
		}
		fgets(str + length, sizeof(str), stdin);
	} while (feof(stdin) == false);
	
	for(unsigned i=0; i<programs.size(); i++){
		builder.deleteProgram(programs[i]);
	}
    builder.clear();
	interpreter.clear();
	return 0;
}
