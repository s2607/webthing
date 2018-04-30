#include <stdio.h>
#include <malloc.h>
#include <curl/curl.h>
#include "html.h"
#include "url.h"

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

