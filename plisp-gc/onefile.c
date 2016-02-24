#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#if EPIPHANY
#include "e-lib.h"
#endif

#if EPIPHANY
#define DIRECTIVE __attribute__((aligned(8)))
#else
#define DIRECTIVE
#endif

#define	TRUE	1
#define	FALSE	0

#define NCORES 16
#define BANKSIZE 8192
#define STRINGMAX BANKSIZE
#define NAMESTRMAX 32
#define LINELENGTH 1024

#define FREESTRING 10
#define FREEOBJECT 10000
#define FREENAME 2000

#define BUF_ADDRESS 0x8f000000

#define is ==
#define isnt !=
#define not !
#define and &&
#define or ||
#define in ,

#define forlist(...) forlist_xp(forlist_in, (__VA_ARGS__))
#define forlist_in(X, S) for(node *X = S; X isnt NULLPTR; nextptr(X))
#define forlist_xp(X, A) X A

#define forlist2(...) forlist_xp(forlist_in2, (__VA_ARGS__))
#define forlist_in2(X, S, XX, SS) for(node *X = S, *XX = SS; X isnt NULLPTR and XX isnt NULLPTR; nextptr(X), nextptr(XX))

#define forheap(...) forlist_xp(forheap_in, (__VA_ARGS__))
#define forheap_in(X, S) for(node *X = S; X isnt NULLPTR; X = next(X))

#define PERMANENT 		2

#define EOS             '\0'
#define EOSP(X)         ((X) is EOS)
#define nullp(X)        ((X) is NULLPTR)

#define ppval(X)        (**(X))
#define ppdec(X)        ((*(X))--)
#define ppvalinc(X)     (*(*(X))++)
#define ppinc(X)     	(*(X))++

#define nextptr(X)      ((X) = cdr(X))
#define rplaca(X,Y)     ((X)->car = (Y))
#define rplacd(X,Y)     ((X)->cdr = (Y))

#define next(X)         ((X)->next)
#define type(X)         ((X)->type)
#define funcname(X)     ((X)->fname->s)
#define funcptr(X)      ((X)->fn)
#define largs(X)        ((X)->args)
#define lbody(X)        ((X)->body)
#define ival(X)        	((X)->i)
#define ebindings(X)    ((X)->bindings)

#define consp(X)        ((X) and (X)->type is LIST)
#define pairp(X)        ((X) and (X)->type is PAIR)
#define symp(X)         ((X) and (X)->type is SYM)
#define subrp(X)        ((X) and (X)->type is SUBR)
#define fsubrp(X)       ((X) and (X)->type is FSUBR)
#define lambdap(X)      ((X) and (X)->type is LAMBDA)
#define intp(X)      	((X) and (X)->type is INT)
#define nilp(X)         ((X) and (X)->type is NIL)
#define teep(X)         ((X) and (X)->type is TEE)

#define car(X)          ((X)->car)
#define cdr(X)          ((X)->cdr)
#define caar(X)         (car(car(X)))
#define cadar(X)        (car(cdr(car(X))))

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
    int row;
    int col;
    int id;
    int finished;
    node *NULLPTR;
    node *history;
    node *freelist;
    namestr *namefreelist;
    string *stringfreelist;
    char code[BANKSIZE];
    string freeStringArray[FREESTRING];
    node freeNodeArray[FREEOBJECT];
    namestr freeNameArray[FREENAME];
};

struct DIRECTIVE ememory {
    edata data[NCORES];
};

node *tee;
node *nil;
node *NULLPTR;
node *globals;
node *history;
node *freelist;
string *stringfreelist;
namestr *namefreelist;
node *allocated;

string *freeStringArray;
node *freeNodeArray;
namestr *freeNameArray;

char *input, *output, *result;
int freeStringIndex = 0;
int freeNodeIndex = 0;
int freeNameIndex = 0;

int nnodes = 0;
int nodemem= 0 ;
int nnames = 0;
int namemem = 0;
int nstrings = 0;
int stringmem = 0;

ememory *memory;
int id;


