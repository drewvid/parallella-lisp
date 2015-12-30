#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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
#define BANKSIZE 4096
#define STRINGMAX BANKSIZE
#define NAMESTRMAX 16

#define FREESTRING 10
#define FREEOBJECT 10000
#define FREENAME 10000

#define BUF_ADDRESS 0x8f000000

#define is ==
#define isnt !=
#define not !
#define and &&
#define or ||
#define in ,

#define forlist(...) forlist_xp(forlist_in, (__VA_ARGS__))
#define forlist_in(X, S) for(node *X = S; X isnt NULL; nextptr(X))
#define forlist_xp(X, A) X A

#define forlist2(...) forlist_xp(forlist_in2, (__VA_ARGS__))
#define forlist_in2(X, S, XX, SS) for(node *X = S, *XX = SS; X isnt NULL and XX isnt NULL; nextptr(X), nextptr(XX))

#define forheap(...) forlist_xp(forheap_in, (__VA_ARGS__))
#define forheap_in(X, S) for(node *X = S; X isnt NULL; X = next(X))

#define PERMANENT 		2

#define NULLPTR         (node *)0
#define EOS             '\0'
#define EOSP(X)         ((X) is EOS)
#define EOI             nullnode
#define EOIP(X)         ((X) is EOI)
#define nullp(X)        ((X) is NULL)

#define ppval(X)        (**(X))
#define ppdec(X)        ((*(X))--)
#define ppvalinc(X)     (*(*(X))++)

#define nextptr(X)      ((X) = (X)->cdr)
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
#define enullp(X)       ((X) and (X)->type is ENULL)
#define teep(X)         ((X) and (X)->type is TEE)

#define car(X)          ((X)->car)
#define cdr(X)          ((X)->cdr)
#define caar(X)         (car(car(X)))
#define cadar(X)        (car(cdr(car(X))))

enum ltype {PAIR, LIST, SYM, SUBR, FSUBR, LAMBDA, INT, NIL, ENULL, TEE, ENV};

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

int callmax = 30000;

int ncalls = 0;

node *tee;
node *nil;
node *nullnode;
node *globals = NULLPTR;
node *top_env = NULLPTR;

string *freeStringArray;
node *freeNodeArray;
namestr *freeNameArray;

char *input, *output, *result;
int freeStringIndex = 0;
int freeNodeIndex = 0;
int freeNameIndex = 0;

ememory *memory;
int id;


/* fl-device.c */
void appendString(char *item);
void appendFloat(double num, char bool);
void appendInt(int num, char bool);
void setflag(void);
string *smalloc(void);
namestr *nmalloc(void);
node *omalloc(void);
char *name(node *o);
node *newnode(enum ltype type);
node *sym(char *n);
node *cons(node *head, node *tail);
node *pair(node *head, node *tail);
node *func(node *(*fn)(node *, node *), enum ltype type);
node *lambda(node *args, node *sexp);
node *integer(int num);
node *newcontext(node *bindings);
node *nextarg(node **pargs);
node *fn_car(node *args, node *env);
node *fn_cdr(node *args, node *env);
node *fn_nilp(node *args, node *env);
node *fn_quote(node *args, node *env);
node *fn_cons(node *args, node *env);
node *fn_equal(node *args, node *env);
node *fn_atom(node *args, node *env);
node *fn_cond(node *args, node *env);
node *fn_if(node *args, node *env);
node *fn_lambda(node *args, node *env);
node *fn_label(node *args, node *env);
node *fn_ldefine(node *args, node *env);
node *binary(node *args, int fcn);
node *compare(node *args, int fcn);
node *fn_lessthan(node *args, node *env);
node *fn_greaterthan(node *args, node *env);
node *fn_eq(node *args, node *env);
node *fn_plus(node *args, node *env);
node *fn_minus(node *args, node *env);
node *fn_times(node *args, node *env);
node *fn_divide(node *args, node *env);
node *fn_append(node *args, node *env);
node *fn_concat(node *args, node *env);
node *fn_loop(node *args, node *env);
node *fn_block(node *args, node *env);
node *fn_progn(node *args, node *env);
node *lastcell(node *list);
node *append(node *list, node *obj);
node *concat(node *l1, node *l2);
node *fn_print(node *args, node *env);
node *fn_terpri(node *args, node *env);
void add_pair(node *head, node *tail, node **list);
void print_globals(void);
void init_lisp(void);
void print(node *sexp);
int is_valid_int(char *str);
void skip_whitespace(char **input);
void read_string(char **input, char *buffer);
node *make_symbol(char *buffer);
node *next_token(char **input);
node *read_tokens(char **input);
node *parse_string(char **input);
int length(node *l);
node *assq(char *key, node *list);
node *lookupsym(char *name, node *env);
node *make_env(node *vars, node *vals, node *env);
node *evlambda(node *vals, node *expr, node *env);
node *evform(node *fnode, node *exp, node *env);
node *evsym(node *exp, node *env);
node *eval_list(node *sexp, node *env);
node *eval(node *input, node *env);
void REPL(char *input);
char *readFile(char *fileName);
int coreID(unsigned int *row, unsigned int *col);
void coreInit(void);
int main(void);


