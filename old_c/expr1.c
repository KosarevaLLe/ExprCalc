#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "bmp.h"

typedef enum { OP, VAL, VAR, FUNC } NodeType;
typedef float ValType;

#define MAX_ID_LEN 20

typedef struct {
    char name[MAX_ID_LEN + 1];
    ValType val;
} VarEntry;

typedef struct {
    char name[MAX_ID_LEN + 1];
    int argc;
    void *func_ptr;
} FuncEntry;

typedef ValType (*ZeroFunc)(void);
typedef ValType (*UnaryFunc)(ValType);
typedef ValType (*BinaryFunc)(ValType, ValType);

typedef struct tag_ExprNode {
    struct tag_ExprNode *left, *right;
    NodeType type;
    char op;
    ValType val;
    VarEntry *var;
    FuncEntry *func;
} ExprNode;

typedef struct {
    NodeType type;
    char op;
    ValType val;
    char var[MAX_ID_LEN + 1];
} Token;

typedef struct {
    const char *buf;
    int pos;
} LexStream;

typedef struct {
    LexStream ls;
    VarEntry *vt;
    int vt_size;
    FuncEntry *ft;
    int ft_size;
} ParserInput;

void print_val(ValType x) { printf("%g", x); }
void read_val(ValType *x) { scanf("%f", x); }

void print_token(const Token *t)
{
    if (t->type == OP)
        printf("op[%c] ", t->op);
    else if (t->type == VAL) {
        printf("val[");
        print_val(t->val);
        printf("] ");
    }
    else if (t->type == VAR)
        printf("id[%s] ", t->var);
    else
        printf("err ");
}

ValType call_func(FuncEntry *f, ValType x, ValType y)
{
    if (f->argc == 0)
        return ((ZeroFunc)f->func_ptr)();
    if (f->argc == 1)
        return ((UnaryFunc)f->func_ptr)(x);
    return ((BinaryFunc)f->func_ptr)(x, y);
}

FuncEntry *func_table_find(FuncEntry *ft, int sz, const char *name)
{
    int i;
    for (i = 0; i < sz; ++i)
        if (!strcmp(ft[i].name, name))
            return &ft[i];
    return NULL;
}

void var_table_init(VarEntry *vt, int sz)
{
    int i;
    for (i = 0; i < sz; ++i)
        vt[i].name[0] = 0;
}

VarEntry *var_table_find(VarEntry *vt, int sz, const char *name)
{
    int i;
    for (i = 0; i < sz; ++i)
        if (!strcmp(vt[i].name, name))
            return &vt[i];
    return NULL;
}

int var_table_set(VarEntry *vt, int sz, const char *name, ValType x)
{
    VarEntry *v;
    v = var_table_find(vt, sz, name);
    if (!v) {
        int i;
        for (i = 0; i < sz; ++i)
            if (!vt[i].name[0]) {
                strcpy(vt[i].name, name);
                break;
            }
        v = var_table_find(vt, sz, name);
    }
    if (v) {
        v->val = x;
        return 1;
    }
    return 0;
}

int var_table_unset(VarEntry *vt, int sz, const char *name)
{
    VarEntry *v;
    v = var_table_find(vt, sz, name);
    if (v) {
        v->name[0] = 0;
        return 1;
    }
    return 0;
}

int var_table_get(VarEntry *vt, int sz, const char *name, ValType *x)
{
    VarEntry *v;
    v = var_table_find(vt, sz, name);
    if (v) {
        *x = v->val;
        return 1;
    }
    return 0;
}

/*
int is_space(char c) { return c && strchr(" \t\r\n", c) != NULL; }
int is_in(char c, char min, char max) { return c >= min && c <= max; }
int is_digit(char c) { return is_in(c, '0', '9'); }
int is_alpha(char c) { return is_in(c, 'a', 'z') || is_in(c, 'A', 'Z'); }
int is_id0(char c) { return c == '_' || is_alpha(c); }
int is_id(char c) { return c == '_' || is_alpha(c) || is_digit(c); }
char cur_c(LexStream *s) { return s->buf[s->pos]; }
int next(LexStream *s) { return ++s->pos; }
int digit(char c) { return c - '0'; }
*/

#define is_space(c) ((c) && strchr(" \t\r\n", (c)) != NULL)
#define is_in(c, min, max) ((c) >= (min) && (c) <= (max))
#define is_digit(c) is_in((c), '0', '9')
#define is_alpha(c) (is_in((c), 'a', 'z') || is_in((c), 'A', 'Z'))
#define is_id0(c) ((c) == '_' || is_alpha(c))
#define is_id(c) ((c) == '_' || is_alpha(c) || is_digit(c))
#define cur_c(s) (s)->buf[(s)->pos]
#define next(s) (++(s)->pos)
#define digit(c) ((c) - '0')

