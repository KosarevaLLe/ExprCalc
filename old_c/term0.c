#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OP 1
#define CONS 2

typedef struct {
    int type;
    int val;
    char op;
} Token;

typedef struct {
    const char *s;
    int pos;
} LStream;

int get_token(LStream *ls, Token *t)
{
    char c;
    c = ls->s[ls->pos];
    if (c == '*' || c == '/') {
        t->type = OP;
        t->op = c;
        ++ls->pos;
        return 1;
    }
    if (c >= '0' && c <= '9') {
        t->type = CONS;
        t->val = c - '0';
        ++ls->pos;
        return 1;
    }
    return c? -1: 0;
}

typedef struct tagTreeNode {
    int type;
    int val;
    char op;
    struct tagTreeNode *right, *left;
} TreeNode;

TreeNode *new_node(int type)
{
    TreeNode *t;
    t = (TreeNode *)malloc(sizeof(TreeNode));
    if (t) {
        t->type = type;
        t->right = t->left = NULL;
    }
    return t;
}

void free_tree(TreeNode *t)
{
}

TreeNode *moper(LStream *ls)
{
    LStream ls0 = *ls;
    Token t;
    if (get_token(ls, &t) != 1)
        return NULL;
    if (t.type == OP && (t.op == '*' || t.op == '/')) {
        TreeNode *n;
        n = new_node(OP);
        if (!n) {
            *ls = ls0;
            return NULL;
        }
        n->op = t.op;
        return n;
    }
    *ls = ls0;
    return NULL;
}

TreeNode *opd(LStream *ls)
{
    LStream ls0 = *ls;
    Token t;
    if (get_token(ls, &t) != 1)
        return NULL;
    if (t.type == CONS) {
        TreeNode *n;
        n = new_node(CONS);
        if (!n) {
            *ls = ls0;
            return NULL;
        }
        n->val = t.val;
        return n;
    }
    *ls = ls0;
    return NULL;
}

TreeNode *mterm(LStream *ls)
{
    TreeNode *root;
    root = opd(ls);
    if (!root)
        return NULL;
    while (1) {
        TreeNode *op, *opd2;
        LStream ls0 = *ls;
        op = moper(ls);
        if (!op)
            return root;
        opd2 = opd(ls);
        if (!opd2) {
            free_tree(op);
            *ls = ls0;
            return root;
        }
        op->left = root;
        op->right = opd2;
        root = op;
    }
}

int eval(const TreeNode *t)
{
    int l, r;
    if (t->type == CONS)
        return t->val;
    l = eval(t->left);
    r = eval(t->right);
    switch (t->op) {
        case '*': return l*r;
        case '/': return l/r;
    }
    return 0;
}

int main()
{
    // should work: "2*6/4*3"
    while (1) {
        char buf[100];
        unsigned len;
        LStream ls;
        TreeNode *t;
        if (!fgets(buf, sizeof(buf), stdin))
            break;
        buf[sizeof(buf) - 1] = 0;
        len = strlen(buf);
        if (buf[len - 1] == '\n')
            buf[len - 1] = 0;
        if (buf[0] == 'q' && buf[1] == 0)
            break;
        ls.pos = 0;
        ls.s = buf;
        t = mterm(&ls);
        printf("%d\n", eval(t));
        free_tree(t);
    }
    return 0;
}

