#include <stdio.h>
#include "pager.h"

char *startof(char *s, int l)
{
	char *o=s;
	while((*s)&&l){
		s++;
		if(*s=='\n')
			l--;
	}
	return s;
}
int lcount(char *s)
{
	int l=0;
	while(*s){
		s++;
		if(*s=='\n')
			l++;
	}
	return l;
}

void pline(char *s, int l)
{
	char *start;
	char *end;
	start=startof(s,l);
	end=startof(s,l+1);
	
	while(end!=start)
	{
		printf("%c",*start);
		start++;
	
	}
}
void ppage(char *s, int p)
{
	int lc=lcount(s);
	int a=p*20;
	int b=(p+1)*20;
	if(b>lc)
		b=lc;
	if(a>=b){
		printf("\nEnd %d >",lc);
		return;
	}
	while(a!=b){
		pline(s,a);
		a++;
	}
	printf("\nLines: %d,%d >",p*20,b);
}
int nullmenu(char *s, int *a, char **b)
{
	if(*s=='q')
		return -1;
	return 0;
}
#define BUFFLEN 16000
void pagethrough(char *s,mfp m,char **b)
{
	int p=0;
	char buff[BUFFLEN];
	if(m==NULL)
		m=nullmenu;

	while(1){
		if(p<0)
			p=0;
		ppage(s,p);
		p++;
		gets(buff);
		int r=0;
		if(*buff&&!(r=m(buff,&p,b))&&!buff[1]){
			if(r<0)
				return;
			switch(*buff){
				case 'b':p=p-2;continue;
				case '?':printf("enter page down, b page up\n");m('?',&p,b);continue;
				case 'g':p=0;break;
				case 'G':p=lcount(s)/20;break;
				default: printf("No command %s\n",buff);continue;
			}
		}
		if(r<0)
			return;
		if(r==0&&*buff)
			printf("No command:%s\n",buff);
	}
}
