package study;

import java.util.ArrayList;

public class ExpressionCalc {
    
    private String text;
    private LexemReader reader;
    private ExpressionNode expr;

    @FunctionalInterface
    public interface Matcher {
        public ExpressionNode match(LexemReader reader);
    }
    
    public static ExpressionNode matchBraces(LexemReader reader,
            Matcher innerMatcher, boolean allowEmpty)
    {
        if (!reader.hasNext())
            return null;
        int state0 = reader.getState();
        Lexem c = reader.getNext();
        if (c.getType() != Lexem.Type.BraceOpen) {
            reader.revertToState(state0);
            return null;
        }
        ExpressionNode inner = innerMatcher.match(reader);
        if (inner == null && !allowEmpty) {
            reader.revertToState(state0);
            return null;
        }
        if (!reader.hasNext()) {
            reader.revertToState(state0);
            return null;
        }
        c = reader.getNext();
        if (c.getType() != Lexem.Type.BraceClose) {
            reader.revertToState(state0);
            return null;
        }
        return inner;
    }

    public static ExpressionNode matchOperand(LexemReader reader)
    {
        /* 1.
        ExpressionNode br = matchBraces(reader,
                new Matcher () {
            
                    @Override
                    public ExpressionNode match(LexemReader reader) {
                        return matchExpr(reader);
                    }
                }, false);
        */

        /* 2.
        ExpressionNode br = matchBraces(reader,
                (LexemReader r) -> {
                    return matchExpr(r);
                }, false);
        */
        
        /* 3. */
        ExpressionNode br = matchBraces(reader, ExpressionCalc::matchExpr, false);
       
        if (br != null)
            return br;

        if (!reader.hasNext())
            return null;

        int state0 = reader.getState();
        Lexem c = reader.getNext();
        if (c.getType() != Lexem.Type.Constant) {
            // TODO: check vars, etc.
            reader.revertToState(state0);
            return null;
        }
        
        return new OperandNode(((LexemConstant)c).getValue());
    }

    public static ExpressionNode matchOp(LexemReader reader, String ops)
    {
        if (!reader.hasNext())
            return null;

        int state0 = reader.getState();
        Lexem c = reader.getNext();
        if (ops.indexOf(c.toString()) == -1) {
            reader.revertToState(state0);
            return null;
        }
        
        return new OperatorNode(null, null, c.toString());
    }

    public static ExpressionNode matchTerm(LexemReader reader,
            Matcher opdMatcher, Matcher opMatcher)
    {
        ExpressionNode opd1 = opdMatcher.match(reader);
        if (opd1 == null)
            return null;
        while (true) {
            int state0 = reader.getState();
            ExpressionNode op = opMatcher.match(reader);
            if (op == null) {
                return opd1;
            }
            ExpressionNode opd2 = opdMatcher.match(reader);
            if (opd2 == null) {
                reader.revertToState(state0);
                return opd1;
            }
            ((OperatorNode)op).setLeft(opd1);
            ((OperatorNode)op).setRight(opd2);
            opd1 = op;
        }
    }
    
    public static ExpressionNode matchMulOp(LexemReader reader)
    {
        return matchOp(reader, "*/");
    }
        
    public static ExpressionNode matchAddOp(LexemReader reader)
    {
        return matchOp(reader, "+-");
    }
        
    public static ExpressionNode matchCommaOp(LexemReader reader)
    {
        return matchOp(reader, ",");
    }
        
    public static ExpressionNode matchMulTerm(LexemReader reader)
    {
        return matchTerm(reader, ExpressionCalc::matchOperand, ExpressionCalc::matchMulOp);
    }
        
    public static ExpressionNode matchAddTerm(LexemReader reader)
    {
        return matchTerm(reader, ExpressionCalc::matchMulTerm, ExpressionCalc::matchAddOp);
    }
        
    public static ExpressionNode matchCommaTerm(LexemReader reader)
    {
        return matchTerm(reader, ExpressionCalc::matchAddTerm, ExpressionCalc::matchCommaOp);
    }
        
    public static ExpressionNode matchExpr(LexemReader reader)
    {
        return matchAddTerm(reader);
    }
        
    private void compile() {
        reader = new LexemReader(text);
        expr = matchExpr(reader);
        // check for the EOF
    }
    
    public ExpressionCalc(String text) {
        this.text = text;
        compile();
    }
    
    public int calc() {
        return expr.calc();
    }
}
