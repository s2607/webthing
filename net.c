#include <stdio.h>
#include <malloc.h>
#include <curl/curl.h>
#include "html.h"
typedef struct {
	int m;
	char *s;//null terminated
} netdatas;
CURL *curl;

netdatas *curnetdata;

char *endofproto(char *u)
{//This only gets hung up on ports when the url requests the TLD directly with a non standard port and no explicit protocol EG: google:9980/index.html
//These are rare so we'll handle them later.
//(I mean, to be quite honest there really is no well defined way to interpret that anyway. Why couldn't there be a "google protocol" who's first argument thing is a number?)
	for(;*u&&*u!='.'&&*u!=':'&&*u!='/';u++);
	if(*u==':')
		return u;
	return NULL;
}
char *endofdomain(char *u)
{//This includes the port.
//this cannot tell wether the url contains a valid domain:
//google/index.html as well as swiley.net/google/index.html are both urls that include a domian (google in the first, swiley.net in the second.)
	int slashes=0;
	char *s=endofproto(u);
	if(s==NULL)
		s=u;
	for(slashes=0;*s&&slashes<3;s++){
		if(*s!='/'&&slashes<2)
			slashes=2;
		if(*s=='/')
			slashes=slashes+1;
	}
	return s;
}
char *endofpath(char *u)
{
//all but last elements of path
	char *s=endofdomain(u);
	char *o=s;
	char *b=s;
	for(o=b=s;*s;s++){
		if(*s=='/')
			o=b,b=s;
	}
	return o;
}

size_t netfunc(void *ptr, size_t size, size_t count, void *stream)
{
	//printf("Got %d\n%.*s",count,count,stream);
	curnetdata->s=asn((char *)ptr,(size*count),curnetdata->s,&(curnetdata->m));
	return count;
}

int initcurl(void)
{
	curl = curl_easy_init();
	if(curl)
		return 1;
	return 0;
}
char *cleanurl(char *o, char *n)
{//Assume urls with no protocol are relative, except if we have no base.
//This makes the interpretation of URLs context dependant, which is vaugely horrifying
	int m;
	char *r=NULL;
	if(endofproto(n)==NULL||endofproto(n)==n){
		if(endofproto(o)==NULL||endofproto(o)==o){
			r=asn("http://",7,r,&m);
			r=asn(n,strlen(n),r,&m);
			
		}else {
			r=asn(o,endofpath(o)-o,r,&m);
			r=asn(n,strlen(n),r,&m);
		}
		
	}else
		r=as(r,n,&m);
	if(r[strlen(r)-1]!='/')
		r=as(r,"/",&m);
	return r;
	
}
int gettexturl(char **s, char *oldurl, char *u)
{
	if(!u)
		return -1;
	char *url;
	if(oldurl)
		url=cleanurl(oldurl,u);
	else{
		url=calloc(strlen(u),1);
		memcpy(url,u,strlen(u));
	}
	CURLcode res;
	netdatas n;
	n.s=NULL;
	n.m=0;
	printf("\n FETCH: %s\n",url);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	/* example.com is redirected, so we tell libcurl to follow redirection */
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, netfunc);
	//curl_easy_setopt(curl, CURLOPT_WRITEDATA,&n);//?
	curnetdata=&n;


	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl);
	/* Check for errors */
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

	/* always cleanup */
	*s=n.s;//consumer frees that
	free(url);
	return res;
}
void endcurl(void)
{
	curl_easy_cleanup(curl);
}

