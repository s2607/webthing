#ifndef NEWPAGER_H
#define NEWPAGER_H 1
#include <curl/curl.h>
#include "html.h"
#include "lists.h"

typedef struct { 
	int curline;
	int totallines;
	char *s;//text
	char *t;//source (lol that's pretty bad)
	int newlink;
	int method;
	int w;
	int l;
	tag *root;
	CURLU *origin;
	cmdl cmds;
	int cmds_m;

} pagers;
int page(pagers *);
void initpage(pagers *,char *, tag *);
void destroypage(pagers *);
typedef struct {
	char *n;
	void (*a) (char *,pagers *);
}pcmd;
int epcmd(char *,pagers *);
#endif
