
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "url.h"
//TODO WARNING : This wil leak memory since there is no url destructor
int hasmethod(char *u)
{
	char *o;
	for(o=u;*u;u++){
		if(*u=='/')
			break;
		if(*u=='.')
			break;//specificially does not correctly handle the url "google" or any other google based urls 
		if(*u==':')
			return 1+u-o;
	}
	return 0;
}

int hasport(char *u)
{
	int b=hasmethod(u);
	if(!b)
		b=0;//duh
	else 
		b=b-1;//udndo the thing

	int o;
	for(o=b;u[b];b++){
		if(u[b]=='/')
			break;
		if(u[b]==':')
			return 1+b-o;
	}
	return 0;
}

int hasresource(char *u)
{
	int b=hasmethod(u)+2;
	int c=hasport(u);
	if(!c)
		c=b;
	if(c)
		c=c-1;
	for(;u[c];c++)
		if(u[c]=='/')
			break;
	if(!u[c])
		return 0;
	return c;
}

int atourl(char *u, parsedurl *pu)
{
	if(u==NULL)
		return 0;
	int m=hasmethod(u);
	if(m){
		m=m-1;
		printf("%d\n",m);
		pu->method=calloc(m+1,1);
		memcpy(pu->method,u,m);
	}else m=0;//duh
	int bb,be;
	bb=m;
	if(bb)
		bb=bb+3;//skip the slashes, blame the user if there are none.
	be=hasport(u);
	//if(!be) {be=hasresource(u);}
	if(be)
		be=be-1;//cuz
	if(!be) {be=strlen(u+bb);}
	if(be&&be-bb){
		pu->base=calloc(1+be-bb,1);
		memcpy(pu->base,u+bb,be-bb);
	}
	pu->port=80;//TODO:port
	int pb=be-3;
	int pe=strlen(u);
	printf("%d %d\n",pe,pb);
	if(pe-pb){
		pu->resource=calloc(pe-pb,1);
		memcpy(pu->resource,u+pb,pe-pb);
	}
	return 1; //whatever

}
char *cleanurl(char *o,char *n)
{
	parsedurl *old=calloc(sizeof(parsedurl),1);
	parsedurl *new=calloc(sizeof(parsedurl),1);
	char *ret=NULL;
	int retsize=0;
	int i=0;
	atourl(n,new);
	if(new->method!=NULL){
		ret=calloc(retsize=strlen(n),1);
		memcpy(ret,n,retsize);
	}else if(atourl(o,old)){
		if(old->method!=NULL){
			retsize=retsize+strlen(old->method);
		}
		if(old->base!=NULL){
			retsize=retsize+strlen(old->base);
		}
		retsize=retsize+strlen(n);
		ret=calloc(retsize,1);	
		if(new->method!=NULL)
			strncpy(ret,old->method,retsize-i);
		else
			strncpy(ret+i,"http://",retsize-i);
		i=strlen(ret);
		if(new->base!=NULL)
			strncpy(ret+i,old->base,retsize-i);
		i=strlen(ret);
		strncpy(ret+i,n,retsize-i);
		
	}
	return ret;
}
