#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STRLEN 10

typedef struct {
	int used;
	char  name[STRLEN];
	char  val[STRLEN];
} prop;
typedef struct {
	char  name[STRLEN];
	char  type[STRLEN];
	void *parent;
	void *c[STRLEN];//NULL terminated list of children
	prop p[STRLEN];
	
} el;
//(a n=v (b n=v)) and so on

el *newchild(el *e)
{
	el *r;
	r=calloc(sizeof(el),1);
	r->parent=e;
	if(e){
		int i;
		for(i=0;i<STRLEN&&e->c[i];i++);
		if(i==STRLEN)
			free(r);
		else
			e->c[i]=r;
	}
	return r;
}

void addprop(el *e, char *pn, char *pv)
{
	if(!pn||!pv||!e||!pn[0]||!pv[0])
		return;
	int i=0;
	for(i=0;i<STRLEN&&e->p[i].used;i++);
	if(i==STRLEN)
		return;
	e->p[i].used=1;
	memcpy(e->p[i].name,pn,strlen(pn));
	memcpy(e->p[i].val,pv,strlen(pv));

}
char *rel( el* e, char *s)
{ 
	int state;
	int pc;
	char pn[STRLEN];
	char pv[STRLEN];
	for(state=pc=0;;s=s+1,pc=pc+1){
		switch(*s){
			case ')':
			case 0: return s+1;
			case '(': addprop(e,pn,pv);s=rel(newchild(e),s+1);break;
			case ' ': if(state!=1){state=1; //parsing new property
				//but only if we're not parsing the p name
				addprop(e,pn,pv);
				int j;
				for(j=0;j<STRLEN;j++)
					pn[j]=0,pv[j]=0;
				}else state=2;
				break;
			case'=': state=2; break;
			default:
				switch(state){
					
					case 0: e->name[strlen(e->name)]=*s;break;
					case 1: pn[strlen(pn)]=*s;break;
					case 2: pv[strlen(pv)]=*s;break;
				}
		}
	}
}
void dump(el *root, int i)
{
	int j,k;
	for(k=0;k<i;k++)printf(" ");
	printf("(%s)",root->name);
	for(j=0;root->p[j].used;j++)printf("%s=%s ",root->p[j].name,root->p[j].val);
	printf("\n");
	for(j=0;root->c[j];j++)dump(root->c[j],i+1);
}
	
int main()
{
	el *root=newchild(NULL);
	rel(root,"abc b=c l=m n=v (d e=g f=n) (uv y=x)");
	dump(root,1);
	return 0;
}