int lex_get(LexStream *s, Token *t)
{
    char c;
    for (; is_space(cur_c(s)); next(s));
    c = cur_c(s);
    if (is_digit(c)) {
        ValType a = 0;
        for (; is_digit(cur_c(s)); a = a*10 + digit(cur_c(s)), next(s));
        if (cur_c(s) == '.') {
            ValType b = 0, d = 10;
            for (next(s); is_digit(cur_c(s)); b += digit(cur_c(s))/d, next(s), d *= 10);
            a += b;
        }
        t->type = VAL;
        t->val = a;
        return 1;
    }
    if (is_id0(c)) {
        int i = 0;
        t->type = VAR;
        for (; is_id(cur_c(s)); t->var[i++] = cur_c(s), next(s));
        /* TODO: add check for MAX_ID_LEN */
        t->var[i] = 0;
        return 1;
    }
    if (c && strchr("+-*/(),", c)) {
        next(s);
        t->type = OP;
        t->op = c;
        return 1;
    }
    return c == 0? 0: -1;
}

ValType eval(const ExprNode *e)
{
    if (e->type == OP) {
        ValType l, r, x = 0;
        l = eval(e->left);
        r = eval(e->right);
        switch (e->op) {
            case '+': x = l + r; break;
            case '-': x = l - r; break;
            case '*': x = l * r; break;
            case '/': x = l / r; break;
        }
        return x;
    }
    if (e->type == VAL)
        return e->val;
    if (e->type == VAR)
        return e->var->val;
    if (e->type == FUNC) {
        ValType x = 0, y = 0;
        if (e->func->argc > 0)
            x = eval(e->left);
        if (e->func->argc > 1)
            y = eval(e->right);
        return call_func(e->func, x, y);
    }
    return 0;
}

void print_expr_postfix(const ExprNode *e)
{
    switch (e->type) {
        case OP:
            print_expr_postfix(e->left);
            print_expr_postfix(e->right);
            printf("%c ", e->op);
            break;
        case VAL:
            print_val(e->val);
            printf(" ");
            break;
        case VAR:
            printf("%s ", e->var->name);
            break;
        case FUNC:
            if (e->func->argc > 0)
                print_expr_postfix(e->left);
            if (e->func->argc > 1)
                print_expr_postfix(e->right);
            printf("%s ", e->func->name);
            break;
    }
}

void print_expr_prefix(const ExprNode *e)
{
    switch (e->type) {
        case OP:
            printf("(%c ", e->op);
            print_expr_prefix(e->left);
            print_expr_prefix(e->right);
            printf(") ");
            break;
        case VAL:
            print_val(e->val);
            printf(" ");
            break;
        case VAR:
            printf("%s ", e->var->name);
            break;
        case FUNC:
            printf("(%s ", e->func->name);
            if (e->func->argc > 0)
                print_expr_prefix(e->left);
            if (e->func->argc > 1)
                print_expr_prefix(e->right);
            printf(") ");
            break;
    }
}

int op_class(int op) { return op == '*' || op == '/'? 2: 1; }

void print_expr_normal(const ExprNode *e, char upper_op)
{
    /* FIXME: not working for '-' and '/' */
    if (e->type == VAL) {
        print_val(e->val);
        printf(" ");
    }
    else if (e->type == VAR) {
        printf("%s ", e->var->name);
    }
    else if (e->type == FUNC) {
        printf("%s(", e->func->name);
        if (e->func->argc > 0)
            print_expr_normal(e->left, 'F');
        if (e->func->argc > 1) {
            printf(", ");
            print_expr_normal(e->right, 'F');
        }
        printf(") ");
    }
    else {
        if (op_class(e->op) < op_class(upper_op))
            printf("(");
        print_expr_normal(e->left, e->op);
        printf("%c ", e->op);
        print_expr_normal(e->right, e->op);
        if (op_class(e->op) < op_class(upper_op))
            printf(") ");
    }
}

ExprNode *new_node(int type) {
    ExprNode *e;
    e = (ExprNode *)malloc(sizeof(ExprNode));
    if (!e) {
        fprintf(stderr, "Not enough memory!");
        abort();
    }
    e->type = type;
    e->left = e->right = NULL;
    return e;
}

void free_node(ExprNode *t)
{
    if (t) {
        if (t->type == OP || t->type == FUNC) {
            free_node(t->left);
            free_node(t->right);
        }
        free(t);
    }
}

typedef int (*MatchFunc)(ParserInput *, ExprNode **);

int match_expr(ParserInput *in, ExprNode **t);
int match_comma_term(ParserInput *in, ExprNode **t);

