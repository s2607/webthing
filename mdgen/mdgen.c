
#include <stdio.h>

/* Markdown will be represented by a linked list of formatting commands rather than a tree. */
/* Linefeeds in strings should be stripped before outputting, they shall be added explicetly by paragraph elements */

enum { MTXT=1, MPAR=2, MHEAD=3, MIMG-4, MLOP=5, MLOC=6, MQUO=7, MTOC=8, MESC=9, MUL=10, MLI=11 };

typedef struct {
	int command;
	char *text;
	char *alt;
	void *next;
	int depth;
	int complete;
} mdel;

mdel *head;
mdel *cur;
mdtail(mdel *h) {if(h==NULL) return NULL; if(h->next==NULL) return h; return mdtail((mdel*)h->next);}

int mdad(int c, char *text, char *alt, int depth)
{
	mdel *this=calloc(sizeof(mdel),1);
	if(this==NULL)
		return 0;
	this.command=command;
	this.text=text;
	this.alt=alt;
	this.next=next;
	this.depth=depth;
	this.complete=0;
	if(head==NULL)
		head=this;
	else
		mdtail(head)->next=(void *)this;
	cur=this;
	return 1;
}
	
int mdhandletag(tag *t, char *a, int *nr)
{
	if(!t)
		return 1;
	if(t->type!=NULL){
		if(!strcasecmp(t->type,"a")){
			if(t->closing){
				mtail(head)->complete=1;
			}else{
			}
		}
		if(strlen(t->type)==2&&(t->type[0]='h'||t->type[0]=='H')&&t->type[1]>'0'&&t->type[1]<'6'){
			if(t->closing){
				mtail(head)->complete=1;	
		}
	}
}
