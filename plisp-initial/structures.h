enum ltype {PAIR, LIST, SYM, SUBR, FSUBR, LAMBDA, INT, NIL, TEE, ENV};

typedef struct DIRECTIVE fdef fdef;
typedef struct DIRECTIVE node node;

typedef struct DIRECTIVE string string;
typedef struct DIRECTIVE namestr namestr;
typedef struct DIRECTIVE edata edata;
typedef struct DIRECTIVE ememory ememory;

struct DIRECTIVE node {
    enum ltype type;
    union {
        namestr *name;
        struct {
            node *car;
            node *cdr;
        };
        struct {
            namestr *fname;
            node *(*fn)(node *, node *);
        };
        struct {
            node *args;
            node *body;
        };
        long i;
        double r;
        struct {
            node *top;
            node *bindings;
        };
    };
};

struct DIRECTIVE string {
    char s[STRINGMAX];
};

struct DIRECTIVE namestr {
    char s[NAMESTRMAX];
};

struct DIRECTIVE fdef {
    const char *fname;
    int type;
    node *(*fn)(node *, node *);
};

struct DIRECTIVE edata {
    int row;
    int col;
    int id;
    int finished;
    char code[BANKSIZE];
    char output[BANKSIZE];
    string freeStringArray[FREESTRING];
    node freeNodeArray[FREEOBJECT];
    namestr freeNameArray[FREENAME];
};

struct DIRECTIVE ememory {
    char code[BANKSIZE];
    edata data[NCORES];
};