/* fl-device.c */
void pr(node *cell);
void addInt(long long i);
void addString(char *s);
void addValue(char *s, long long i);
char *scopy(char *s1, const char *s2);
long long stoi(const char *c);
int slen(char *s);
void prStats(void);
char *readFile(char *fileName);
void createFreelist(ememory *memory, int rows, int cols);
void createStringFreelist(ememory *memory, int rows, int cols);
void createNameFreelist(ememory *memory, int rows, int cols);
int coreID(unsigned int *row, unsigned int *col);
void coreInit(int argc, char *argv[]);
void nl(void);
void prpair(node *l);
void print(node *l);
void setflag(void);
string *smalloc(void);
string *string_malloc(void);
void string_free(string *n);
namestr *nmalloc(void);
namestr *name_malloc(void);
void name_free(namestr *n);
node *omalloc(void);
node *node_malloc(void);
void node_free(node *n);
void pushFree(stack *ptr, stack **stk);
stack *popFree(stack **stk);
void mark_expr(node *o, unsigned char persistence);
void release_node(node *o);
void free_unmarked(node **allocated);
node *newnode(enum ltype type);
node *sym(char *val);
node *cons(node *head, node *tail);
node *pair(node *head, node *tail);
node *func(node *(*fn)(node *, node *), enum ltype type);
node *lambda(node *args, node *sexp);
node *integer(long long num);
node *newcontext(node *bindings, node *top);
node *lastcell(node *list);
node *append(node *list, node *obj);
node *concat(node *l1, node *l2);
void atl(node **l, node *item);
void add_pair(node *head, node *tail, node **list);
void pushNode(node *item, node **stk);
node *popNode(node **stk);
node *nextarg(node **pargs);
char *name(node *o);
int strequal(char *s1, char *s2);
node *assq(char *key, node *list);
node *lookupsym(char *name, node *env);
node *make_env(node *vars, node *vals, node *env);
node *el_car(node *args, node *env);
node *el_cdr(node *args, node *env);
node *el_nilp(node *args, node *env);
node *el_quote(node *args, node *env);
node *el_cons(node *args, node *env);
node *el_cond(node *args, node *env);
node *el_if(node *args, node *env);
node *el_lambda(node *args, node *env);
node *el_label(node *args, node *env);
node *el_ldefine(node *args, node *env);
node *el_loop(node *args, node *env);
node *el_block(node *args, node *env);
node *el_progn(node *args, node *env);
node *el_print(node *args, node *env);
node *el_terpri(node *args, node *env);
node *binary(node *args, int fcn);
node *compare(node *args, int fcn);
node *el_lessthan(node *args, node *env);
node *el_greaterthan(node *args, node *env);
node *el_eq(node *args, node *env);
node *el_plus(node *args, node *env);
node *el_minus(node *args, node *env);
node *el_times(node *args, node *env);
node *el_divide(node *args, node *env);
node *el_atom(node *args, node *env);
node *el_equal(node *args, node *env);
node *el_lessthanequal(node *args, node *env);
node *el_greaterthanequal(node *args, node *env);
node *el_defun(node *args, node *env);
node *el_consp(node *args, node *env);
node *el_funcall(node *args, node *env);
node *el_zerop(node *args, node *env);
node *el_sub1(node *args, node *env);
node *el_add1(node *args, node *env);
node *el_numberp(node *args, node *env);
node *el_or(node *args, node *env);
node *el_and(node *args, node *env);
node *el_not(node *args, node *env);
node *el_setflag(node *args, node *env);
node *el_id(node *args, node *env);
void init_lisp(void);
int getChar(char **s);
int ungetChar(char **s);
char *getToken(char **s, char *token);
node *tokenize(char **code);
int equal(node *sym, char *s2);
int is_valid_int(char *str);
node *makeNode(node *n);
node *_parse(node **code, char *terminator);
node *parse(node **code);
node *parse_string(char **input);
int length(node *l);
node *evlambda(node *vals, node *expr, node *env);
node *evform(node *fnode, node *exp, node *env);
node *evsym(node *exp, node *env);
node *eval_list(node *sexp, node *env);
node *eval(node *input, node *env);
void REPL(char *input);
int main(int argc, char *argv[]);


#define BUF_ADDRESS 0x8f000000

//
// Add items to the history
//
void pr(node *cell) {
    atl(&history, cell);
}

void addInt(long long i) {
    pr(integer(i));
}

void addString(char *s) {
    pr(sym(s));
}

void addValue(char *s, long long i) {
    addString(s); addInt(i);
}

//
// local version of strcpy
//
char *scopy(char *s1, const char *s2) {
    char *s = s1;
    while ((*s++ = *s2++) != '\0')
        ;
    *s = '\0';
    return (s1);
}

//
// local version of atoi
//
long long stoi(const char *c)
{
    long long value = 0;
    int sign = 1;
    if( *c == '+' || *c == '-' ) {
        if( *c == '-' ) sign = -1;
        c++;
    }
    while (isdigit(*c)) {
        value *= 10;
        value += (int) (*c-'0');
        c++;
    }
    return (value * sign);
}

//
// length of a string
//
int slen(char *s) {
   int c = 0;
   while(*(s+c))
      c++;
   return c;
}

//
// add memory stats to the history list
//
void prStats() {
    addValue("id: ", id);
    addValue("memory: ", sizeof(ememory));
    addValue("node size: ", sizeof(node));
    addValue("nnodes: ", nnodes);
    addValue("nodemem: ", nodemem);
    addValue("nnames: ", nnames);
    addValue("namemem: ", namemem);
    addValue("nstrings: ", nstrings);
    addValue("stringmem: ", stringmem);
}

#if EPIPHANY

