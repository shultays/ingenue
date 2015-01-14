
public class Test {

    public static void main(String args[]){
        TokenAdmin ta = new TokenAdmin();

        ta.addToken("empty", ta.fromString(""));
        ta.addToken("letter", new Validator(){
            public int validate(Token parent, String str) {
                if(str.length() == 0) return INVALID;
                char c = str.charAt(0);
                return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')?1:INVALID;
            }
        });
        
        ta.addToken("digit", new Validator(){
            public int validate(Token parent, String str) {
                if(str.length() == 0) return INVALID;
                char c = str.charAt(0);
                return (c >= '0' && c <= '9')?1:INVALID;
            }
        });
        ta.addToken(true, "float", ta.concatbool(true, ta.zeroOrMore("digit"), ta.fromString("."), ta.oneOrMore("digit")));
        ta.addToken(true, "operator", ta.oneOf(
                ta.fromString("+"),
                ta.fromString("-"),
                ta.fromString("*"),
                ta.fromString("/"),
                ta.fromString("%"),
                ta.fromString("<"),
                ta.fromString(">"),
                ta.fromString("<="),
                ta.fromString(">="),
                ta.fromString("!="),
                ta.fromString("&&"),
                ta.fromString("||"),
                ta.fromString("&"),
                ta.fromString("|"),
                ta.fromString("<<"),
                ta.fromString(">>"),
                ta.fromString("^"),
                ta.fromString("=="))
        );
        
        ta.addToken("underscore", ta.fromString("_"));

        ta.addToken(true, "integer", ta.oneOrMore("digit"));
        ta.addToken(true, "word", ta.oneOrMore("letter"));
        ta.addToken("semicolon", ta.fromString(";"));
        ta.addToken("if", ta.fromString("if"));
        ta.addToken("p_left", ta.fromString("("));
        ta.addToken("p_right", ta.fromString(")"));
        ta.addToken("cp_left", ta.fromString("{"));
        ta.addToken("cp_right", ta.fromString("}"));
        ta.addToken("for", ta.fromString("for"));

        ta.addToken("while", ta.fromString("while"));
        ta.addToken("do", ta.fromString("do"));

        ta.getToken("while").anon = true;
        ta.getToken("do").anon = true;
        ta.getToken("if").anon = true;
        ta.getToken("for").anon = true;
        ta.getToken("do").anon = true;
        ta.getToken("semicolon").anon = true;
        ta.getToken("p_left").anon = true;
        ta.getToken("p_right").anon = true;
        ta.getToken("cp_left").anon = true;
        ta.getToken("cp_right").anon = true;
        
        ta.addToken("string_1", new Validator(){
            public int validate(Token parent, String str) {
                if(str.length() < 2) return INVALID;
                if(str.charAt(0) != '"') return INVALID;
                System.out.println("ok");
                int i=1;
                while(i<str.length()){
                	if(str.charAt(i) == '"') break;
                	if(str.charAt(i) == '\\') i+=2;
                	else i++;
                }
                if(i>=str.length()) return INVALID;
                if(parent != null){
                    Token t = new Token();
                    t.def = null;
                    t.setStr(str.substring(0, i+1));
                    parent.children.add(t);
                }
                return i+1;

            }
        });        
        ta.addToken("string_2", new Validator(){
            public int validate(Token parent, String str) {
                if(str.length() < 2) return INVALID;
                if(str.charAt(0) != '\'') return INVALID;
                int i=1;
                while(i<str.length()){
                	if(str.charAt(i) == '\'') break;
                	if(str.charAt(i) == '\\') i+=2;
                	else i++;
                }
                if(i>=str.length()) return INVALID;
                if(parent != null){
                	 Token t = new Token();
                     t.def = null;
                     t.setStr(str.substring(0, i+1));
                     parent.children.add(t);
                }
                return i+1;

            }
        });
        ta.addToken(true, "string", ta.oneOf("string_1", "string_2"));
        ta.addToken(true, "variable", 
            ta.concatbool(true,
                    ta.oneOf("letter", "underscore"), 
                    ta.zeroOrMore(ta.oneOf("letter", "digit", "underscore"))
            )
        );
        ta.addToken(true, "singleoperator", 
                ta.oneOf(
                    ta.fromString("++"),
                    ta.fromString("--")
                    ));
        ta.addToken("variable_withpost", 
                ta.concat("variable","singleoperator")
            );
        ta.addToken("negate_value", 
                ta.concat(ta.fromString("-"),"value"))
            ;
        ta.addToken("variable_withpre", 
                ta.concat("singleoperator", "variable")
            );
        
        ta.addToken("value_one",
                ta.oneOf("assignment","funccall","variable","integer","float","string", "variable_withpre",
                        "variable_withpost", "negate_value",ta.concat("p_left", "value", "p_right"))
        );

        ta.addToken("assignment", ta.concat("variable",ta.fromString("="), "value"));
        
        ta.tokens.get("value_one").anon = true;
        ta.addToken("value", 
                ta.concat("value_one", ta.zeroOrMore(ta.concat("operator","value_one")))
        );

        ta.addToken("multiple_statement", ta.oneOrMore("statement"));
        
        ta.addToken("return", ta.fromString("return"));
        ta.addToken("break", ta.fromString("break"));
        
        ta.addToken("statement", ta.oneOf(
                ta.concat("value","semicolon"),
                ta.concat("cp_left","multiple_statement","cp_right"),
                "ifcond",
                "forloop",
                "whileloop",
                "dowhileloop",
                "semicolon",
                "funcdef",
                ta.concat("break","semicolon"),
                ta.concat("return","semicolon"),
                ta.concat("return","value","semicolon")
         ));

        
        ta.addToken("funcdef",
                ta.concat(
                        ta.fromString("func"), "variable",
                        "p_left",
                            ta.concat("variable", ta.zeroOrMore(ta.concat(ta.fromString(","), "variable"))),
                        "p_right",
                        "statement"));

        ta.addToken("funccall",
                ta.concat("variable", "p_left",
                        ta.concat("value", ta.zeroOrMore(ta.concat(ta.fromString(","), "value"))),
                        "p_right")
        );
        
        ta.tokens.get("multiple_statement").anon = true;

        ta.addToken("else", ta.fromString("else"));
        ta.addToken("ifcond",
                ta.concat(
                    ta.concat("if", "p_left","value", "p_right", "statement"),
                    ta.zeroOrMore(ta.concat("else", "statement"))
                )
        );
        
        ta.addToken("forloop", ta.concat("for",
                "p_left", 
                    "value", "semicolon", 
                    "value", "semicolon", 
                    "value", "p_right", "statement"));

        ta.addToken("whileloop", ta.concat("while",
                "p_left", "value", "p_right", "statement", "semicolon"));
        

        ta.addToken("whileloop", ta.concat("while",
                "p_left", "value", "p_right", "statement"));
        
        ta.addToken("dowhileloop", ta.concat("do","statement",
                "while", "p_left", "value", "p_right" ));

        ta.addToken("program", ta.oneOrMore("multiple_statement"));
        
        
        Token root = new Token();
        String str = "func asd( ) {\n\tc = a+b ;\n \tif(a>0)\n\t\treturn a; \n\telse \n\t\treturn b;\n} \n\nasd(1, 2);\n\nif(c==0) b=a---c;//hmm\ndo {\n\tx=1;\n\tc=2;\n\ta=5;  /*asdasd hello worl!*/\n}while(1);\nfor(i=0; i<15; i++)\n\t a=.1+3.0/2*(a+b);//comment\n  ";

        root.interprete();

        str = "func t(a){a = 1;} t(b, c, d); a = 1; a++; ++a; b = 1 + a++; c = 1 + ++a; d = a + b + c++; e = c-- + a--;";
        if(args.length > 0) str = args[0];
        ta.validate("program", root, str);
        root.print(0, false);
    }
}
