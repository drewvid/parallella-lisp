enum ltype {PAIR, LIST, SYM, SUBR, FSUBR, LAMBDA, INT, NIL, TEE, ENV, FREE};

typedef struct DIRECTIVE fdef fdef;
typedef struct DIRECTIVE node node;

typedef struct DIRECTIVE string string;
typedef struct DIRECTIVE namestr namestr;
typedef struct DIRECTIVE edata edata;
typedef struct DIRECTIVE ememory ememory;

typedef struct DIRECTIVE stack stack;

struct DIRECTIVE node {
    node *next;
    unsigned char type;
    unsigned char marked;
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
        long long i;
        double r;
        struct {
            node *top;
            node *bindings;
        };
    };
};

struct DIRECTIVE string {
    string *next;
    char s[STRINGMAX];
};

struct DIRECTIVE namestr {
    namestr *next;
    char s[NAMESTRMAX];
};

struct DIRECTIVE stack {
    void *next;
};

struct DIRECTIVE edata {
    int id;
    int ememory_size;
    int node_size;
    int nnodes;
    int nodemem;
    int nnames;
    int namemem;
    int nstrings;
    int stringmem;
    int finished;
    char message[1024];
    char code[BANKSIZE];
    node *NULLPTR;
    node *history;
    node *freelist;
    namestr *namefreelist;
    string *stringfreelist;
    string freeStringArray[FREESTRING];
    node freeNodeArray[FREEOBJECT];
    namestr freeNameArray[FREENAME];
};

struct DIRECTIVE ememory {
    edata data[NCORES];
};
