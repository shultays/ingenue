#include "Tools.h"
#include "Tokenizer.h"
#include "ProgramBuilder.h"
#include "Interpreter.h"

int main() {
	buildTools();
	Tokenizer tokenizer;

	char str[1024] = "a=3;if(a==3){z=4;}";
	str[0] = '\0';
	//str = "a=1;";

	//tokenizer.printList(list);

	Interpreter interpreter;
	ProgramBuilder builder;

	int length = 0;
	std::vector<const Object*> programs;
	do 
	{
		TokenList list;
		tokenizer.tokenize(str, list);
		const Object* program = builder.buildProgram(list);

		if(program)
		{
			programs.push_back(program);

			//builder.printProgram(program);

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
	} while (true);
	
	return 0;
}
