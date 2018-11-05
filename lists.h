
#ifndef LISTS_H 
#define LISTS_H
typedef void ** tagl;
typedef char ** stringl;
//lists always grow until they are freed
void **append(void **l, int *lm, void *a);
//inline defs for specific list types
tagl tlappend(tagl l, int *lm, void *a);
stringl slappend(stringl l, int *lm, void *a);
#endif
