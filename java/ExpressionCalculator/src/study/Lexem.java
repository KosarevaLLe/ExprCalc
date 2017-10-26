package study;

public class Lexem {

    public enum Type {
        Constant, Identifier,
        OpPlus, OpMinus, OpMul, OpDiv,
        BraceOpen, BraceClose
    };

    private Type type;

    public Type getType() {
        return type;
    }
    
    public Lexem(Type type) {
        this.type = type;
    }

    public Lexem(char c) {
        switch (c) {
        case '+': type = Type.OpPlus; break;
        case '-': type = Type.OpMinus; break;
        case '*': type = Type.OpMul; break;
        case '/': type = Type.OpDiv; break;
        case '(': type = Type.BraceOpen; break;
        case ')': type = Type.BraceClose; break;
        }
    }
    
    @Override
    public String toString() {
        switch (type) {
        case OpPlus: return "+";
        case OpMinus: return "-";
        case OpMul: return "*";
        case OpDiv: return "/";
        case BraceOpen: return "(";
        case BraceClose: return ")";
        }
        return "?";
    }
    
}
