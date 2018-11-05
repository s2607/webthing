#!/bin/sh



cc -g url.c html.c tdump.c pager.c net.c -lcurl -o web