int match_brackets(ParserInput *in, ExprNode **t,
        MatchFunc inner_match, int allow_empty)
{
    LexStream ls0;
    Token c;
    int do_free = 1;
    ls0 = in->ls;
    if (lex_get(&in->ls, &c) != 1)
        return 0;
    if (c.type != OP || c.op != '(')
        goto error0;
    if (!inner_match(in, t)) {
        if (!allow_empty)
            goto error0;
        do_free = 0;
    }
    if (lex_get(&in->ls, &c) != 1)
        goto error1;
    if (c.type != OP || c.op != ')')
        goto error1;
    return 1;
error1:
    if (do_free)
        free_node(*t);
error0:
    in->ls = ls0;
    return 0;
}

int match_operand(ParserInput *in, ExprNode **t)
{
    LexStream ls0;
    Token c;

    if (match_brackets(in, t, match_expr, 0))
        return 1;
    ls0 = in->ls;
    if (lex_get(&in->ls, &c) != 1)
        return 0;
    if (c.type == VAL) {
        *t = new_node(VAL);
        (*t)->val = c.val;
        return 1;
    }
    if (c.type == VAR) {
        ExprNode *op_list;
        VarEntry *v;
        /* an ID is found */
        if (match_brackets(in, &op_list, match_comma_term, 1)) {
            FuncEntry *f;
            f = func_table_find(&in->ft[0], in->ft_size, c.var);
            /* TODO: check if not found */
            *t = new_node(FUNC);
            (*t)->func = f;
            if (f->argc == 1) {
                (*t)->left = op_list;
            }
            if (f->argc == 2) {
                (*t)->left = op_list->left;
                (*t)->right = op_list->right;
                free(op_list);
            }
            return 1;
        }
        var_table_set(&in->vt[0], in->vt_size, c.var, 0);
        v = var_table_find(&in->vt[0], in->vt_size, c.var);
        /* TODO: check if not found */
        *t = new_node(VAR);
        (*t)->var = v;
        return 1;
    }
    if (c.type == OP && c.op == '-') {
        ExprNode *opd;
        if (!match_operand(in, &opd))
            goto error0;
        *t = new_node(OP);
        (*t)->op = '-';
        (*t)->left = new_node(VAL);
        (*t)->left->val = 0;
        (*t)->right = opd;
        return 1;
    }
error0:
    in->ls = ls0;
    return 0;
}

int match_op(ParserInput *in, ExprNode **t, const char *ops)
{
    LexStream ls0;
    Token c;
    ls0 = in->ls;
    if (lex_get(&in->ls, &c) != 1)
        return 0;
    if (c.type != OP || !(c.op && strchr(ops, c.op))) {
        in->ls = ls0;
        return 0;
    }
    *t = new_node(OP);
    (*t)->op = c.op;
    return 1;
}

int match_mul_op(ParserInput *in, ExprNode **t) { return match_op(in, t, "*/"); }
int match_add_op(ParserInput *in, ExprNode **t) { return match_op(in, t, "+-"); }
int match_comma_op(ParserInput *in, ExprNode **t) { return match_op(in, t, ","); }

int match_term(ParserInput *in, ExprNode **t,
        MatchFunc match_opd, MatchFunc match_op)
{
    ExprNode *opd1;
    if (!match_opd(in, &opd1))
        return 0;
    while (1) {
        ExprNode *op, *opd2;
        LexStream ls0 = in->ls;
        if (!match_op(in, &op)) {
            *t = opd1;
            return 1;
        }
        if (!match_opd(in, &opd2)) {
            *t = opd1;
            free_node(op);
            in->ls = ls0;
            return 1;
        }
        op->left = opd1;
        op->right = opd2;
        opd1 = op;
    }
}

int match_mul_term(ParserInput *in, ExprNode **t)
    { return match_term(in, t, match_operand, match_mul_op); }

int match_add_term(ParserInput *in, ExprNode **t)
    { return match_term(in, t, match_mul_term, match_add_op); }

int match_comma_term(ParserInput *in, ExprNode **t)
    { return match_term(in, t, match_add_term, match_comma_op); }

int match_expr(ParserInput *in, ExprNode **t)
    { return match_add_term(in, t); }


ValType min(ValType x, ValType y)
{
    return x < y? x: y;
}

ValType my_sin(ValType x)
{
    return sin(x);
}

ValType my_pi(void)
{
    return 3.14159265;
}

