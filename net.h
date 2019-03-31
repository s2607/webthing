#ifndef NET_H
#define NET_H 1

int initcurl(void);
int gettexturl(char **, CURLU **, char *, int , char *);

void endcurl(void);

#define METHOD_POST 1
#define METHOD_GET  2

#endif
