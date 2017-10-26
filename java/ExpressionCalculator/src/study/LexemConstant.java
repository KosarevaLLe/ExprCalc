package study;

public class LexemConstant extends Lexem {
    
    private int value;
    
    public int getValue() {
        return value;
    }

    public LexemConstant(int value) {
        super(Type.Constant);
        this.value = value;
    }

    @Override
    public String toString() {
        return new Integer(value).toString();
    }
}
