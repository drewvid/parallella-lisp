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

#define type(X)         ((X)->type)

#define nullp(X)        ((X) == NULLPTR)
#define consp(X)        ((X) && (X)->type == LIST)
#define pairp(X)        ((X) && (X)->type == PAIR)
#define symp(X)         ((X) && (X)->type == SYM)
#define subrp(X)        ((X) && (X)->type == SUBR)
#define fsubrp(X)       ((X) && (X)->type == FSUBR)
#define lambdap(X)      ((X) && (X)->type == LAMBDA)
#define intp(X)      	((X) && (X)->type == INT)
#define nilp(X)         ((X) && (X)->type == NIL)
#define teep(X)         ((X) && (X)->type == TEE)

#define caar(X)         (car(car(X)))
#define cadar(X)        (car(cdr(car(X))))
