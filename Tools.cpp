#include "Tools.h"
FILE *input;

char buff[128];


void loadFile(char *c){
  
  input = fopen("input.txt", "r");
  
}

void getToken(char *c){
  fscanf(input, "%s", c); 
}

void getToken(){
  getToken(buff);
}
