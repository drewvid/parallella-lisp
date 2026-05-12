/* host_main.c */
int main(int argc, char *argv[]);
/* libhost.c */
char *host_ptr(char *ptr);
node *dr_node(node *cell);
node *car(node *cell);
node *cdr(node *cell);
node *largs(node *cell);
node *lbody(node *cell);
char *dr_name(node *cell);
long long ival(node *cell);
void prpair(node *l);
void print(node *l);
char *readFile(char *fileName);
void *device_ptr(char *base, char *ptr);
void createFreelist(ememory *memory, int rows, int cols);
void createStringFreelist(ememory *memory, int rows, int cols);
void createNameFreelist(ememory *memory, int rows, int cols);
ememory *init_ememory(int argc, char *argv[], int rows, int cols);
void write_ememory(e_mem_t *emem, ememory *memory);
void clear_done_flags(e_epiphany_t *dev, int rows, int cols);
void poll_device(e_epiphany_t *dev, int rows, int cols);
void prGlobals(ememory *memory, int id);
void process_ememory(e_mem_t *emem, ememory *memory, int rows, int cols);
