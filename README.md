#ingenue

ingenue is a programming language I am/was working on. 

It is dynmamic typing language. It has 4 data types: float, int, string and function.

It can do conditionals, loops or basic arithmetic. Syntax is similar to C language. Function definitions are a little bit different though. In ingenue, functions are also value. You can assign them to variables, pass them to functions etc. A function is defined and called like this:

```
f = func(x){
  return x + 2;
};

t = f(2);
```

There are two scopes. Global scope and local scope. You can use `{}` to define a scope. 

There is a stack and a heap. But currently there is no way to make a dynamic allocation on heap. Primitives (int & float) stored completely on stack. strings allocates their content in heap. Kinde like Java. Everything gets autamaticly freed when they are out of scope. 

Standard lib includes 4 functions. `print`, `scan`, `assert`, `exit`. It is possible to override them by simply assigning them to a new value.

There are two phases in program. 

### Tokenizer

Tokenizer reads the source code as a string and converts it into tokens. Tokens are defined like this:

`addToken(Tt_semicolon, fromString(";"));`

More complex token can be defined by combining other tokens.

```
addToken(Tt_func_def, join(
		fromString("func"),
		Tt_p_left,
	    atMostOne(join(Tt_variable_def, zeroOrMore(join(fromString(","), Tt_variable_def)))),
		Tt_p_right,
		Tt_cp_left, Tt_multiple_statement, Tt_cp_right)
)
```

Or tokens can use a lambda to define a token.

```
addToken(Tt_comment_multi, 
  [](const char *code, TokenList& tokenDataList) {
		const char *i = code;
		if (*i++ == '/' && *i++ == '*') {
			while (*i != '\0' && (*i != '*' && *(i + 1) != '/')) i++;
			return *i ? (int)(i - code + 2) : INVALID;
		}
		return INVALID;
	}
)
```

This one accepts C style multi line comments `/* comment! */`. It should return INVALID if input string is not a multi line comment, or return length of the string it accepts. `/* comment! */` should return 14 for example.


### Program Builder

Program builders inputs tokens generated by tokenizer and compiles them into an easier format to make it run. For example an integer constant token is just an enum and string like "42". Programbuilder gets this token, parses "42" and stores 42 as an integer. An arithmetic value token list such as `a+b*2-1` is reordered in postfix format `ab2*1-+` so it can be interpreted more easily.

Program builder also finds out where the variables are located. If it is a local variable, it converts the stack position relative to current position of stack pointer. If it is a global variable, it simply converts it to absolute position of the variable on stack. For example 

```
g; //#0
f /*#1*/ = func(x /*0*/, y /*1*/){
  a; //2
  b; //3
  {
    d; //4
  }
  e; //4
  d; //5
  a; //2
  g; //#0
}
```

* g is a global. It is our first global so it gets the absolute position of #0
* f is also a global. It gets next absolute position, #1
* After that it defines a function value. Now it switches to relative mode. Function parameters are also located on stack, relative to current position of stack. Each function call moves stack pointer up and moves it back on return.
* Parameters gets the first positions relative to stack pointer. x gets 0 and y gets 1
* a and b are local variables gets next two relative positions, 2 and 3.
* Before defining d, it opens a new scope, but stack pointer is unchanged. d gets 4th relative position. 
* Once scope is closed, d is no longer defined. And e gets the same stack pointer as d, 4.
* Next it defines another d, this is completely unrelated to one in previous scope, it is a new variable and gets a new relative position.
* a is already defined and program boilder knows its position as relative 2
* g is defined in global scope, its absolute position is #0

Once program builder is done with variables, their memory addresses are known in compile time so they can be accessed in O(1). Interpreter does not care about name of variables, they are decayed into relative/absolute positions on stack.

### Interpreter

Interpreter interpretes the output of program builder. It inputs a tree

source:
```
z=1;p = func(a){ return a+z;};f=p(2);
```

program tree:
```
program 3
statement
  value
    assignment
      z (4)
      value
        1
statement
  value
    assignment
      p (5)
      value
        func (a (0))
          multiple_statement 0
            statement
              return
                value
                  a (0)
                  z (-5)
                  +

statement
  value
    assignment
      f (6)
      value
        func_call
          p (5)
          (
            value
              2
          )
```

At this point I tried not use dynamic allocations anymore. Interpreter allocates a big chunck of buffer and everything related to our program is stored in this buffer. Beginning of this buffer is used for stack, and tail of this buffer is used for heap. Once the program is compiled, there are no more allocations.

# Usage

Once the program is run, you can type in your code and it will compile and run if it is valid. It will accept input until it can compile it correctly. If you make a mistake you can use ctrl-c to ignore current inputs. For debugging purposes it also shows compiled form of program and last value on stack.

An example run is like this

```
>> a = 3;
  program 1
    statement
      value
        assignment
          a (4)
          value
            3
3
>> b = a + 2;
  program 1
    statement
      value
        assignment
          b (5)
          value
            a (4)
            2
            +
5
>> f = func(x, y){
>>>> return x + y;
>>>> };
  program 1
    statement
      value
        assignment
          f (6)
          value
            func (x (0), y (0))
              multiple_statement 0
                statement
                  return
                    value
                      x (0)
                      y (1)
                      +

func (x (0), y (0))
  multiple_statement 0
    statement
      return
        value
          x (0)
          y (1)
          +

>> t = f(3,4);
  program 1
    statement
      value
        assignment
          t (7)
          value
            func_call
              f (6)
              (
                value
                  3
                value
                  4
              )

7
>>
```

My input is given in lines that start with >>

In future there will be ways to run a text file directly.

# Install

ingenue comes with VS project files. There are no dependincies.

Real source is in lexer_cpp folder. Other folders are from previous versions of the language, in which lexer and interpreter was different applications. lexer was a java program and interpreter were C++, it was my wilder days :)

# Future

Once the language is more mature, I am planning to add more experimental features. One feature I think of is being able to modify source code in run time. 

