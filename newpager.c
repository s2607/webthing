#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "newpager.h"
#include "net.h"
#include "html.h"


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
void showpage(pagers *p) {
	char *s=lfn(p->s,p->w,p->curline);
	int b=lfn(p->s,p->w,p->curline+p->l)-lfn(p->s,p->w,p->curline);
	fwrite(s,1,b,stdout);
}
int epcmd(char *l,pagers *p) {
	pcmd **c=NULL;
	for(c=((pcmd **)(p->cmds));*c!=NULL;c++){
		if(!strncmp(l,(*c)->n,strlen((*c)->n))) {
			(*c)->a(l,p);
			return 1;
		}
	}
	return 0;
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
		showpage(p);
	}else {
		if(!epcmd(l,p))
			puts("?");
	}
	free(l);
	return 1;

}
void opage(char *url, pagers *p) {
	char *t=NULL;
	if(url == NULL){
		printf("No url?\n");
		return;
	}
	int res= gettexturl(&t,&p->origin,url);
	if(t!=NULL){
		tag *root=NULL;
		if(p->root==NULL)
			initpage(p,"",NULL);
		root=p->root;
		rtag(root,t,"!",3);//man I wish I remembed how this worked.
		char *text=tops(root);
		destroypage(p);
		initpage(p,text,root);
	}else {
		puts("bad cheese");
		printf("Got %d From net.",res);
	}
}
void p_top(char *l, pagers *p){
	p->curline=0;
	showpage(p);
}
pcmd p_top_s ={n:"top",a:p_top};
void p_bottom(char *l, pagers *p){
	p->curline=p->totallines-p->l;
	if(p->curline<0)p->curline=0;
	showpage(p);
}
pcmd p_bottom_s={n:"bottom",a:p_bottom};
void p_help(char *l, pagers *p){
		puts(" top\n bottom\n empty lines page down.\n jump <num> follow links.\n goto resets the origin url.\n EOF:Yeet.\n");
}
pcmd p_help_s={n:"help",a:p_help};
void p_goto(char *l, pagers *p){
	char *ns=l+strlen("goto ");//TODO: trim whitespace
	opage(ns,p);
}
pcmd p_goto_s ={n:"goto",a:p_goto};
void p_jump(char *l, pagers *p){
	char *ns=l+strlen("goto ");//TODO: trim whitespace
	int n = atoi(ns);
	char *url=NULL;
	url=nthref(p->root,n);
	if(url == NULL) {
		printf("No link number %d.\n",n);
		free(url);
		return;
	}
	opage(url,p);
	free(url);
}
pcmd p_jump_s ={n:"jump",a:p_jump};
void p_dump(char *l, pagers *p) {
	puts(p->s);
	p->curline=p->totallines;
}
pcmd p_dump_s ={n:"dump",a:p_dump};

void initpage(pagers *p,char *s, tag *root) {//NOTE: init and destroy leave the URL untouched.
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
	p->cmds=clappend(p->cmds,&p->cmds_m,&p_goto_s);
	p->cmds=clappend(p->cmds,&p->cmds_m,&p_jump_s);
	p->cmds=clappend(p->cmds,&p->cmds_m,&p_dump_s);
	if(root!=NULL)
		p->root=root;
	else
		p->root=newchild(NULL);
}
void destroypage(pagers *p) {//TODO: free document.
	if(p->cmds!=NULL)
		free(p->cmds);
	p->cmds=NULL;
	if(p->s!=NULL)
		free(p->s);//It's just easier
	p->s=NULL;
}
