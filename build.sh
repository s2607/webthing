#!/bin/sh



cc -O0   -fno-omit-frame-pointer -g newpager.c lists.c url.c html.c cleandump.c net.c -lcurl -lreadline -o web