//
// get the core ID
//
int coreID(unsigned int *row, unsigned int *col) {

    e_coreid_t coreid;

    coreid = e_get_coreid();
    coreid = coreid - e_group_config.group_id;
    *row = (coreid >> 6) & 0x3f;
    *col = coreid & 0x3f;

    return((*row * 4) + *col);
}

//
// Initilaize core memory
//
void coreInit(int argc, char *argv[]) {

    memory = (ememory *)(BUF_ADDRESS);

    freeStringArray = &memory->data[id].freeStringArray[0];
    freeNodeArray = &memory->data[id].freeNodeArray[0];
    freeNameArray = &memory->data[id].freeNameArray[0];

    freelist = freeNodeArray;
    stringfreelist = freeStringArray;
    namefreelist = freeNameArray;

}

//
// Print memory stats and store a pointer to the history list.
// Put the processor in idle mode
//
void setflag() {
    unsigned *d;

    if(nnodes < FREEOBJECT && nnames < FREENAME)
        prStats();

    memory->data[id].NULLPTR = NULLPTR;
    memory->data[id].history = history;
    memory->data[id].finished = 1;

    d = (unsigned *) 0x7000;
    (*(d)) = 0x00000001;

    __asm__ __volatile__("idle");
}

#else

//
// Read a text file
//
char *readFile(char *fileName) {
    FILE *file = fopen(fileName, "r");
    if(!file) {
        fprintf(stderr, "%s\n", "file not found");
        exit(-1);
    }
    string *code;
    size_t n = 0;
    int c;
    if (file == NULL) return NULL;
    code = smalloc();
    while ((c = fgetc(file)) != EOF)
        code->s[n++] = (char)c;
    code->s[n] = '\0';
    return code->s;
}

//
// Create the freelist
//
void createFreelist(ememory *memory, int rows, int cols) {
    int id, k;
    node *freeNodeArray;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            id = (cols * i) + j;
            freeNodeArray = memory->data[id].freeNodeArray;
            for (k = 0; k < FREEOBJECT - 1; k++) {
                freeNodeArray[k].next = &freeNodeArray[k + 1];
                freeNodeArray[k].type = FREE;
            }
            freeNodeArray[FREEOBJECT - 1].next = NULL;
        }
    }
}

void createStringFreelist(ememory *memory, int rows, int cols) {
    int id, k;
    string *freeStringArray;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            id = (cols * i) + j;
            freeStringArray = memory->data[id].freeStringArray;
            for (k = 0; k < FREESTRING - 1; k++) {
                freeStringArray[k].next = &freeStringArray[k + 1];
            }
            freeStringArray[FREESTRING - 1].next = NULL;
        }
    }
}

void createNameFreelist(ememory *memory, int rows, int cols) {
    int id, k;
    namestr *freeNameArray;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            id = (cols * i) + j;
            freeNameArray = memory->data[id].freeNameArray;
            for (k = 0; k < FREENAME - 1; k++) {
                freeNameArray[k].next = &freeNameArray[k + 1];
            }
            freeNameArray[FREENAME - 1].next = NULL;
        }
    }
}

//
// Generate a core ID for testing
//
int coreID(unsigned int *row, unsigned int *col) {

    *row = 1;
    *col = 1;

    return((*row * 4) + *col);
}

//
// Initialize globals
//
void coreInit(int argc, char *argv[]) {
    char *code;

    memory = (ememory *)calloc(1, sizeof(ememory));
    if(!memory) {
        fprintf(stderr, "%s\n", "out of memory in init_ememory");
        exit(-1);
    }
    freeStringArray = &memory->data[id].freeStringArray[0];
    freeNodeArray = &memory->data[id].freeNodeArray[0];
    freeNameArray = &memory->data[id].freeNameArray[0];

    freelist = freeNodeArray;
    stringfreelist = freeStringArray;
    namefreelist = freeNameArray;

    if (argc == 2)
        code = readFile(argv[1]);
    else
        code = readFile("testfuncs.lisp");

    scopy(memory->data[id].code, code);

    createFreelist(memory, 4, 4);
    createNameFreelist(memory, 4, 4);
    createStringFreelist(memory, 4, 4);

}

//
// Printing routines
//

void nl(void) {
    printf("\n");
}

void prpair(node *l) {
    printf("%s", "(");
    print(car(l));
    printf("%s", ".");
    print(cdr(l));
    printf("%s", ")");
}

