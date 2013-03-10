
public interface Validator {
	public static final int VALID = 0;
	public static final int INVALID = -2;
	
	//public int validate(String str);

    public int validate(Token parent, String str);
}
