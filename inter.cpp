#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "Tools.h"
#include "TokenLib.h"
#include "ValueLib.h"
#include "FlowLib.h"

Token *root;

int main(){
  int i;
  loadFile("input.txt");
  buildProgram(&root);
  print(root, 0);


  interpreteFlow(root);
  getVar("a");
  getVar("b");
  getVar("c");
  getVar("d");
  return 0; 
}