void print(node *l) {
    if (nullp(l))
        printf(" NULL ");
    else if (teep(l))
         printf(" t ");
    else if (nilp(l))
         printf(" nil ");
    else if (symp(l)) // symbol
        printf(" %s ", name(l));
    else if (intp(l)) // integer
        printf(" %lld ", ival(l));
    else if(lambdap(l)) { // lambda expression
        printf(" #lambda ");
        print(largs(l));
        print(lbody(l));
    } else if (subrp(l))
        printf(" subr ");
    else if (fsubrp(l))
        printf(" fsubr ");
    else if (pairp(l)) // pair
        prpair(l);
    else if (consp(l)) {
        if (not nullp(cdr(l)) and not consp(cdr(l))) // untyped dotted pair
            prpair(l);
        else { // list
            printf("( ");
            for (node *ptr = l; ptr != NULLPTR; ptr = cdr(ptr))
                print(car(ptr));
            printf(" )");
        }
    } else
        printf(" Something went wrong \n");
}

//
// Print out the history list and exit
//
void setflag() {

    if(nnodes < FREEOBJECT && nnames < FREENAME) {
        prStats();
        forlist (ptr in history) {
            print(car(ptr));
            printf("\n");
        }
    } else {
        printf("OUT OF MEMORY\n");
        printf("nnodes %d\n", nnodes);
        printf("nnames %d\n", nnames);
        printf("nstrings %d\n", nstrings);
    }

    exit(0);
}

#endif

// LISP Code
//
// Structure allocation
//
string *smalloc(void) {
    if (stringfreelist != NULL)
        return (string *)popFree((stack **)(&stringfreelist));
    setflag();
    return NULL;
}

string *string_malloc() {
    stringmem += sizeof(string);
    nstrings += 1;
    return smalloc();
}

void string_free(string *n) {
    stringmem -= slen(n->s) + 1;
    nstrings -= 1;
    pushFree((stack *)n, (stack **)(&stringfreelist));
}

namestr *nmalloc(void) {
    if (namefreelist != NULL)
        return (namestr *)popFree((stack **)(&namefreelist));
    setflag();
    return NULL;
}

namestr *name_malloc() {
    namemem += sizeof(namestr);
    nnames += 1;
    return nmalloc();
}

void name_free(namestr *n) {
    namemem -= sizeof(namestr);
    nnames -= 1;
    pushFree((stack *)n, (stack **)(&namefreelist));
}

node *omalloc(void) {
    if (freelist != NULL)
        return (node *)popFree((stack **)(&freelist));
    setflag();
    return NULL;
}

node *node_malloc() {
    nodemem += sizeof(node);
    nnodes += 1;
    return (node *)omalloc();
}

void node_free(node *n) {
    nodemem -= sizeof(node);
    nnodes -= 1;
    pushFree((stack *)n, (stack **)(&freelist));
}

//
// GC
//
void pushFree(stack *ptr, stack **stk) {
    ptr->next = *stk;
    *stk = ptr;
}

stack *popFree(stack **stk) {
    if (*stk is NULL) return NULL;
    stack *item = *stk;
    *stk = (*stk)->next;
    item->next = NULL;
    return item;
}

void mark_expr(node *o, unsigned char persistence) {
    if ( nullp(o) ) return;
    if (pairp(o) or consp(o)) {
        if (not nullp(o)) mark_expr(o->car, persistence);
        if (not nullp(o)) mark_expr(o->cdr, persistence);
    } else if (lambdap(o)) {
        if (not nullp(o)) mark_expr(o->args, persistence);
        if (not nullp(o)) mark_expr(o->body, persistence);
    }
    if (o->marked <= 1)
        o->marked = persistence;
    return;
}

void release_node(node *o) {
    if ( nullp(o) ) return;

    if(symp(o)) name_free(o->name);
    else if (subrp(o) or fsubrp(o)) name_free(o->fname);

    node_free(o);
}

void free_unmarked(node **allocated) {
    node *cpy, **ptr = allocated;
    while (*ptr isnt NULL) {
        cpy = *ptr;
        if (cpy->marked is 0) {
            *ptr = next(cpy);
            release_node(cpy);
        } else {
            ptr = &next(cpy);
            if (cpy->marked is 1)
                cpy->marked = 0;
        }
    }
}

//
// node allocation
//
node *newnode(enum ltype type) {
    node *n;
    n = (node *) node_malloc();
    n->type = type;
    n->marked = 0;
    next(n) = allocated;
    allocated = n;
    return n;
}

node *sym(char *val) {
    node *ptr = newnode(SYM);
    namestr *name = name_malloc();
    scopy(name->s, val);
    ptr->name = name;
    return ptr;
}

node *cons (node *head, node *tail) {
    node *ptr = newnode(LIST);
    rplaca(ptr, head);
    rplacd(ptr, tail);
    return ptr;
}

node *pair(node *head, node *tail) {
    node *ptr = cons(head, tail);
    type(ptr) = PAIR;
    return ptr;
}

node *func (node* (*fn)(node*, node*), enum ltype type) {
    node *ptr = newnode(type);
    funcptr(ptr) = fn;
    return ptr;
}

node *lambda (node *args, node *sexp) {
    node *ptr = newnode(LAMBDA);
    largs(ptr) = args;
    lbody(ptr) = sexp;
    return ptr;
}