void test_eval(void)
{
    ExprNode a, b, c, d, e, op, op2;
    VarEntry x0;
    FuncEntry f;
    strcpy(x0.name, "x0");
    x0.val = 4;
    strcpy(f.name, "min");
    f.argc = 2;
    f.func_ptr = min;
    a.type = VAR;
    a.var = &x0;
    b.type = VAL;
    b.val = 7;
    c.type = VAL;
    c.val = 2;
    op2.type = OP;
    op2.op = '+';
    op2.left = &b;
    op2.right = &c;
    op.type = OP;
    op.op = '*';
    op.left = &a;
    op.right = &op2;
    e.type = VAL;
    e.val = 50;
    d.type = FUNC;
    d.func = &f;
    d.left = &e;
    d.right = &op;
    assert(eval(&d) == 36);
    print_expr_postfix(&d);
    printf("\n");
    print_expr_prefix(&d);
    printf("\n");
    print_expr_normal(&d, '+');
    printf("\n");
}

void test_lex(void)
{
    const char *e = "1 * 124.5678 * min(x, y) -(3+5 / my_var\t)/ 1+ X0@ 2 - 3";
    LexStream s;
    Token t;
    s.buf = e;
    s.pos = 0;
    while (lex_get(&s, &t) == 1)
        print_token(&t);
    printf("\n");
}

void test_match(void)
{
    const char *e = "1/-4.0 + my_var * ( (33/3) - 4 * 5 )";
    ParserInput in;
    VarEntry vt[10];
    ExprNode *t;
    int x;
    in.ls.buf = e;
    in.ls.pos = 0;
    in.vt = &vt[0];
    in.vt_size = 10;
    var_table_init(vt, 10);
    x = match_expr(&in, &t);
    printf("%d\n", x);
    if (x) {
        print_expr_prefix(t);
        printf("\n");
        var_table_set(vt, 10, "my_var", 2);
        printf("=");
        print_val(eval(t));
        printf("\n");
        var_table_set(vt, 10, "my_var", 3);
        printf("=");
        print_val(eval(t));
        printf("\n");
        free_node(t);
    }
}


#define WIDTH 80
#define HEIGHT 25

void test_draw(ExprNode *e, VarEntry *v)
{
    BitMap bmp;
    if (bmp_init(&bmp, WIDTH, HEIGHT)) {
        bmp_rect(&bmp, 0, 0, WIDTH, HEIGHT, '*');
        float x, y;
        for (x = 0; x < 7; x += 0.01) {
            int ix, iy;
            v->val = x;
            y = eval(e);
            ix = my_round(x * (WIDTH / 7.0));
            iy = my_round(- y * (HEIGHT / 3.0) + HEIGHT / 2.0);
            bmp_put(&bmp, ix, iy, 'O');
        }
        bmp_print(stdout, &bmp, 0, 0);
        bmp_fin(&bmp);
    }
    else {
        fprintf(stderr, "Can't get memory for bitmap\n");
    }
}

#define LINE_MAX 30
#define VT_MAX 3
#define FT_MAX 3
void test_interact(void)
{
    FuncEntry ft[FT_MAX];
    strcpy(ft[0].name, "min");
    ft[0].argc = 2;
    ft[0].func_ptr = min;
    strcpy(ft[1].name, "sin");
    ft[1].argc = 1;
    ft[1].func_ptr = my_sin;
    strcpy(ft[2].name, "pi");
    ft[2].argc = 0;
    ft[2].func_ptr = my_pi;

    while (1) {
        char line[LINE_MAX];
        int i, x, j;
        ParserInput in;
        Token c;
        VarEntry vt[VT_MAX];
        ExprNode *t;
        
        printf("input expression ('q' to quit):\n");
        fflush(stdin);
        if (fgets(line, LINE_MAX, stdin) == NULL)
            line[0] = 0;
        line[LINE_MAX - 1] = 0;
        for (i = strlen(line) - 1; i >= 0 && is_space(line[i]); --i);
        line[i + 1] = 0;
        if (!line[0])
            continue;
        if (!strcmp(line, "q"))
            break;
        printf("you entered: \"%s\"\n", line);
        in.ls.buf = line;
        in.ls.pos = 0;
        in.vt = &vt[0];
        in.vt_size = VT_MAX;
        in.ft = &ft[0];
        in.ft_size = FT_MAX;
        var_table_init(vt, VT_MAX);
        x = match_expr(&in, &t);
        i = in.ls.pos;
        if (!x || lex_get(&in.ls, &c) != 0) {
            printf("              ");
            for (j = 0; j < i; printf(" "), ++j);
            printf("^\n");
            printf("parse error, pos: %d\n", i + 1);
        }
        else {
            print_expr_prefix(t);
            printf("\n");
            for (i = 0; i < VT_MAX; ++i) {
                if (vt[i].name[0]) {
                    printf("var %s=?\n", vt[i].name);
                    read_val(&vt[i].val);
                }
            }
            printf("result=");
            print_val(eval(t));
            printf("\n");
            test_draw(t, &vt[0]);
            free_node(t);
        }
        printf("\n");
    }
}

int main()
{
    test_eval();
    test_lex();
    test_match();
    test_interact();
    return 0;
}
