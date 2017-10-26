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
        ExpressionNode node1 =
                new OperatorNode(
                        new OperandNode(13),
                        new OperandNode(14),
                        "+");
        assertEquals(27, node1.calc());

        ExpressionNode node2 =
                new OperatorNode(
                        new OperandNode(15),
                        new OperandNode(10),
                        "-");
        assertEquals(5, node2.calc());

        ExpressionNode node3 =
                new OperatorNode(
                        new OperandNode(12),
                        new OperandNode(10),
                        "*");
        assertEquals(120, node3.calc());

        ExpressionNode node4 =
                new OperatorNode(
                        new OperandNode(150),
                        new OperandNode(10),
                        "/");
        assertEquals(15, node4.calc());

        ExpressionNode node5 =
                new OperatorNode(
                        new OperandNode(2),
                        new OperandNode(0),
                        "/");
        assertEquals(0, node5.calc());
    }
}