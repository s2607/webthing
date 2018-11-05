#ifndef HTML_H
#define HTML_H 
#include "lists.h"
typedef struct {
	int closing;
	stringl pn; //a list of immutable strings
	int pnm;
	stringl pv; //same but changes sometimes
	int pvm;
	char *type;
	tagl child; //a list of these, it forms the tree
	int childm;
	void *parent;
	char *freetext;
} tag;
//trade space for time
#define PRLC 100


tag *newchild(tag *t);
//text fragment nodes (type "itf") are the only ones that use the freetext field of the tag type
void newchildtext(tag *t,char *it);
char *rtag(tag *t, char *s, char *supername,int state);
//this does a (preorder?) dom traversal with the funcction you specify. 
//It's first argument will be the current tag, the last two arguments are used by the consumer to configure your search function
//The first of the user configurable arguments is just a string.
//The secondone as an accumulator like pointer that can be updated by modifying it or returning a new pointer, the
//Your function can terminate the search by (TODO: figure out what terminates the search)
tag *sdom(tag *, int(*)(tag *,char *,int *),char *,int *);
int bytype(tag *,char *, int);
char **getprop(tag *,char *);
//TODO: taglist insert, delete
//TODO: get element by id
//TODO: property dictionary delete

char *scrubquotes(char *);
char *as(char *,char *,int *);
char *asn(char *,int n, char *,int *);
char *tomarkdown(tag *);
char *getref(int);

#endif
