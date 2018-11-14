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
	}else {
		pcmd **c=NULL;
		for(c=(p->cmds);*c!=NULL;c++){
			if(!strcmp(l,(*c)->n))
				(*c)->a(l,p);
		}
		if(c==NULL)
			puts("?");
			
	}
	
	free(l);
	char *s=lfn(p->s,p->w,p->curline);
	int b=lfn(p->s,p->w,p->curline+p->l)-lfn(p->s,p->w,p->curline);
	printf("%s%d\n",s,b);
	fwrite(s,1,b,stdout);
	return 1;

}
void p_top(char *l, pagers *p){
	p->curline=0;
}
pcmd p_top_s ={n:"top",a:p_top};
void p_bottom(char *l, pagers *p){
	p->curline=p->totallines-p->l;
	if(p->curline<0)p->curline=0;
}
pcmd p_bottom_s={n:"bottom",a:p_bottom};
void p_help(char *l, pagers *p){
		puts("top\nbottom\n empty lines page down\n jump <num> follow links\n goto resets the origin url\n");
}
pcmd p_help_s={n:"help",a:p_help};

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
	p->cmds=clappend(p->cmds,&p->cmds_m,&p_top_s);
	p->cmds=clappend(p->cmds,&p->cmds_m,&p_bottom_s);
	p->cmds=clappend(p->cmds,&p->cmds_m,&p_help_s);
}
