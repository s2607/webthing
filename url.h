
typedef struct {
	char *u;//other methods need this
	char *method;
	char *base;
	int port;
	char *resource;
} parsedurl;


int atourl(char *u, parsedurl *pu);
char *cleanurl(char *o,char *n);
