int callmax = 30000;

int ncalls = 0;

node *tee;
node *nil;
node *NULLPTR;
node *globals;
node *top_env;

string *freeStringArray;
node *freeNodeArray;
namestr *freeNameArray;

char *input, *output, *result;
int freeStringIndex = 0;
int freeNodeIndex = 0;
int freeNameIndex = 0;

ememory *memory;
int id;