node* integer(long long num) {
    node *ptr = newnode(INT);
    ival(ptr) = num;
    return ptr;
}

node *newcontext(node *bindings, node *top) {
    node *env = newnode(ENV);
    if(ebindings(top))
        bindings = concat(bindings, ebindings(top));
    ebindings(env) = bindings;
    return env;
}

//
// list creation/access
//
node *lastcell(node *list) {
    node *ptr = list;
    while (consp(ptr) and not nullp(cdr(ptr)))
        nextptr(ptr);
    return ptr;
}

node *append(node *list, node *obj) {
    node *ptr = lastcell(list);
    rplacd(ptr, cons(obj, NULLPTR));
    return list;
}

node *concat(node *l1, node *l2) {
    node *ptr = lastcell(l1);
    rplacd(ptr, l2);
    return l1;
}

void atl(node **l, node *item) {
    if (*l is NULLPTR) // Initialize the list
        *l = cons(item, NULLPTR);
    else if ((*l)->type is LIST) // Append item to the list
        append(*l, item);
}

void add_pair(node *head, node *tail, node **list) {
    *list = cons(pair(head, tail), *list);
}

void pushNode(node *item, node **stk) {
    node *ptr = cons(item, NULLPTR);
    ptr->cdr = *stk;
    *stk = ptr;
}

node *popNode(node **stk) {
    if (*stk is NULLPTR)
        return NULLPTR;
    node *item = *stk;
    *stk = (*stk)->cdr;
    item->cdr = NULLPTR;
    return item;
}

//
// argument/struture access
//
node *nextarg(node **pargs) {
    if (not consp(*pargs))
        pr(sym("too few arguments\n"));
    node *arg = car(*pargs);
    *pargs = cdr(*pargs);
    return arg;
}

char *name(node *o) {
    if(not symp(o))
        return "";
    return o->name->s;
}

//
// Symbol lookup/creation - environment creation
//
int strequal(char *s1, char *s2) {  // compare 2 strings
    while (*s1 == *s2++)
        if (*s1++ == '\0')
            return (0);
    return 1;
}

node *assq(char *key, node *list) {
    forlist (ptr in list)
        if (strequal(key, name(caar(ptr))) is 0)
            return car(ptr);
    return NULLPTR;
}

node *lookupsym(char *name, node *env) {
    node *fptr = NULLPTR;
    if (ebindings(env) isnt NULLPTR)
        fptr = assq(name, ebindings(env));
    if(nullp(fptr))
        fptr = assq(name, globals);
    return not nullp(fptr)? cdr(fptr) : NULLPTR;
}

node *make_env(node *vars, node *vals, node *env) {
    node *nenv = NULLPTR;
    forlist2 (pvar in vars, pval in vals)
        add_pair(car(pvar), eval(car(pval), env), &nenv);
    return newcontext(nenv, env);
}

//
// builtins
//
node *el_car (node *args, node *env) {
    return car(nextarg(&args));
}

node *el_cdr (node *args, node *env) {
    node *tail = cdr(nextarg(&args));
    return nullp(tail)? nil : tail;
}

node *el_nilp(node *args, node *env) {
    node *arg = nextarg(&args);
    return nilp(arg)? tee : nil;
}

node *el_quote (node *args, node *env) {
    return nextarg(&args);
}

node *el_cons (node *args, node *env) {
    node * head = nextarg(&args);
    node * tail = nextarg(&args);
    if (nilp(tail))
        tail = NULLPTR;
    return cons(head, tail);
}

node *el_cond(node *args, node *env) {
    forlist (ptr in args)
        if (not nilp(eval(caar(ptr), env)))
            return eval(cadar(ptr), env);
    return nil;
}

node *el_if(node *args, node *env) {
    node *cond, *iftrue;
    cond = eval(nextarg(&args), env);
    iftrue = nextarg(&args);
    return (type(cond) != NIL) ? eval(iftrue, env) : eval(nextarg(&args), env);
}

node *el_lambda (node *args, node *env) {
    node *lambda_args = nextarg(&args), *lambda_body;
    if (length(args) > 1)
        lambda_body = concat(cons(sym("progn"), NULLPTR), args);
    else
        lambda_body = nextarg(&args);
    return lambda(lambda_args, lambda_body);
}

node *el_label (node *args, node *env) {
    node *name1 = nextarg(&args), *val;
    node *def = assq(name(name1), globals);
    val = eval(nextarg(&args), env);
    if(not nullp(def))
        cdr(def) = val;
    else
        add_pair(name1, val, &globals);
    return val;
}

node *el_ldefine(node *args, node *env) {
    node *name1 = nextarg(&args), *val;
    val = eval(nextarg(&args), env);
    if (nullp(ebindings(env))) {
        env->bindings = cons(pair(name1, val), NULLPTR);
        return val;
    }
    node *def = assq(name(name1), ebindings(env));
    if (not nullp(def))
        def->cdr = val;
    else
        append(ebindings(env), pair(name1, val));
    return val;
}

