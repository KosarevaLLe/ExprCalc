package study;

import java.util.ArrayList;
import java.util.Iterator;

public class LexemReader {

    public enum State {Initial, ReadNumber, ReadIdentifier};

    public static boolean isSpace(char c) {
        return c == ' ' || c == '\t';
    }

    public static boolean isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    public static boolean isAlpha(char c) {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z');
    }

    public static ArrayList<Lexem> parseLexems(String expr_str)
    {
        ArrayList<Lexem> result = new ArrayList<>();
        
        State state = State.Initial;
        String buffer = "";
        
        for (int i = 0; i <= expr_str.length(); ) {
            char c = i < expr_str.length() ?
                    expr_str.charAt(i): ' ';
            switch (state) {

            case ReadNumber:
                if (isDigit(c)) {
                    buffer += String.valueOf(c);
                    i++;
                }
                else {
                    result.add(new LexemConstant(Integer.parseInt(buffer)));
                    state = State.Initial;
                }
                break;
            
            case ReadIdentifier:
                if (isDigit(c) || isAlpha(c) || c == '_') {
                    buffer += String.valueOf(c);
                    i++;
                }
                else {
                    result.add(new LexemIdentifier(buffer));
                    state = State.Initial;
                }
                break;
                
            default:
                if (isDigit(c)) {
                    state = State.ReadNumber;
                    buffer = "";
                }
                else if (isAlpha(c) || c == '_') {
                    state = State.ReadIdentifier;
                    buffer = "";
                }
                else {
                    if (!isSpace(c))
                        result.add(new Lexem(c));
                    i++;
                }
            }
        }
        
        return result;
    }

    private ArrayList<Lexem> lexList;
    private int lexPos;

    public LexemReader(String s) {
        lexList = parseLexems(s);
        lexPos = 0;
    }
    
    public int getState() {
        return lexPos;
    }
    
    public void revertToState(int oldState) {
        lexPos = oldState;
    }
    
    public boolean hasNext() {
        return lexPos < lexList.size();
    }
    
    public Lexem getNext() {
        Lexem result = lexList.get(lexPos);
        lexPos++;
        return result;
    }
   
}
