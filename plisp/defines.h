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
#define NAMESTRMAX 22
#define LINELENGTH 1024

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
#define teep(X)         ((X) and (X)->type is TEE)

#define car(X)          ((X)->car)
#define cdr(X)          ((X)->cdr)
#define caar(X)         (car(car(X)))
#define cadar(X)        (car(cdr(car(X))))
