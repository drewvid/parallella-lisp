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

node* integer(long num) {
    node *ptr = newnode(INT);
    ival(ptr) = num;
    return ptr;
}

node *newcontext(node *bindings) {
    node *env = newnode(ENV);
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
    return newcontext(nenv);
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
    int i = ival(nextarg(&args)), icmp = 0;
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
    node *funcargs = eval(nextarg(&args), env);
    return eval(cons(funcname, cons(funcargs, NULLPTR)), env);
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
    top_env = NULLPTR;
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
    top_env = globals;
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

    forlist (sexp in l) {
        pr(car(sexp));
        val = eval(car(sexp), top_env);
        pr(val);
        mark_expr(globals, PERMANENT);
        mark_expr(history, PERMANENT);
        free_unmarked(&allocated);
    }

}