#define BUF_ADDRESS 0x8f000000

void appendString(char *item) {
    int i;
    for(i = 0; i < strlen(item); i++)
        *result++ = *(item + i);
    result[i] = '\0';
}

void appendFloat(double num, char bool) {
    int i;
    char tmp[16];
    char *tptr = &tmp[0];
    sprintf(tptr, "%f", num);
    for(i = 0; i < strlen(tmp); i++)
        *result++ = *(tptr + i);
    result[i] = '\0';
    if (bool)
        appendString("\n");
}

void appendInt(int num, char bool) {
    int i;
    char tmp[16];
    char *tptr = &tmp[0];
    sprintf(tptr, "%d", num);
    for(i = 0; i < strlen(tmp); i++)
        *result++ = *(tptr + i);
    result[i] = '\0';
    if (bool)
        appendString("\n");
}

#if EPIPHANY
void setflag() {
    unsigned *d;

    appendString("\nnode size: ");
    appendInt(sizeof(node), TRUE);
    appendString("strings allocated: ");
    appendInt(freeStringIndex, TRUE);
    appendString("node allocated: ");
    appendInt(freeNodeIndex, TRUE);
    appendString("names allocated: ");
    appendInt(freeNameIndex, TRUE);
    appendString("memory size: ");
    appendInt(sizeof(ememory), TRUE);

    memory->data[id].finished = 1;

    d = (unsigned *) 0x7000;
    (*(d)) = 0x00000001;

    __asm__ __volatile__("idle");
}
#else
void setflag() {

    appendString("\nnode size: ");
    appendInt(sizeof(node), TRUE);
    appendString("strings allocated: ");
    appendInt(freeStringIndex, TRUE);
    appendString("node allocated: ");
    appendInt(freeNodeIndex, TRUE);
    appendString("names allocated: ");
    appendInt(freeNameIndex, TRUE);
    appendString("memory size: ");
    appendInt(sizeof(ememory), TRUE);

    printf("%s", output);
    exit(0);
}
#endif

// LISP Code

string *smalloc(void) {
    return &freeStringArray[freeStringIndex++];
}

namestr *nmalloc(void) {
    return &freeNameArray[freeNameIndex++];
}

node *omalloc(void) {
    return &freeNodeArray[freeNodeIndex++];
}

char *name(node *o) {
    if(not symp(o)) return "";
    return o->name->s;
}

node *newnode(enum ltype type) {
    node *n = omalloc();
    type(n) = type;
    return n;
}

node *sym (char *n) {
    node *ptr = newnode(SYM);
    namestr *name;
    name = nmalloc();
    strcpy(name->s, n);
    ptr->name = name;
    return ptr;
}

node *cons (node *head, node *tail) {
    node *ptr = omalloc ();
    type(ptr) = LIST;
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
    node *ptr = omalloc ();
    type(ptr) = type;
    funcptr(ptr) = fn;
    return ptr;
}

node *lambda (node *args, node *sexp) {
    node *ptr = omalloc ();
    type(ptr) = LAMBDA;
    largs(ptr) = args;
    lbody(ptr) = sexp;
    return ptr;
}

node* integer(int num) {
    node *ptr = newnode(INT);
    ival(ptr) = num;
    return ptr;
}

node *newcontext(node *bindings) {
    node *env = newnode(ENV);
    ebindings(env) = bindings;
    return env;
}

