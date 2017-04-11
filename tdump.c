#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "html.h"
#include <mcheck.h>
#include <unistd.h>

#define MARG 6
//type,descriptor

char *as(char *s,char *a,int *m)
{
	if(!a)
		return s;
	if(s==NULL){//TODO:some kind of heap corruption is causing this to crash
		//s=calloc(PRLC,1);
		//*m=PRLC;
		s=calloc(strlen(a)+PRLC,1);
		*m=strlen(a)+PRLC;
	}
	if(*m-strlen(s)<strlen(a)+1){
		s=realloc(s,*m+strlen(a)+1);
		*m=*m+strlen(a)+1;
		int i=0;
		for(i=0;i<(*m)-strlen(s);i++)
			s[i+strlen(s)]=0;
	}
	s[strlen(s)+strlen(a)]=0;
	memcpy(s+strlen(s),a,strlen(a));
//	usleep(1*1000*100);
	return s;
}
int ptext(char **d, int *dm, char *m, char *s, int i)
{//TODO: wrap
	int is;
	char tm[MARG+1];
	*d=as(*d,m,dm);
	for(is=0;is<i-strlen(m);is++){
		tm[is]=' ';
	}
	tm[is]=0;
	*d=as(*d,tm,dm);
	*d=as(*d,s,dm);
	*d=as(*d,"\n",dm);
	return 0;
		
}
char *hrefs[255];
int ntos(tag *t,char *a,int *nr)
{//n must not be zero (obviously)
	//mcheck_check_all();
	static int islink=0;//must do this
	static int istitle=0;
	static int inbody=0;
	static int inhead=0;
	static int suppress=0;
	static int em=0;
	static int headding=0;
	static int acount=0;
	static int islist=0;
	static int lcount=0;
	
	char m[7];
	if(!t)
		return 1;
	char **docm=(char **)a;
	int n=*nr;

	if(t->type){
		if(!strcasecmp(t->type,"a")){
			if(t->closing)
				islink=0;
			else{
				char **p=getprop(t,"href");
				islink=1;
				if(p!=NULL)
					hrefs[acount]=*p;
				acount=acount+1;
			}

		}
		if(!strcasecmp(t->type,"img")){
			char **p=getprop(t,"src");
			islink=1;
			if(p!=NULL)
				hrefs[acount]=*p;
			acount=acount+1;
			ptext(docm,&n,"i","img",MARG);

		}
		if(!strcasecmp(t->type,"ul")){
			islist=1;
			ptext(docm,&n,"","\n",MARG);
		
		}
		if(!strcasecmp(t->type,"li")){
			if(t->closing){
				if(islist>2){
					islist=islist-2;
				}
			}
			if(islist==2)
				lcount=lcount+1;
			islist=islist+2;
		}
		if(!strcasecmp(t->type,"script")){//ehhhhhh
				suppress=!suppress;//TODO: nested suppressed elements (ehh) (no, that wont work)
		}
		if(!strcasecmp(t->type,"hr"))
			ptext(docm,&n,"______","_________________________\n",MARG);
		
		if(!strcasecmp(t->type,"br"))
			ptext(docm,&n," |P","",MARG);
		//*docm=as(*docm,t->type,&n);
	}
	//actuall text content rendering goes here
	if(!suppress){
		int j;
		for(j=0;j<7;j++)
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
			m[2]=' ';
		if(t->freetext)
			ptext(docm,&n,m,t->freetext,MARG);
	}
	*nr=n;
	return 1;
}
char *refs(char *d, int *m, char **refs)
{
	int i;
	char *t;
	d=as(d,"======REFRENCES======\n",m);
	for(i=0;refs[i]!=NULL;i++){
		t=calloc(strlen(refs[i])+14,1);
		sprintf(t,"[%d]:%s\n",i+1,refs[i]);
		d=as(d,t,m);
		free(t);
	}
	return d;
}
char *tomarkdown(tag *root)
{
	char *r=NULL;
	int rm=0;
	r=as(r,"DOCUMENT FOLLOWS\n",&rm);
	sdom(root,ntos,(char *)&r,&rm);
	r=refs(r,&rm,hrefs);
	return r;
}

