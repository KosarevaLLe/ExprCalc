package study;

public class OperandNode extends ExpressionNode {

    private int value;
    
    public int getValue() {
        return value;
    }

    public OperandNode(int i) {
        value = i;
    }

    public int calc() {
        return value;
    }
}