node *nextarg(node **pargs) {
    if (not consp(*pargs)) appendString("too few arguments\n");
    node *arg = car(*pargs);
    *pargs = cdr(*pargs);
    return arg;
}

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
    if (nilp(tail)) tail = NULLPTR;
    return cons(head, tail);
}

node *el_equal (node *args, node *env) {
    node *first = nextarg(&args);
    node *second = nextarg(&args);
    if (symp(first) and symp(second))
        return strcmp(name(first), name(second)) is 0? tee : nil;
    else
        return nil;
}

node *el_atom (node *args, node *env) {
    node *res = tee;
    while (args isnt NULL and consp(args)) {
        if (not symp(car(args))) res = nil;
        args = cdr(args);
    }
    return res;
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
    return (type(cond) is TEE) ? eval(iftrue, env) : eval(nextarg(&args), env);
}

node *el_lambda (node *args, node *env) {
    node *lambda_args = nextarg(&args);
    return lambda(lambda_args, nextarg(&args));
}

node *el_label (node *args, node *env) {
    node *name1 = nextarg(&args), *val;
    node *def = assq(name(name1), globals);
    val = eval(nextarg(&args), env);
    if(not nullp(def)) cdr(def) = val;
    else add_pair(name1, val, &globals);
    return val;
}

node *el_ldefine(node *args, node *env) {
    node *name1 = nextarg(&args), *val;
    val = eval(nextarg(&args), env);
    if (nullp(ebindings(env))) {
        env->bindings = cons(pair(name1, val), NULL);
        return val;
    }
    node *def = assq(name(name1), ebindings(env));
    if (not nullp(def)) def->cdr = val;
    else append(ebindings(env), pair(name1, val));
    return val;
}

