#if EPIPHANY
#define DIRECTIVE __attribute__((aligned(8)))
#else
#define DIRECTIVE
#endif

#define TRUE    1
#define FALSE   0

#define NCORES 16
#define BANKSIZE 8192
#define STRINGMAX BANKSIZE
#define NAMESTRMAX 32
#define LINELENGTH 1024

#define FREESTRING 10
#define FREEOBJECT 20000
#define FREENAME 8000
