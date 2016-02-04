int callmax = 30000;

int ncalls = 0;

node *tee;
node *nil;
node *NULLPTR;
node *globals;
node *top_env;
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