node *el_loop(node *args, node *env) {
    node *cond = nextarg(&args), *val = NULLPTR;
    while(type(eval(cond, env)) is TEE) {
        forlist (ptr in args)
            val = eval(car(ptr), env);
    }
    return val;
}

node *el_block(node *args, node *env) {
    node *res = NULLPTR;
    forlist (ptr in args)
        atl(&res, eval(car(ptr), env));
    return res;
}

node *el_progn(node *args, node *env) {
    node *res = nil;
    forlist (ptr in args)
        res = eval(car(ptr), env);
    return res;
}

node *el_print(node *args, node *env) {
    pr(args);
    return nil;
}

node *el_terpri(node *args, node *env) {
    return nil;
}

node *binary(node *args, int fcn) {
    long long i = ival(nextarg(&args));
    forlist (ptr in args) {
        switch(fcn) {
        case '+':
            i += ival(car(ptr));
            break;
        case '-':
            i -= ival(car(ptr));
            break;
        case '*':
            i *= ival(car(ptr));
            break;
        case '/':
            i /= ival(car(ptr));
            break;
        default:
            break;
        }
    }
    return(integer(i));
}

node *compare(node *args, int fcn) {
    long long i = ival(nextarg(&args)), icmp = 0;
    forlist (ptr in args) {
        icmp = i - ival(car(ptr));
        switch(fcn) {
        case '<':
            icmp = (icmp < 0);
            break;
        case 'l':
            icmp = (icmp <= 0);
            break;
        case '=':
            icmp = (icmp is 0);
            break;
        case '>':
            icmp = (icmp > 0);
            break;
        case 'g':
            icmp = (icmp >= 0);
            break;
        default:
            break;
        }
        if (not icmp) break;
    }
    return (icmp ? tee : nil);
}

node *el_lessthan(node *args, node *env) {
    return compare(args, '<');
}

node *el_greaterthan(node *args, node *env) {
    return compare(args, '>');
}

node *el_eq(node *args, node *env) {
    return compare(args, '=');
}

node *el_plus(node *args, node *env) {
    return binary(args, '+');
}

node *el_minus(node *args, node *env) {
    return binary(args, '-');
}

node *el_times(node *args, node *env) {
    return binary(args, '*');
}

node *el_divide(node *args, node *env) {
    return binary(args, '/');
}

//
// New and modified primitives
//
node *el_atom (node *args, node *env) {
    node *res = tee, *head;
    while (args isnt NULLPTR and consp(args)) {
        head = car(args);
        if (not (nilp(head) || teep(head) || intp(head) || symp(head)))
            res = nil;
        args = cdr(args);
    }
    return res;
}

node *el_equal (node *args, node *env) {
    node *first = nextarg(&args);
    node *second = nextarg(&args);
    if ((nilp(first) and nilp(second)) or (teep(first) and teep(second)))
        return tee;
    else if (symp(first) and symp(second))
        return strequal(name(first), name(second)) is 0? tee : nil;
    else if (intp(first) and intp(second))
        return ival(first) == ival(second) ? tee : nil;
    else
        return nil;
}

node *el_lessthanequal(node *args, node *env) {
    return compare(args, 'l');
}

node *el_greaterthanequal(node *args, node *env) {
    return compare(args, 'g');
}

node *el_defun(node *args, node *env) {
    node *name1 = nextarg(&args);
    node *lambda_args = nextarg(&args);
    node *lam = lambda(lambda_args, nextarg(&args));
    node *def = assq(name(name1), globals);
    if(not nullp(def))
        cdr(def) = lam;
    else
        add_pair(name1, lam, &globals);
    return lam;
}

node *el_consp (node *args, node *env) {
    node *val = nextarg(&args);
    return consp(val) ? tee : nil;
}

node *el_funcall(node *args, node *env) {
    node *funcname = eval(nextarg(&args), env);
    node *expr = concat(cons(funcname, NULLPTR), args);
    return eval(expr, env);
}

node *el_zerop(node *args, node *env) {
    node *val = nextarg(&args);
    return ival(val) == 0 ? tee : nil;
}

node *el_sub1(node *args, node *env) {
    node *val = nextarg(&args);
    return integer(ival(val) - 1);
}

node *el_add1(node *args, node *env) {
    node *val = nextarg(&args);
    return integer(ival(val) + 1);
}

node *el_numberp(node *args, node *env) {
    node *val = nextarg(&args);
    return intp(val) ? tee : nil;
}

node *el_or(node *args, node *env) {
    forlist (item in args) {
        node *val =  eval(car(item), env);
        if (teep(val))
            return tee;
    }
    return nil;
}

node *el_and(node *args, node *env) {
    forlist (item in args) {
        node *val =  eval(car(item), env);
        if (!teep(val))
            return nil;
    }
    return tee;
}

