#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "html.h"
#include "pager.h"
#include "net.h"
#include "tdump.h"

char *scrubquotes(char *s)
{
	char *a=s;
	char *b=s+strlen(s)-1;
	if(*a=='"')
		a=a+1;
	if(*b=='"')
		b=b-1;
	char *r;
	r=calloc(b-a+1,1);
	memcpy(r,a,b-a+1);
	r[b-a+1]=0;
	return r;
}

tag *newchild(tag *t)
{
	tag *r=calloc(sizeof(tag),1);
	r->parent=t;

	if(t){
		t->child=append(t->child,&(t->childm),r);
	}
	return r;
}
void newchildtext(tag *t,char *it)
{
	if(!it)
		return;
	tag *r;
	r=newchild(t);
	r->type=calloc(4,1);
	memcpy(r->type,"itf",4);
	r->freetext=calloc(strlen(it)+1,1);
	memcpy(r->freetext,it,strlen(it));
	r->freetext[strlen(it)]=0;
	free(it);
}
int closed(tag *t)
{
	int len;
	for(len=0;t->child[len];len++){
		tag *c=(tag *)t->child[len];
		if(c->closing){
			if(!strcasecmp(t->type,c->type))
				return 1;
		}
	}
	return 0;
}
tag *sdom(tag *r, int(*v)(tag *,char **,int*), char **d, int *nr)
{
	//search/visit the DOM (in order)
	//visits all dom nodes until v returns FALSE, then returns pointer to that node
	if(!r)
		return NULL;
	
	if(!v(r,d,nr))
		return r;
	tag *rv;
	if((r->child)==NULL)
		return NULL;
	if(*(r->child)==NULL)
		return NULL;
	int i;
	for(i=0;r->child[i];i++){
		rv=sdom(r->child[i],v,d,nr);
		if(rv)
			return rv;
	}
	
	return NULL;
	
}
char **getprop(tag *t,char *n)
{
	if(!t)
		return NULL;
	if(!t->pn)
		return NULL;
	
	int i;
	for(i=0;t->pn[i]!=NULL;i++){
		if(!strcasecmp(n,t->pn[i]))
			return &(t->pv[i]);
	}
	return NULL;
}
int bytype(tag *r, char *t, int n)
{
	if(!r)
		return n;
	if(!r->type)
		return n;
	if(r->closing)
		return n;
	if(!strcasecmp(t,r->type))
		return n-1;
	return n;
}
int isbaren(char *type)
{//font is ignored intentionally, DO NOT ADD IT
//a negative value means the parser should terminate imediately on a close bracket
//a positive value means the parse should place all of the inner text in  a single text fragment node
//the main point of this function is to answer the question "can this node have children?" 
	char *voids[] = {"br","hr","img","link","!--",""};//TODO: correct comment syntex (friggen wacked)
	char *raws[] = {"script","plaintext",""};//plaintext shouldn't end but whatever
	int i;
	if(!type)
		return -9999;//ehhhh
	if(*type=='/')
		type=type+1;
	//printf("t:%s ",type);
	for(i=1;strlen(voids[i]);i++)
		if(!strcasecmp(type,voids[i])) 
			return -(i+1);
//	printf(" not void\n");
	for(i=0;strlen(raws[i]);i++)
		if(!strcasecmp(type,raws[i])) 
			return i+1;
//	printf(" not raw\n ");
	return 0; //a normal node
	

	/*char *blocks[] = {"pre","p","h1","h2","h3","h4","h5","h6","noscript","div",""};
	char *inlines[] = {"a","em","strong","code","kbd","samp","var","b","b","i","u","small","s","strike","*/
}

