#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>

#define FALSE 0
#define TRUE (!FALSE)

extern char buff[128];
extern FILE *input;

void getToken(char *c);
void getString(char *c);

void getToken();
void getString();

bool isDigit(char c);
bool isValidInt(char *str);
bool isValidFloat(char *str);

void loadFile(char *c);

#define PERROR(s) do { printf("ERROR %s %s %d\n%s\n", __FILE__, __FUNCTION__, __LINE__, s); system("pause");} while(FALSE)

#define PERRORTOK(s, t) do { printf("ERROR %s %s %d TOK:%d\n%s\n", __FILE__, __FUNCTION__, __LINE__, t->line, s); system("pause");} while(FALSE)

#endif
