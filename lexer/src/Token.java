import java.util.Vector;


public class Token {
    TokenDef def;
    String strcontent;
    Vector<Token> children = new Vector<Token>();
    public void setStr(String str) {

        strcontent = str;       
    }
    
    void print(int tab, boolean showAnon){
        boolean p = def != null && (showAnon || !def.anon);
        String name = null;
        if(def != null){
            name=def.name;
            if(def.alias != null){
                name = def.alias;
            }
        }
        if(p){
            for(int i=0; i<tab;i++)System.out.print("    ");

            if(def.printcontent){
                System.out.println(name + " " + strcontent);
            }else{
                System.out.println(name);
            }
        }
        for(Token t : children) t.print(tab+(p?1:0), showAnon);
        if(p && !def.printcontent){
            for(int i=0; i<tab;i++)System.out.print("    ");
            if(def != null) System.out.println("/"+name);
            
        }
    }
    void print(int tab){
        print(tab, false);
    }


    String realstr = null;
    String getStr(){
        if(realstr != null) return realstr;
        realstr = "";
        for(Token t : children) realstr += t.getStr();
        if(realstr.length() == 0 && strcontent != null) realstr = strcontent;
        return realstr;
    }

	public void interprete() {
		//if(def != null && def.interpreter != null) def.interprete();
        for(Token t : children) t.interprete();
		
	}
}
