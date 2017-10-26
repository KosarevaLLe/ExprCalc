package tests.expr;

import junit.framework.TestCase;

import study.*;

public class TestExpressionCalc extends TestCase {

    public void testExpressionCalc() {
        ExpressionCalc ecalc = new ExpressionCalc("2 + 2");
        assertEquals(4, ecalc.calc());
    }

    public void testOperandNode() {
        OperandNode node = 
                new OperandNode(42);
        assertEquals(42, node.getValue());
    }
    
    public void testExpressionNode3() {
        OperatorNode node = 
                new OperatorNode(
                        new OperandNode(13),
                        new OperandNode(14),
                        "+");
        assertEquals("+", node.getOpCode());
        assertEquals(13,
                ((OperandNode)node.getLeft()).getValue());
        assertEquals(14,
                ((OperandNode)node.getRight()).getValue());
    }
    
    public void testExpressionNodeCalc() {
        ExpressionNode node = 
                new OperatorNode(
                        new OperandNode(13),
                        new OperandNode(14),
                        "+");
        assertEquals(27, node.calc());
    }
}