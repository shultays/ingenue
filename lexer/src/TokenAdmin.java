import java.util.Hashtable;


public class TokenAdmin {
	
	public Hashtable<String, TokenDef> tokens = new Hashtable<String, TokenDef>();
	
	TokenDef ws;
	static int id = 0;
	public static void print(Object str){
	    System.out.println(Thread.currentThread().getStackTrace()[2].getLineNumber() + " " + str);
	}
	public TokenDef addToken(boolean printcontent, String name, TokenDef tok){
		tok.name = name;
		tok.anon = false;
		tok.printcontent = printcontent;
		tokens.put(name, tok);
		return tok;
	}
    public TokenDef addToken(String name, TokenDef tok){
 
        return addToken(false, name, tok);
    }
    public TokenDef addToken(String name, Validator func){

        return addToken(false, name, func);
    }
	public TokenDef addToken(boolean printcontent, String name, Validator func){
		TokenDef tok = new TokenDef();
        tok.anon = false;
        tok.printcontent = printcontent;
		tok.func = func;
		tok.name = name;
		tokens.put(name, tok);
		return tok;
	}
	
	public TokenDef func(Validator func){
		TokenDef tok = new TokenDef();
		tok.func = func;
		return tok;
	}

	
    public TokenDef oneOf(final Object ... rest){
        final TokenDef tok = new TokenDef();
        tok.name = "~"+id++ +"oneof";
        tok.func = new Validator(){
            public int validate(Token parent, String str) {
                int ret = INVALID;
                Token selected = null;
                int maxl = -1;
                for(Object obj : rest){
                    final TokenDef t;
                    if(obj instanceof TokenDef) t = (TokenDef) obj;
                    else t = tokens.get(obj);
                    Token tt = new Token();
                    int v = t.validate(tt, str);
                    if(maxl < tt.getStr().length()){
                        ret = v;
                        maxl = tt.getStr().length();
                        selected = tt;
                    }
                }
                if(ret >= VALID && parent != null){
                    Token t = new Token();
                    t.def = tok;
                    t.setStr(str.substring(0, ret));


                    t.children.add(selected);

                    parent.children.add(t);
                }
                return ret;
            }
        };
        return tok;
    }
    

    public TokenDef atMostOne(final Object childTok){
    	final TokenDef tok = new TokenDef();
        tok.name = "~"+id++ +"atMostOne";

		tok.func = new Validator(){
			public int validate(Token parent, String strinput) {
			    
				TokenDef ch ;
			
				Token content = new Token();
				content.def = tok;
		        if(childTok instanceof TokenDef) ch = (TokenDef) childTok;
		        else ch = tokens.get(childTok);

			
			    int valch = ch.validate(content, strinput);
			    if(valch == INVALID){
			    	valch = 0;
			    }
				if(valch>VALID  && parent != null){
				    content.setStr(strinput.substring(0, valch));
				    parent.children.add(content);
				}
				return valch;
			}
		};
		return tok;
	}
	

    
	
	public TokenDef oneOrMore(final Object childTok){
		final TokenDef tok = new TokenDef();
        tok.name = "~"+id++ +"oneOrMore";

		tok.func = new Validator(){
			public int validate(Token parent, String strinput) {
			    
				int ret = 0;
				TokenDef ch ;
			
				Token content = new Token();
				content.def = tok;
		        if(childTok instanceof TokenDef) ch = (TokenDef) childTok;
		        else ch = tokens.get(childTok);

				String str = strinput;
				
				while(str.length() >= 0){
				    int valch = ch.validate(content, str);
				    if(valch == INVALID){
				        if(ret==0)ret = INVALID;
				        break;
				    }else if(valch >= VALID){
				        if(valch == 0) break;
				        ret+=valch;
				        str = str.substring(valch);
				    }
				}
				if(ret>VALID  && parent != null){
				    content.setStr(strinput.substring(0, ret));
				    parent.children.add(content);
				}
				return ret;
			}
		};
		return tok;
	}
	

    
	public TokenDef zeroOrMore(Object childTok){
		final TokenDef oneOrMore = oneOrMore(childTok);
		final TokenDef tok = new TokenDef();
        tok.name = "~"+id++ +"zeroOrMore";
		tok.func = new Validator(){
			public int validate(Token parent, String str) {
			    Token t = new Token();
				int vone = oneOrMore.validate(t, str);
                if(vone == INVALID) vone = 0;
			    if(parent != null){
			        t.def = tok;
			        t.setStr(str.substring(0, vone));
			        parent.children.add(t);
			    }

				return vone;				
			}
		};
		return tok;
	}