node *el_not(node *args, node *env) {
    node *val = nextarg(&args);

    if (nilp(val))
        return tee;
    if (intp(val)) {
        if(ival(val) != 0)
            return tee;
    }

    return nil;
}

node *el_setflag(node *args, node *env) {
    pr(args);
    setflag();
    return nil;
}

node *el_id(node *args, node *env) {
    return integer(id);
}


//
// init
//
void init_lisp() {
    allocated = NULL;
    NULLPTR = sym("NULLPTR");
    globals = NULLPTR;
    history = NULLPTR;
    add_pair(sym("eval") ,      func(&eval, SUBR), &globals);
    add_pair(sym("quote") ,     func(&el_quote, FSUBR), &globals);
    add_pair(sym("car"),        func(&el_car, SUBR), &globals);
    add_pair(sym("cdr"),        func(&el_cdr, SUBR), &globals);
    add_pair(sym("nilp"),       func(&el_nilp, SUBR), &globals);
    add_pair(sym("cons"),       func(&el_cons, SUBR), &globals);
    add_pair(sym("loop"),       func(&el_loop, FSUBR), &globals);
    add_pair(sym("block"),      func(&el_block, FSUBR), &globals);
    add_pair(sym("progn"),      func(&el_progn, FSUBR), &globals);
    add_pair(sym("equal"),      func(&el_equal, SUBR), &globals);
    add_pair(sym("atom"),       func(&el_atom, SUBR), &globals);
    add_pair(sym("if"),         func(&el_if, FSUBR), &globals);
    add_pair(sym("cond"),       func(&el_cond, FSUBR), &globals);
    add_pair(sym("lambda"),     func(&el_lambda, FSUBR), &globals);
    add_pair(sym("label"),      func(&el_label, FSUBR), &globals);
    add_pair(sym("define"),     func(&el_label, FSUBR), &globals);
    add_pair(sym("ldefine"),    func(&el_ldefine, FSUBR), &globals);
    add_pair(sym("defun"),      func(&el_defun, FSUBR), &globals);
    add_pair(sym("print"),      func(&el_print, SUBR), &globals);
    add_pair(sym("terpri"),     func(&el_terpri, FSUBR), &globals);
    add_pair(sym("<"),          func(&el_lessthan, SUBR), &globals);
    add_pair(sym(">"),          func(&el_greaterthan, SUBR), &globals);
    add_pair(sym("+"),          func(&el_plus, SUBR), &globals);
    add_pair(sym("-"),          func(&el_minus, SUBR), &globals);
    add_pair(sym("/"),          func(&el_divide, SUBR), &globals);
    add_pair(sym("*"),          func(&el_times, SUBR), &globals);
    add_pair(sym("="),          func(&el_eq, SUBR), &globals);
// new primitives
    add_pair(sym("id"),         func(&el_id, FSUBR), &globals);
    add_pair(sym("<="),         func(&el_lessthanequal, SUBR), &globals);
    add_pair(sym(">="),         func(&el_greaterthanequal, SUBR), &globals);
    add_pair(sym("defun"),      func(&el_defun, FSUBR), &globals);
    add_pair(sym("funcall"),    func(&el_funcall, FSUBR), &globals);
    add_pair(sym("null"),       func(&el_nilp, SUBR), &globals);
    add_pair(sym("consp"),      func(&el_consp, SUBR), &globals);
    add_pair(sym("times"),      func(&el_times, SUBR), &globals);
    add_pair(sym("zerop"),      func(&el_zerop, SUBR), &globals);
    add_pair(sym("greaterp"),   func(&el_greaterthan, SUBR), &globals);
    add_pair(sym("lessp"),      func(&el_lessthan, SUBR), &globals);
    add_pair(sym("sub1"),       func(&el_sub1, SUBR), &globals);
    add_pair(sym("add1"),       func(&el_add1, SUBR), &globals);
    add_pair(sym("numberp"),    func(&el_numberp, SUBR), &globals);
    add_pair(sym("eq"),         func(&el_equal, SUBR), &globals);
    add_pair(sym("and"),        func(&el_and, FSUBR), &globals);
    add_pair(sym("or"),         func(&el_or, FSUBR), &globals);
    add_pair(sym("not"),        func(&el_not, SUBR), &globals);
    add_pair(sym("setflag"),    func(&el_setflag, SUBR), &globals);
    nil = newnode(NIL);
    tee = newnode(TEE);
    add_pair(sym("t"), tee, &globals);
    add_pair(sym("nil"), nil, &globals);
}

//
// Tokenization
//
int getChar(char **s) {
    return *(*s)++;
}

int ungetChar(char **s) {
    return *(--(*s));
}

