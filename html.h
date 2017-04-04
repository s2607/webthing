#ifndef HTML_H
#define HTML_H 
typedef struct {
	int closing;
	char **pn; //a list of immutable strings
	int pnm;
	char **pv; //same but changes sometimes
	int pvm;
	char *type;
	void **child; //a list of these
	int childm;
	void *parent;
	char *freetext;
} tag;
//trade space for time
#define PRLC 100

//lists always grow until they are freed
void **append(char **l, int *lm, void *a);
tag *newchild(tag *t);
void newchildtext(tag *t,char *it);
char *rtag(tag *t, char *s, char *supername,int state);
tag *sdom(tag *, int(*)(tag *,char *,int *),char *,int *);
int bytype(tag *,char *, int);
char **getprop(tag *,char *);
//TODO: taglist insert, delete
//TODO: get element by id
//TODO: property dictionary delete

char *as(char *,char *,int *);
char *tomarkdown(tag *);
#endif