    public TokenDef concat(final Object ...rest){
        return concatbool(false, rest);
    }
    public TokenDef concatbool(final boolean nowscheck, final Object ...rest){
        final TokenDef tok = new TokenDef();
        tok.name = "~"+id++ +"concat";
        tok.func = new Validator(){
            public int validate(Token parent, String strinput) {
                int ret = 0;
                String str = strinput;

                Token content = new Token();
                content.def = tok;
                
                for(Object obj : rest){
                    int v;
                    if(!nowscheck){
                        v = ws.validate(null, str);
                        ret += v;
                        str = str.substring(v);
                    }
                    final TokenDef t;
                    if(obj instanceof TokenDef) t = (TokenDef) obj;
                    else t = tokens.get(obj);
                    v = t.validate(content, str);
                    if(v == INVALID) return INVALID;
                    ret += v;
                    str = str.substring(v);
                    if(!nowscheck){
                        v = ws.validate(null, str);
                        ret += v;
                        str = str.substring(v);
                    }
                    
                }
                if(ret >= VALID && parent != null){
                    content.setStr(strinput.substring(0, ret));
                    
                    parent.children.add(content);
                }
                return ret;
            }
        };
        return tok;
    }

    public TokenDef getToken(String str){
        return tokens.get(str);
    }

    public TokenDef fromString(final String input){
        final TokenDef tok = new TokenDef();
        tok.name = "~"+id++ +"fromString";
        tok.func = new Validator(){
            public int validate(Token parent, String str) {
                if(str.length() < input.length()){
                    return INVALID;
                }
                if(parent != null && input.compareTo(str.substring(0, input.length())) == 0){
                    Token t = new Token();
                    t.def = tok;
                    t.setStr(input);
                    parent.children.add(t);
                    return input.length();
                }
                return INVALID;
            }
        };
        return tok;
    }

	public int validate(String name, String str){
		return validate(name, null, str);
	}

    public int validate(String name, Token parent, String str){
        return tokens.get(name).validate(parent, str);
    }
    
	
	public TokenAdmin(){

        addToken("comment_multi", new Validator(){
            public int validate(Token parent, String str) {
                if(str.length() < 4) return 0;
                if(str.substring(0, 2).compareTo("/*") != 0) return 0;
                
                int i=str.indexOf("*/");
                if(i==-1) return 0;
                else return i+2;
            }
        });
        

        addToken("comment_single", new Validator(){
            public int validate(Token parent, String str) {
                if(str.length() < 2) return 0;
                if(str.substring(0, 2).compareTo("//") != 0) return 0;
                
                int i=str.indexOf("\n");
                if(i==-1) return str.length();
                else return i+1;
            }
        });

        
        addToken("whitespace",  new Validator(){
            public int validate(Token parent, String str) {
                for(int i=0; i<str.length(); i++){
                    char c = str.charAt(i);
                    if(c == ' ' || c == '\n' || c == '\t') continue;
                    return i==0?-2:i;
                }
                return str.length();
            }
        });
        
        addToken("ws", zeroOrMore(oneOf("whitespace", "comment_single", "comment_multi")));
        ws = tokens.get("ws");
        ws.anon = true;
	}
	
}
