package tests.expr;

import java.util.ArrayList;

import junit.framework.TestCase;

import study.*;

public class TestExpressionCalc extends TestCase {

    public void testExpressionCalc() {
        ExpressionCalc ecalc = new ExpressionCalc("2 + 2");
        assertEquals(4, ecalc.calc());
        ExpressionCalc ecalc2 = new ExpressionCalc("49 - 6*8");
        assertEquals(1, ecalc2.calc());
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
    
    public void testLexemCons() {
        Lexem l1 = new Lexem(Lexem.Type.OpPlus);
        assertEquals(Lexem.Type.OpPlus, l1.getType());
        assertEquals("+", l1.toString());

        Lexem l2 = new Lexem(Lexem.Type.OpMinus);
        assertEquals("-", l2.toString());

        Lexem l3 = new Lexem(Lexem.Type.OpMul);
        assertEquals("*", l3.toString());

        Lexem l4 = new Lexem(Lexem.Type.OpDiv);
        assertEquals("/", l4.toString());

        Lexem l5 = new Lexem(Lexem.Type.BraceOpen);
        assertEquals(Lexem.Type.BraceOpen, l5.getType());
        assertEquals("(", l5.toString());

        Lexem l6 = new Lexem(Lexem.Type.BraceClose);
        assertEquals(")", l6.toString());

        LexemIdentifier l7 = new LexemIdentifier("abc");
        assertEquals(Lexem.Type.Identifier, l7.getType());
        assertEquals("abc", l7.getId());
        assertEquals("abc", l7.toString());

        LexemConstant l8 = new LexemConstant(123);
        assertEquals(Lexem.Type.Constant, l8.getType());
        assertEquals(123, l8.getValue());
        assertEquals("123", l8.toString());
   }

   public void testLexemReader() {
       ArrayList<Lexem> lx = LexemReader.parseLexems(" 43 + x1*0");
       assertEquals(5, lx.size());
       assertEquals("43", lx.get(0).toString());
       assertEquals(Lexem.Type.Constant, lx.get(0).getType());
       assertEquals("+", lx.get(1).toString());
       assertEquals(Lexem.Type.OpPlus, lx.get(1).getType());
       assertEquals("x1", lx.get(2).toString());
       assertEquals(Lexem.Type.Identifier, lx.get(2).getType());
       assertEquals("*", lx.get(3).toString());
       assertEquals(Lexem.Type.OpMul, lx.get(3).getType());
       assertEquals("0", lx.get(4).toString());
       assertEquals(Lexem.Type.Constant, lx.get(4).getType());
       
   }
}