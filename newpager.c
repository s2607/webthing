#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <malloc.h>
#include "newpager.h"

int llf(char *s, int w) {
	int i=0;
	for(i=0; s[i]!=0&&s[i]!='\n'&&i<w;i++);
	if(s[i]=='\n'||i>=w)
		i+=1;
	return i;
	
}
char *lfn(char *s, int w, int n) {
	int i=0;
	int l=0;

	for(i=l=0;s[i]!=0&&l<n;i+=llf(s+i,w),l+=1);
	return s+i;
}
int page(pagers *p) {
	char lbuf[44];
	snprintf(lbuf,44,"%d of %d>",p->curline,p->totallines);

	char *l=readline(lbuf);
	if(l==NULL)
		return 0;//It's fine
	if(strlen(l)==0) {
		p->curline=p->curline+p->l;
		if(p->curline>p->totallines)
			p->curline=p->totallines;
	}else if(strcmp(l,"top") ){
		p->curline=0;
	}else if(strcmp(l,"bottom") ){
		p->curline=p->totallines-p->l;
		if(p->curline<0)p->curline=0;
	}else if(strcmp(l,"help") ){
		puts("top,bottom, empty lines page down, numbers follow links, goto resets the origin url");
	}else
		puts("?");
	free(l);
	char *s=lfn(p->s,p->w,p->curline);
	int b=lfn(p->s,p->w,p->curline+p->l)-lfn(p->s,p->w,p->curline);
	printf("%s%d\n",s,b);
	fwrite(s,1,b,stdout);
	return 1;

}
void initpage(pagers *p,char *s) {
	p->w=79;
	p->l=20;
	p->curline=0;
	int i=0;
	p->s=s;
	for(i=p->totallines=0;i<strlen(p->s);i+=llf(p->s+i,p->w))
		p->totallines+=1;
	p->newlink=0;
	p->method=0;

}
