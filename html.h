#ifndef HTML_H
#define HTML_H 1
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
	//internal attributes 
	char *freetext;
	int lnum;//link num
	int loc;//byte num
	int lineloc;//line num in tty
	int eloc;
} tag;
//trade space for time
#define PRLC 100


tag *newchild(tag *t);
//text fragment nodes (type "itf") are the only ones that use the freetext field of the tag type
void newchildtext(tag *t,char *it);
char *rtag(tag *t, char *s, char *supername,int state, char *olds);
//this does a (preorder?) dom traversal with the funcction you specify. 
//It's first argument will be the current tag, the last two arguments are used by the consumer to configure your search function
//The first of the user configurable arguments is just a string.
//The secondone as an accumulator like pointer that can be updated by modifying it or returning a new pointer, the
//Your function can terminate the search by returning a zero at which point the current tag is returned from sdom (if the dom ends NULL is returned.)
//The n argument is intened to be used to indicate "nth matching" or as a memory counter.
tag *sdom(tag *, int(*)(tag *,char **,int *),char **,int *);
int bytype(tag *,char *, int);
char **getprop(tag *,char *);
//TODO: taglist insert, delete
//TODO: get element by id
//TODO: property dictionary delete
char *formfields(tag *,char **, int *);
int mnum(tag *);

typedef struct {
	int islink;
	int acount;
	int islist;
	int suppress;
	char **a;
	int *n;
} renderstate;

void initrenderer(renderstate *);

char *scrubquotes(char *);
char *as(char **,char *,int *);
char *asn(char *,int n, char *,int *);
char *tomarkdown(tag *);

//TODO: move to another header
char *tops(tag *);

tag *nthel(tag *, int);
char *nthref(tag *, int );
int lcount(tag *);//potentially super slow!
//TODO:in the future: maybe have the dom be a tuple of the root tag and a structure containing locks and caches.

#endif
