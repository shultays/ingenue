import java.util.Hashtable;


public class TokenDef implements Validator{
	public String name;
	public Validator func;
	public boolean anon = true;
    public boolean printcontent = false;
    public String alias;


	/*
	public int validate(String str) {
		return func.validate(str);
	}*/
	//public Interpreter interpreter;

    public int validate(Token parent, String str){
        return func.validate(parent, str);
    }

	public void interprete() {
		
	}
    
}
