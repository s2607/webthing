#!/bin/sh



cc -g lists.c url.c html.c tdump.c net.c -lcurl -o web
