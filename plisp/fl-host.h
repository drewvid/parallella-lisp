#include "params.h"

#define type(X)         ((X)->type)

#define nullp(X)        ((X) == NULLPTR)
#define consp(X)        ((X) && (X)->type == LIST)
#define pairp(X)        ((X) && (X)->type == PAIR)
#define symp(X)         ((X) && (X)->type == SYM)
#define subrp(X)        ((X) && (X)->type == SUBR)
#define fsubrp(X)       ((X) && (X)->type == FSUBR)
#define lambdap(X)      ((X) && (X)->type == LAMBDA)
#define intp(X)         ((X) && (X)->type == INT)
#define nilp(X)         ((X) && (X)->type == NIL)
#define teep(X)         ((X) && (X)->type == TEE)

#define caar(X)         (car(car(X)))
#define cadar(X)        (car(cdr(car(X))))
