#ifndef NEWPAGER_H
#define NEWPAGER_H 1
#include <curl/curl.h>
#include "html.h"

typedef struct { 
	int curline;
	int totallines;
	char *s;
	int newlink;
	int method;
	int w;
	int l;
	tag *root;
	CURLU *origin;

} pagers;
int page(pagers *);
void initpage(pagers *,char *);
#endif
