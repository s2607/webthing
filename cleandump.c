#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "html.h"
#include <unistd.h>
#include "form.h"

#define MARG 6
//type,descriptor

char *as(char **d, char *s, int *m)
{
	int l=strlen(s);
	*d=asn(s,l,*d,m);
	return *d;
}
char *asn(char *source,int n,char *dest,int *m)
{
	if(!source)
		return dest;
	if(dest==NULL){//TODO:some kind of heap corruption is causing this to crash
		//s=calloc(PRLC,1);
		//*m=PRLC;
		dest=calloc(n+PRLC,1);
		*m=n+PRLC;
	}
	if(*m-strlen(dest)<n+1){
		dest=realloc(dest,*m+n+1);
		*m=*m+n+1;
		int i=0;
		for(i=0;i<(*m)-strlen(dest);i++)
			dest[i+strlen(dest)]=0;
	}
	dest[strlen(dest)+n]=0;
	memcpy(dest+strlen(dest),source,n);
	return dest;
}
typedef struct {
	char *c;
	void *prev;
} stel;
stel bot;
stel *top=&bot;
void pusht(char *t) {
	top->prev=calloc(1,sizeof(stel));
	top->c=calloc(strlen(t),1);
	memcpy(top->c,t,strlen(t));
}
char *popt() {
	if(top==&bot){
		return calloc(1,1);//XXX
	}
	stel *c=&bot;
	char *r=c->c;
	top=bot.prev;
	free(c);
	return r;
}
int ispushed(char *t) {
	stel *c=top;
	int count=0;
	for(c=top;c!=&bot;c=c->prev){
		if(!strcmp(c->c,t))
			count=count+1;
	}
	return count;
}
void initrenderstate(renderstate *r,char **s, int *m) {
	r->islink=0;
	r->acount=0;
	r->islist=0;
	r->suppress=0;
	r->a=s;
	r->n=m;
}
int ntos(tag *t,char **rs, int *UNUSED)
{//n must not be zero (it is the space argument to asn)
	/*static int islink=0;//must do this
	static int istitle=0;
	static int inbody=0;
	static int inhead=0;
	static int suppress=0;
	static int em=0;
	static int headding=0;
	static int acount=1;
	static int islist=0;
	static int lcount=0;*/
	char lnbuf[20];
	char *tbufm=alloca(1);
	*tbufm=0;
	int tbufn=1;
	renderstate *r=(*(renderstate **)rs);
	
	if(!t)
		return 1;
//TODO: replace all these with "emit" functions that take a rendering state structure and a tag
	if(t->type){
		if(!strcasecmp(t->type,"form")){
			if(t->closing){
				*(r->a)=as(r->a,"\n+------------+\n",r->n);
				free(popt());
			} else {
				pusht("form");
				t->lnum=r->acount;
				r->acount=r->acount+1;
				snprintf(lnbuf,18,"\n+---form[%d]-+\n",r->acount);
				printf("link %d (form) \n",r->acount);
				*(r->a)=as(r->a,lnbuf,r->n);
			}
		}
		if(!strcasecmp(t->type,"input")){
			int submit=0;
			char **pt=getprop(t,"type");
			if(pt!=NULL &&!strcasecmp(*pt,"submit"))
				submit=1;
			if(t->closing){
				free(popt());
				snprintf(lnbuf,10,"[%d]",r->acount);
				printf("link %d (input) \n",r->acount);
				r->acount=r->acount+1;
				*(r->a)=as(r->a,lnbuf,r->n);
				char **p=getprop(t,"type");
				if(p!=NULL){
					char *v=scrubquotes(*p);
					*(r->a)=as(r->a,"-",r->n);
					*(r->a)=as(r->a,v,r->n);
					*(r->a)=as(r->a,"-",r->n);
					free(v);
				}else if(submit){
					*(r->a)=as((r->a),"-submit-",r->n);
				}
			} else {
				pusht("input");
				t->lnum=r->acount;
				printf("link %d (input) \n",r->acount);
			}
		}
		if(!strcasecmp(t->type,"a")){
			if(t->closing){
				free(popt());
				snprintf(lnbuf,10,"[%d]",r->acount);
				printf("link %d (anchor) \n",r->acount);
				r->acount=r->acount+1;
				*(r->a)=as(r->a,lnbuf,r->n);

			} else{
				char **p=getprop(t,"href");
				r->islink=1;
				pusht("a");//TODO:free on initpage to prevent LEAK
				if(p!=NULL){
					t->lnum=r->acount;
					printf("link %d (anchor) %s\n",r->acount,*p);
					//hrefs[acount]=scrubquotes(*p);
				}
			}

		}
		if(!strcasecmp(t->type,"img")){
			char **p=getprop(t,"src");
			if(p!=NULL){
				t->lnum=r->acount;
				snprintf(lnbuf,10,"[i%d]",r->acount);
				printf("link %d (image) %s\n",r->acount,*p);
				*(r->a)=as(r->a,lnbuf,r->n);
				r->acount=r->acount+1;
			}
			p=getprop(t,"alt");
			if(p!=NULL){//OPT cachestrlen
				if(tbufn<strlen(*p)){
					alloca(strlen(*p)-tbufn);
					tbufn+=strlen(*p);
				}
				snprintf(tbufm,tbufn-1,"%s",*p);
				*(r->a)=as(r->a,tbufm,r->n);
			}

		}
		if(!strcasecmp(t->type,"ul")){
			r->islist=1;
			//ptext(docm,&n,"","\n",MARG);
			//emitstring("[list]","p");
			*(r->a)=as(r->a,"\n",r->n);
		
		}
		if(!strcasecmp(t->type,"li")){
			if(t->closing){
				free(popt());
			}else
				pusht("li");
		}
		if(!strcasecmp(t->type,"script")){//ehhhhhh
				r->suppress=!r->suppress;//TODO: nested suppressed elements (ehh) (no, that wont work)
		}
		if(!strcasecmp(t->type,"style")){
				r->suppress=!r->suppress;
		}
		/*if(!strcasecmp(t->type,"meta")){
				suppress=!suppress;
		}*/
		if(!strcasecmp(t->type,"link")){
				r->suppress=!r->suppress;//eehhhh
		}
		if(!strcasecmp(t->type,"hr"))
			*(r->a)=as(r->a,"     ----------------------------------\n",r->n);
		
		if(!strcasecmp(t->type,"br"))
			*(r->a)=as(r->a,"\n",r->n);
	}
	//actuall text content rendering goes here
	if(!r->suppress){
		if(t->freetext){
			char *s=scrubquotes(t->freetext);
			if(strlen(s) <2)
				return 1;
			if(ispushed("a"))
				as(r->a,s,r->n);
			else
				as(r->a,s,r->n);
			free(s);
			
		}
	}
	return 1;
}
int snr(tag *t,char **a,int *n) {
	if(t == NULL){
	//	puts("NULL");
		return 1;
	}
	if(t->lnum == *n){
	//	puts("found.");
		return 0;
	}
	return 1;
}
char *nthref(tag *root, int n) {//THE DOM MUST HAVE SOMETHING TO RETURN OR THIS WILL CAUSE 
//A NULL POINTER TO BE FREED (or searches for links to prematurely terminate)
	if(root == NULL) {
		printf("No Document!\n");
		return NULL;
	}
	tag *a = sdom(root,snr,NULL,&n);//don't need to free?
	if(a == NULL)
		return NULL;
	char ** r=getprop(a,"href");
	if(r == NULL || *r == NULL)
		r=getprop(a,"src");
	if(r == NULL || *r == NULL)
		r=getprop(a,"action");
	if(r == NULL || *r == NULL)
		r=getprop(a,"id");
	if(r == NULL || *r == NULL)
		r=getprop(a,"value");
	if(r == NULL || *r == NULL)
		return NULL;
	return scrubquotes(*r);

}

char *refs(tag *root,char **d, int *m)
{
	int i;
	char *t;
	char nbuf[20];
	*d=as(d,"\n\n======REFRENCES======\n",m);
	for(i=1;
	(t=nthref(root,i))!=NULL
	;i++){
		snprintf(nbuf,20,"\n[%d]",i);
		*d=as(d,nbuf,m);
		*d=as(d,t,m);
		free(t);
	}
	return *d;
}
char *tops(tag *root)
{
	char *r=NULL;
	int rm=0;
	renderstate *rs=calloc(1,sizeof(renderstate));//heh
	initrenderstate(rs,&r,&rm);
	r=as(&r,"\%hdoc\n",&rm);
	sdom(root,ntos,(char **)&rs,&rm);
	r=refs(root,&r,&rm);
	free(rs);
	return r;
}
