package study;

public class LexemIdentifier extends Lexem {
    
    private String id;
    
    public String getId() {
        return id;
    }

    public LexemIdentifier(String id) {
        super(Type.Identifier);
        this.id = id;
    }

    @Override
    public String toString() {
        return id;
    }
}
