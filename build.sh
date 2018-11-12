#!/bin/sh



cc -O0   -fno-omit-frame-pointer -g lists.c url.c html.c cleandump.c net.c -lcurl -o web
