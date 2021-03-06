#include <stdio.h>
#include <malloc.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "html.h"
#include "url.h"
#include "net.h"

typedef struct {
	int m;
	char *s;//null terminated
} netdatas;
CURL *curl;

netdatas *curnetdata;

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
int gettexturl(char **s, CURLU **oldurl, char *u, int method, char *fields)
{
	if(u == NULL)
		return -1;
	if(oldurl == NULL)
		return -2;
	if(*oldurl == NULL) 
		*oldurl = curl_url();
	CURLUcode res = curl_url_set(*oldurl,CURLUPART_URL,u,CURLU_DEFAULT_SCHEME);
	if(res){
		fprintf(stderr, "curl_url_set() failed: %s\n", curl_easy_strerror(res));
		return res;
	}
	netdatas n;
	n.s=NULL;
	n.m=0;
	char *urlt=NULL;//NEEDS TO BE FREED?
	curl_url_get(*oldurl,CURLUPART_URL,&urlt,0);
	printf("\n FETCH: %s\n",urlt);
	if(fields!=NULL){
		if(method==METHOD_POST)
			curl_easy_setopt(curl,CURLOPT_POSTFIELDS, fields);
		else if(method==METHOD_GET){
			char *urltt=calloc(strlen(urlt)+strlen(fields)+2,1);
			strcpy(urltt,urlt);
			strcat(urltt,"?");
			strcat(urltt,fields);
			free(urlt);
			urlt=urltt;
		} else {
			printf("UNKOWN METHOD!\n");
			exit(1);
		}
	}
	curl_easy_setopt(curl, CURLOPT_URL, urlt);
	/* example.com is redirected, so we tell libcurl to follow redirection */
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, netfunc);
	printf("TODO: verfy remote cert. THIS CONNECTION IS UNAUTHENTICATED!!!\n");
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	//curl_easy_setopt(curl, CURLOPT_WRITEDATA,&n);//?
	curnetdata=&n;
	//form stuff.


	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl);

	/* Check for errors */
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

	*s=n.s;//consumer frees that
	free(urlt);
	return res;
}
void submitform(tag *root, int n) {
	
}
void endcurl(void)
{
	curl_easy_cleanup(curl);
}