node *binary(node *args, int fcn) {
    int i = ival(nextarg(&args));
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
    int i = ival(nextarg(&args)), icmp;
    forlist (ptr in args) {
        icmp = i - ival(car(ptr));
        switch(fcn) {
        case '<':
            icmp = (icmp < 0.0);
            break;
        case '=':
            icmp = (icmp is 0.0);
            break;
        case '>':
            icmp = (icmp > 0.0);
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

node *el_append(node *args, node *env) {
    node *l1 = nextarg(&args);
    return append(l1, nextarg(&args));
}

node *el_concat(node *args, node *env) {
    node *l1 = nextarg(&args);
    return concat(l1, nextarg(&args));
}

node *el_loop(node *args, node *env) {
    node *cond = nextarg(&args), *val = NULL;
    while(type(eval(cond, env)) is TEE) {
        forlist (ptr in args)
            val = eval(car(ptr), env);
    }
    return val;
}

node *el_block(node *args, node *env) {
    node *res = nil;
    forlist (ptr in args)
        res = eval(car(ptr), env);
    return res;
}

node *el_progn(node *args, node *env) {
    return args;
}

node *lastcell(node *list) {
    node *ptr = list;
    while (consp(ptr) and not nullp(cdr(ptr))) nextptr(ptr);
    return ptr;
}

node *append(node *list, node *obj) {
    node *ptr = lastcell(list);
    rplacd(ptr, cons(obj, NULL));
    return list;
}

node *concat(node *l1, node *l2) {
    node *ptr = lastcell(l1);
    rplacd(ptr, l2);
    return l1;
}

node *el_print(node *args, node *env) {
    forlist (ptr in args)
        print(car(ptr));
    return nil;
}

node *el_terpri(node *args, node *env) {
    int i, n = ival(nextarg(&args));
    for (i = 0; i < n; i++)
        appendString("\n");
    return nil;
}

void add_pair(node *head, node *tail, node **list) {
    *list = cons(pair(head, tail), *list);
}

void print_globals() {
    forlist (ptr in globals)
        print(car(ptr));
    appendString("\n");
}

void init_lisp() {
    add_pair(sym("quote") ,     func(&el_quote, FSUBR), &globals);
    add_pair(sym("car"),        func(&el_car, SUBR), &globals);
    add_pair(sym("cdr"),        func(&el_cdr, SUBR), &globals);
    add_pair(sym("nilp"),       func(&el_nilp, SUBR), &globals);
    add_pair(sym("cons"),       func(&el_cons, SUBR), &globals);
    add_pair(sym("append"),     func(&el_append, SUBR), &globals);
    add_pair(sym("concat"),     func(&el_concat, SUBR), &globals);
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
    add_pair(sym("print"),      func(&el_print, SUBR), &globals);
    add_pair(sym("terpri"),     func(&el_terpri, FSUBR), &globals);
    add_pair(sym("<"),          func(&el_lessthan, SUBR), &globals);
    add_pair(sym(">"),          func(&el_greaterthan, SUBR), &globals);
    add_pair(sym("+"),          func(&el_plus, SUBR), &globals);
    add_pair(sym("-"),          func(&el_minus, SUBR), &globals);
    add_pair(sym("/"),          func(&el_divide, SUBR), &globals);
    add_pair(sym("*"),          func(&el_times, SUBR), &globals);
    add_pair(sym("="),          func(&el_eq, SUBR), &globals);
    nil = newnode(NIL);
    tee = newnode(TEE);
    nullnode = newnode(ENULL);
    add_pair(sym("t"), tee, &globals);
    add_pair(sym("nil"), nil, &globals);
    top_env = globals;
}

void print(node *sexp) {
    if(nullp(sexp)) return;
    if(consp(sexp)) {
        appendString ("(");
        print(car(sexp));
        sexp = cdr(sexp);
        while (consp(sexp)) {
            appendString (" ");
            print(car(sexp));
            sexp = cdr(sexp);
        }
        appendString ( ")");
    } else if (pairp(sexp)) {
        appendString("(");
        print(car(sexp));
        appendString(" . ");
        print(cdr(sexp));
        appendString(")");
    } else if(symp(sexp)) {
        appendString (name(sexp));
    } else if(lambdap(sexp)) {
        appendString ("#");
        print(largs(sexp));
        print(lbody(sexp));
    } else if (intp(sexp))
        appendInt(sexp->i, FALSE);
    else if (subrp(sexp))
        appendString(" subr");
    else if (fsubrp(sexp))
        appendString(" fsubr");
    else if (nilp(sexp))
        appendString(" nil ");
    else if (enullp(sexp))
        appendString(" null ");
    else if (teep(sexp))
        appendString(" t ");
    else
        appendString ("Error.");
}

int is_valid_int(char *str) {
    if (*str is '-') ++str;
    if (not *str) return FALSE;
    while (*str) {
        if (not isdigit((int)*str)) return FALSE;
        ++str;
    }
    return TRUE;
}

void skip_whitespace( char **input) {
    int ch = ppvalinc(input);
    while (isspace(ch)) ch = ppvalinc(input);
    ppdec(input);
}

void read_string(char **input, char *buffer) {
    int index = 0, ch = ppvalinc(input);
    while(not isspace(ch) and ch isnt ')' and ch isnt '\0' and index < 256 - 1) {
        buffer[index++] = ch;
        ch = ppvalinc(input);
    }
    buffer[index++] = '\0';
    if (ch is ')' or ch is '\0')
        ppdec(input);
}

node *make_symbol(char *buffer) {
    int i;
    if (is_valid_int(buffer) and sscanf(buffer, "%d", &i) isnt 0)
        return integer(i);
    else
        return sym(buffer);
}

node *next_token(char **input) {
    int ch;
    skip_whitespace(input);
    ch = ppvalinc(input);
    if(ch is '\0') setflag();
    if(ch is ')')
        return sym(")");
    else if(ch is '(')
        return sym("(");
    else if (ch is '\'')
        return sym("'");
    else {
        char buffer[256];
        ppdec(input);
        read_string(input, &buffer[0]);
        return make_symbol(buffer);
    }
}

node *read_tokens(char **input) {
    node *token = next_token(input), *head;
    if(strcmp(name(token),")") is 0)
        return NULL;
    else if(strcmp(name(token),"(") is 0)
        head = read_tokens(input);
    else if (symp(token) and strcmp(name(token), "'") is 0)
        head = cons(sym("quote"), cons(parse_string(input), NULL));
    else
        head = token;
    return cons(head, read_tokens(input));
}

node *parse_string(char **input) {
    node *token = next_token(input);
    if(strcmp(name(token),"(") is 0)
        token = read_tokens(input);
    else if (symp(token) and strcmp(name(token), "'") is 0)
        token = cons(sym("quote"), cons(parse_string(input), NULL));
    return token;
}

int length(node *l) {
    int n = 0;
    forlist (ptr in l)
        n++;
    return n;
}

node *assq(char *key, node *list) {
    forlist (ptr in list)
        if (strcmp(key, name(caar(ptr))) is 0)
            return car(ptr);
    return NULL;
}

node *lookupsym(char *name, node *env) {
    node *fptr = NULLPTR;
    if (ebindings(env) isnt NULLPTR) fptr = assq(name, ebindings(env));
    if(nullp(fptr))                   fptr = assq(name, globals);
    return not nullp(fptr)? cdr(fptr) : NULL;
}

node *make_env(node *vars, node *vals, node *env) {
    node *nenv = NULL;
    forlist2 (pvar in vars, pval in vals)
        add_pair(car(pvar), eval(car(pval), env), &nenv);
    return newcontext(nenv);
}

node *evlambda(node *vals, node *expr, node *env) {
    node *args = largs(expr), *res = nil;
    if (length(args) is length(vals)) {
        ncalls += 1;
        res = eval(lbody(expr), make_env(args, vals, env));
    }
    return res ;
}

node *evform(node *fnode, node *exp, node *env) {
    return subrp(fnode)? (*funcptr(fnode))(eval(exp, env), env) : (*funcptr(fnode))(exp, env);
}

node *evsym(node *exp, node *env) {
    node *val = lookupsym(name(exp), env);
    if (val is NULL) val = exp;
    return val;
}

node *eval_list(node *sexp, node *env) {
    node *head = eval(car(sexp), env), *res = NULL;
    if (subrp(head) or fsubrp(head)) res = evform(head, cdr(sexp), env);
    else if (lambdap(head))          res = evlambda(cdr(sexp), head, env);
    else {
        res = cons(head, NULL);
        forlist (ptr in cdr(sexp))
            append(res, eval(car(ptr), env));
    }
    return res;
}

node *eval(node *input, node *env) {
    if (nullp(input) or ncalls > callmax) setflag();
    if (consp(input))     input = eval_list(input, env);
    else if (symp(input)) input = evsym(input, env);
    return input;
}

void REPL(char *input) {
    node *sexp, *val;
    init_lisp();
    print_globals();
    do {
        sexp = parse_string(&input);
        appendString ("> ");
        print(sexp);
        appendString("\n");
        val = eval(sexp, top_env);
        print(val);
        appendString("\n");
    } while (1);
}


// End of LISP Code

#if EPIPHANY

int coreID(unsigned int *row, unsigned int *col) {

    e_coreid_t coreid;

    coreid = e_get_coreid();
    coreid = coreid - e_group_config.group_id;
    *row = (coreid >> 6) & 0x3f;
    *col = coreid & 0x3f;

    return((*row * 4) + *col);
}

void coreInit() {

    memory = (ememory *)(BUF_ADDRESS);

    freeStringArray = &memory->data[id].freeStringArray[0];
    freeNodeArray = &memory->data[id].freeNodeArray[0];
    freeNameArray = &memory->data[id].freeNameArray[0];

}

#else

char *readFile(char *fileName) {
    FILE *file = fopen(fileName, "r");
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

int coreID(unsigned int *row, unsigned int *col) {

    *row = 1;
    *col = 1;

    return((*row * 4) + *col);
}

void coreInit(void) {
    char *code;

    memory = (ememory *)malloc(sizeof(ememory));

    freeStringArray = &memory->data[id].freeStringArray[0];
    freeNodeArray = &memory->data[id].freeNodeArray[0];
    freeNameArray = &memory->data[id].freeNameArray[0];

    code = readFile("code/p2.lisp");
    sprintf(memory->data[id].code, "%s", code);
}

#endif

int main(void) {
    int row, col;
    char tmpbuf[16];

    id = coreID(&row, &col);

    coreInit();


    input = &memory->data[id].code[0];
    output = &memory->data[id].output[0];

    result = output;
    memset(output, 0, BANKSIZE);

    appendInt(id, TRUE);
    sprintf(tmpbuf, "(%d, %d)\n", row + 1, col + 1);
    appendString(tmpbuf);

    REPL(input);

    setflag();

    return 0;
}
