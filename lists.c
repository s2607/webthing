
#include <malloc.h>
#include <stddef.h>
#include "html.h"
#include "lists.h"

//lists always grow until they are freed
void **append(void **l, int *lm, void *a)//put pointer at end of list, growing backing buffer if nescesary
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
	if(len==(*lm)-1){//the extra elment garuntees null terminator
		l=realloc(l,*lm+PRLC);
		*lm=*lm+PRLC;
		int i;
		for(i=0;i<PRLC;i++)
			(l)[*lm+i]=NULL;
	}
	(l)[len]=a;
	return l;
}

tagl tlappend(tagl l, int *lm, void *a){return (tagl)append((void **)l,lm,a);}//heh
stringl slappend(stringl l, int *lm, void *a){return (stringl)append((void **)l,lm,a);}//heh
