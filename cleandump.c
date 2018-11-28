#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "html.h"
#include <unistd.h>

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
int ntos(tag *t,char **a,int *n)
{//n must not be zero (it is the space argument to asn)
	static int islink=0;//must do this
	static int istitle=0;
	static int inbody=0;
	static int inhead=0;
	static int suppress=0;
	static int em=0;
	static int headding=0;
	static int acount=1;
	static int islist=0;
	static int lcount=0;
	char lnbuf[20];
	
	if(!t)
		return 1;
//TODO: replace all these with "emit" functions that take a rendering state structure and a tag
	if(t->type){
		if(!strcasecmp(t->type,"a")){
			if(t->closing){
				free(popt());
				snprintf(lnbuf,10,"[%d]",acount);
				acount=acount+1;
				*a=as(a,lnbuf,n);

			} else{
				char **p=getprop(t,"href");
				islink=1;
				pusht("a");
				if(p!=NULL){
					t->lnum=acount;
					//hrefs[acount]=scrubquotes(*p);
				}
			}

		}
		if(!strcasecmp(t->type,"img")){
			char **p=getprop(t,"src");
			if(p!=NULL){
				t->lnum=acount;
				snprintf(lnbuf,10,"[i%d]",acount);
				*a=as(a,lnbuf,n);
				acount=acount+1;
			}

		}
		if(!strcasecmp(t->type,"ul")){
			islist=1;
			//ptext(docm,&n,"","\n",MARG);
			//emitstring("[list]","p");
			*a=as(a,"\n",n);
		
		}
		if(!strcasecmp(t->type,"li")){
			if(t->closing){
				free(popt());
			}else
				pusht("li");
		}
		if(!strcasecmp(t->type,"script")){//ehhhhhh
				suppress=!suppress;//TODO: nested suppressed elements (ehh) (no, that wont work)
		}
		if(!strcasecmp(t->type,"hr"))
			*a=as(a,"     ----------------------------------\n",n);
		
		if(!strcasecmp(t->type,"br"))
			*a=as(a,"\n",n);
	}
	//actuall text content rendering goes here
	if(!suppress){
		int j;
		/*for(j=0;j<7;j++)
			m[j]=0;
		if(inhead)	
			m[0]='=';
		else
			m[0]=' ';
		if(islink)
			m[1]='0'+acount;//TODO:insanely crappy
		else
			m[1]=' ';
		if(islist>2)
			m[2]='*';
		else
			m[2]=' ';*/
		if(t->freetext){
			char *s=scrubquotes(t->freetext);
			if(strlen(s) <2)
				return 1;
			if(ispushed("a"))
				as(a,s,n);
			else
				as(a,s,n);
			free(s);
			
		}
	}
	return 1;
}
int snr(tag *t,char **a,int *n) {
	printf("looking for %d.\n",*n);
	if(t == NULL){
		puts("NULL");
		return 1;
	}
	if(t->lnum == *n){
		puts("found.");
		return 0;
	}
	return 1;
}

char *nthref(tag *root, int n) {
	printf("searching in %x",root);
	if(root == NULL) {
		printf("No Document!\n");
		return NULL;
	}
	tag *a = sdom(root,snr,NULL,&n);//don't need to free?
	if(a == NULL)
		return NULL;
	char ** r=getprop(a,"href");
	if(r == NULL || *r == NULL)
		return NULL;
	char * cr = calloc(strlen(*r),1);
	memcpy(cr,*r, strlen(*r));
	return cr;

}

char *refs(tag *root,char **d, int *m)
{
	int i;
	char *t;
	char nbuf[20];
	*d=as(d,"\n\n======REFRENCES======\n",m);
	for(i=1;(t=nthref(root,i))&&t!=NULL;i++){
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
	r=as(&r,"\%hdoc\n",&rm);
	sdom(root,ntos,&r,&rm);
	r=refs(root,&r,&rm);
	return r;
}
