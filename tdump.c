#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "html.h"
#include <mcheck.h>

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
	if(*m-strlen(s)<strlen(a)){
		s=realloc(s,*m+strlen(a));
		*m=*m+strlen(a);
		int i=0;
		for(i=0;i<*m-strlen(s);i++)
			s[i+strlen(s)]=0;
	}
	memcpy(s+strlen(s),a,strlen(a)+1);
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
int ntos(tag *t,char *a,int n)
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
	
	char m[7];
	if(!t)
		return n;
	char **docm=(char **)a;

	if(t->type){
		if(!strcmp(t->type,"a")){
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
		if(!strcmp(t->type,"script")){//ehhhhhh
				suppress=!suppress;//TODO: nested suppressed elements (ehh)
		}
		if(!strcmp(t->type,"hr"))
			ptext(docm,&n,"______","_________________________",MARG);
		
		if(!strcmp(t->type,"br"))
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
		if(t->freetext)
			ptext(docm,&n,m,t->freetext,MARG);
	}
	return n;
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
	sdom(root,ntos,(char *)&r,rm);
	printf("%s\n",r);
	//rm is invalid now, r must be copied and freed to be modified
	char *t=NULL;
	rm=0;
	t=as(t,r,&rm);
	free(r);
	r=t;
	printf("%s\n",r);
	r=refs(r,&rm,hrefs);
	return r;
}

