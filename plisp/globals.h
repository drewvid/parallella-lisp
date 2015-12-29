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
