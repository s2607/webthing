
#ifndef PAGER_H
#define PAGER_H 1

typedef int (*mfp)(char *, int *, char **);
int nullmenu(char *, int *, char **);
void pagethrough(char *,mfp,char **);

#endif
