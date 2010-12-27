#!/bin/bash

mkdir -p Build/{podfm,Jivai}

../Jivai-Utils/jutils.bin        \
	build                        \
	output=podfm.bin             \
	main=Source/Main.c           \
	manifest=Manifest.h          \
	include=../Jivai/src         \
	map=Source:Build/podfm       \
	map=../Jivai/src:Build/Jivai \
	optimlevel=0                 \
	inclhdr=config.h             \
	dbgsym=yes                   \
	add=Source/Providers/*.c     \
	link=@bfd                    \
	link=@dl                     \
	link=@m