char *getToken(char **s, char *token) {
    int ch = getChar(s), index = 0;

    while (isspace(ch))
        ch = getChar(s);

    while (ch isnt '\0') {
        if (ch is '(' or ch is ')' or ch is '\'') {
            token[index++] = ch;
            break;
        } else {
            while(not isspace(ch) and ch isnt ')' and ch isnt '(' and ch isnt '\0' and index < LINELENGTH - 1) {
                token[index++] = ch;
                ch = getChar(s);
            }
            ungetChar(s);
            break;
        }
    }
    token[index] = '\0';
    return token;
}

node *tokenize(char **code) {
    char token[LINELENGTH], *s = *code;
    node *l = NULLPTR;
    while(*(getToken(code, token)) isnt '\0')
        atl(&l, sym(token));
    *code = s;
    return l;
}

//
// Parse
//
int equal(node *sym, char *s2) {
    char *s1 = sym->name->s;
    while (*s1 is *s2++)
        if (*s1++ is '\0')
            return (1);
    return 0;
}

int is_valid_int( char *str) {
    if (*str is '-')
        ++str;
    if (not *str)
        return FALSE;
    while (*str) {
        if (not isdigit((int)*str))
            return FALSE;
        ++str;
    }
    return TRUE;
}

node *makeNode(node *n) {
    if (n->type is SYM) {
        char *name = n->name->s;
        if (is_valid_int(name))
            return integer(stoi(name));
    }
    return n;
}

node *_parse(node **code, char *terminator) {
    node *l = NULLPTR, *res, *top;
    int quote = 0;

    while ((top = popNode(code)) isnt NULLPTR and not equal((res = car(top)), terminator)) {
        if ((equal(res, "(") and equal(car(*code), ")"))) {
            popNode(code);
            res = nil;
        } else if (equal(res, "nil"))
            res = nil;
        else if (equal (res, "t"))
            res = tee;
        else if (equal(res, "("))
            res = _parse(code, ")");
        else if (equal(res, "'")) {
            quote = 1;
            res = NULL;
        }
        if (res isnt NULL) {
            res = makeNode(res);
            if (quote) {
                res = cons(sym("quote"), cons(res, NULLPTR));
                quote = 0;
            }
            atl(&l, res);
        }
    }
    return l;
}

node *parse(node **code) {
    return _parse(code, "");
}

node *parse_string(char **input) {
    node *tokens = tokenize(input);
    return parse(&tokens);
}

//
// Eval
//
int length(node *l) {
    int n = 0;
    if (nilp(l))
        return 0;
    forlist (ptr in l)
        n++;
    return n;
}

node *evlambda(node *vals, node *expr, node *env) {
    node *args = largs(expr), *res = nil;
    if (length(args) == 0)
        res = eval(lbody(expr), env);
    else if (length(args) is length(vals))
        res = eval(lbody(expr), make_env(args, vals, env));
    return res ;
}

node *evform(node *fnode, node *exp, node *env) {
    return subrp(fnode)? (*funcptr(fnode))(eval(exp, env), env) : (*funcptr(fnode))(exp, env);
}

node *evsym(node *exp, node *env) {
    node *val = lookupsym(name(exp), env);
    if (val is NULLPTR)
        val = exp;
    return val;
}

node *eval_list(node *sexp, node *env) {
    node *head = eval(car(sexp), env), *res = NULLPTR;
    if (subrp(head) or fsubrp(head))
        res = evform(head, cdr(sexp), env);
    else if (lambdap(head))
        res = evlambda(cdr(sexp), head, env);
    else {
        res = cons(head, NULLPTR);
        forlist (ptr in cdr(sexp))
            append(res, eval(car(ptr), env));
    }
    return res;
}

node *eval(node *input, node *env) {
    if (nullp(input))
        setflag();
    if (consp(input))
        input = eval_list(input, env);
    else if (symp(input))
        input = evsym(input, env);
    return input;
}

//
// REPL
//
void REPL(char *input) {

    init_lisp();

    mark_expr(globals, PERMANENT);
    mark_expr(NULLPTR, PERMANENT);

    node *val;

    node *l = parse_string(&input);

    mark_expr(l, PERMANENT);

    node *top_env = newnode(ENV);
    top_env->bindings = NULLPTR;

    forlist (sexp in l) {
        pr(car(sexp));
        top_env->bindings = NULLPTR;
        val = eval(car(sexp), top_env);
        pr(val);
        mark_expr(globals, PERMANENT);
        mark_expr(history, PERMANENT);
        free_unmarked(&allocated);
    }

}

// End of LISP Code

//
// test on the host - simulate the info for a single core
//
int main(int argc, char *argv[]) {
    unsigned int row, col;

    //
    // get the core id
    //
    id = coreID(&row, &col);

    //
    // Initialize the core
    //
    coreInit(argc, argv);

    //
    // load the code
    //
    input = &memory->data[id].code[0];

    //
    // Read, Eval and Print
    //
    REPL(input);

    //
    // Print stats and exit
    //
    setflag();

    return 0;
}