char *rtag(tag *t, char *s, char *supername,int state)
{
//TODO: non html inner text (script, comment, pre): replace mode 3 with mode 4 that just fills inner text
	//0 tagname 1 propname  2 propval 3 freetext 
	char *freetext=NULL;
	int freetextm=0;
	int typem=0;
	char **curs=&(t->type);
	int *curm=&typem;
	char **cursn;

	int tm;//used for reallocing property lists entrys
//	for(state=0;*s&&!(t->closing&&state>=1);s=s+1){
	for(;*s;s=s+1){
		switch(*s){
			case '<': if(state==3){ 
				newchildtext(t,freetext);
				freetextm=0;
				s=rtag(newchild(t),s+1,t->type,0); 
				if(closed(t)){return s;} 
				freetext=NULL;
				freetextm=0;
				curs=&freetext;
				curm=&freetextm;
				continue; 
				} else if(state == 4){//check for closing raw tag
					char *u;
					char *v;
					for(u=v=s+1;*u&&*u!=' '&&*u!='>';u=u+1){
						if(*v=='/')
							v=v+1;
					}
					char *u1;
					char *v1;
					if(!strncasecmp(v,t->type,u-v)){
						tag *c=newchild(t);
						c->type=calloc(strlen(t->type)+1,1);
						memcpy(c->type,t->type,strlen(t->type));
						c->closing=1;
						
						return u;
					}
				}else
				break;
			case '/': if(state==0){t->closing=1;continue;}break; 
			case '>': 
				if(state<3){
					int b=isbaren(t->type);
					if(b<0)
						return s;//void tag like br
					if(b>0)
						state=4;
					else
						state=3;
					curs=&t->freetext;
					curm=&freetextm;
					if(t->closing==1)
						return s; 
					//printf("state:%d b:%d ",state,b);
					continue;
				} 
			case '=': if(state==1){state=2; tm=PRLC; curm=&tm; curs=cursn; continue;}
			case ' ': if(state==2||state==0){
				state=1;
				//new property, must also alloc name and value
				t->pn=slappend(t->pn,&(t->pnm),calloc(sizeof(char),PRLC));
				tm=PRLC;
				int tail;
				for(tail=0;t->pn[tail];tail++);
				curs=&(t->pn[tail-1]);
				//alloc value
				
				t->pv=slappend(t->pv,&(t->pvm),calloc(sizeof(char),PRLC));
				//NOTE: on state 2 we must remember we calloced PRLC elemnts
				for(tail=0;t->pn[tail];tail++);
				cursn=&(t->pv[tail-1]);
				continue;
				
				} if(state==1) continue; //dont put space in property name
			}//switch
			//some text or name or attribute

			if(!(*curs&&strnlen(*curs,*curm)<=*curm-1)){//ensure room for next char, demorgans
				if(!*curs){
					*curs=calloc(PRLC,1);
					*curm=PRLC;
				}else{
					char *t=realloc(*curs,*curm+PRLC+1);
					if(t==NULL)
						printf("BAD CHEESE!\n");
					*curs=t;
					int flen,i;
					for(i=0;i<PRLC+1;i++)
						(*curs)[i+*curm]=0;
					*curm+=PRLC+1;
				}
			}
			(*curs)[strlen(*curs)]=*s;
	}	//for
	newchildtext(t,freetext);
	return s+1;
}
void dump(tag *root,int i)
{
	int j,k;
	for(j=0;j<=i;j++)printf(" ");
	if(root->type&&strcasecmp(root->type,"itf"))
		printf("%s:\n",root->type);
	for(j=0;j<=i;j++)printf(" ");
	if(root->pn)
		for(k=0;root->pn[k];k++)
			printf(" %s=%s",root->pn[k],root->pv[k]);
	printf("\n");
	if(root->freetext){
		for(j=0;j<=i;j++)printf(" ");
		printf("inner: %s\n",root->freetext);
	}
	if(root->child)
		for(k=0;root->child[k];k++)
			dump(root->child[k],i+1);
	
}
int linkmenu(char *c,int *p,char **u)
{
	if(*c=='?'){
		printf("Type a refrence number to jump there.\n");
		printf("Type a u followed by a url to go there\n");
		printf("Type a h to view history\n");
		return 1;
	}
	if(getref(atoi(c))!=NULL){
		char *n=getref(atoi(c));
		printf("selected %d, %s\n",atoi(c),n);
		*u=calloc(strlen(n),1);
		memcpy(*u,n,strlen(n));
		return -1;
	}
	return 0;
}
/*char *viewpage(char *url, char *oldurl)
{
	char *t;
	char *u=url;

	gettexturl(&t,oldurl,u);
	tag *root=newchild(NULL);
	rtag(root,t,"!",3);
	char *text=tomarkdown(root);
	char *ut;
	pagethrough(text,linkmenu,&ut);

	free(t);
	free(text);
	return ut;
}*/
void bloop(char *starturl)
{
	char *t=NULL;
	CURLU *oldurl=NULL;
	gettexturl(&t,&oldurl,starturl);
	tag *root=newchild(NULL);
	rtag(root,t,"!",3);
	char *text=tops(root);
	puts("end tops");
	puts(text);
	//tdump();
	free(t);
	free(text);
}
int main(int argc, char **argv)
{
	if(!initcurl()||argc!=2);
		//return -1;
	//viewpage(argv[1]);
	bloop("http://swiley.net");
//	bloop("http://192.30.252.153");
	endcurl();
	return 0;
}
