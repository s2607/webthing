#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
typedef struct {
	int closing;
	char **pn; //a list of immutable strings
	int pnm;
	char **pv; //same but changes sometimes
	int pvm;
	char *type;
	void **child; //a list of these
	int childm;
	void *parent;
	char *freetext;
} tag;
//trade space for time
#define PRLC 100

//lists always grow until they are freed
void **append(char **l, int *lm, void *a)//put pointer at end of list, growing backing buffer if nescesary
//so much void *
{
	if(!l){
		l=calloc(PRLC,sizeof(char *));
		*lm=PRLC;
	}
	if(!a)
		return l;
	int len;
	for(len=0;(l)[len]&&len<*lm;len++);
	if(len==*lm){
		l=realloc(l,*lm+PRLC);
		*lm=*lm+PRLC;
		int i;
		for(i=0;i<PRLC;i++)
			(l)[*lm+i]=NULL;
	}
	(l)[len]=a;
	return l;
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
	r->type=calloc(4,1);
	memcpy(r->type,"itf",4);
	r->freetext=calloc(strlen(it),1);
	memcpy(r->freetext,it,strlen(it));
	free(it);
}
int closed(tag *t)
{
	int len;
	for(len=0;t->child[len];len++){
		tag *c=(tag *)t->child[len];
		if(c->closing){
			if(!strcmp(t->type,c->type))
				return 1;
		}
	}
	return 0;
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
	printf("t:%s ",type);
	for(i=1;strlen(voids[i]);i++)
		if(!strcmp(type,voids[i])) 
			return -(i+1);
	printf(" not void\n");
	for(i=0;strlen(raws[i]);i++)
		if(!strcmp(type,raws[i])) 
			return i+1;
	printf(" not raw\n ");
	return 0; //a normal node
	

	/*char *blocks[] = {"pre","p","h1","h2","h3","h4","h5","h6","noscript","div",""};
	char *inlines[] = {"a","em","strong","code","kbd","samp","var","b","b","i","u","small","s","strike","*/
}

char *rtag(tag *t, char *s, char *supername,int state)
{
//TODO: non html inner text (script, comment, pre): replace mode 3 with mode 4 that just fills inner text
//TODO: don't eat syntax characters in raw tags
	//0 tagname 1 propname  2 propval 3 freetext 
	char *freetext=NULL;
	int freetextm=0;
	int typem=0;
	char **curs=&(t->type);
	int *curm=&typem;
	char **cursn;

	int tm;//used for reallocing property lists entrys
//	for(state=0;*s&&!(t->closing&&state>=1);s=s+1){
	for(state=0;*s;s=s+1){
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
					if(!strncmp(v,t->type,u-v))
						return u;
				}else
				break;
			case '/': if(state==0){t->closing=1;continue;} break;
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
					printf("state:%d b:%d ",state,b);
					break;
				} 
				else break;
			case '=': if(state==1){state=2; tm=PRLC; curm=&tm; curs=cursn; continue;}
			case ' ': if(state==2||state==0){
				state=1;
				//new property, must also alloc name and value
				t->pn=append(t->pn,&(t->pnm),calloc(sizeof(char),PRLC));
				tm=PRLC;
				int tail;
				for(tail=0;t->pn[tail];tail++);
				curs=&(t->pn[tail-1]);
				//alloc value
				
				t->pv=append(t->pv,&(t->pvm),calloc(sizeof(char),PRLC));
				//NOTE: on state 2 we must remember we calloced PRLC elemnts
				for(tail=0;t->pn[tail];tail++);
				cursn=&(t->pv[tail-1]);
				continue;
				
				} if(state==1) continue; //dont put space in property name
			default: //some text or name or attribute

				if(!(*curs&&strlen(*curs)<=*curm)){//ensure room for next char, demorgans
					if(!*curs){
						*curs=calloc(PRLC,1);
						*curm=PRLC;
					}else{
						*curs=realloc(*curs,*curm+PRLC);
						int flen,i;
						for(i=0;i<PRLC;i++)
							*curs[i+*curm]=0;
						*curm+=PRLC;
					}
				}
				(*curs)[strlen(*curs)]=*s;
			break;
		}
	}	
	newchildtext(t,freetext);
	return s+1;
}
void dump(tag *root,int i)
{
	int j,k;
	for(j=0;j<=i;j++)printf(" ");
	if(strcmp(root->type,"itf"))
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
int main()
{
	tag *root=newchild(NULL);
	rtag(root,"<html><title>hello</title><body> hello world <hr> wooot! <script> var a=\"<html>blah</html>\"; </script>  <a href=\"test\"> test </a> </body></html>","!",3);
	dump(root,1);
	return 0;
}
