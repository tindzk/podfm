#!/bin/sh

mkdir -p build/{podfm,Jivai}

../Jivai-Utils/jutils.bin \
	build                 \
	output=podfm.bin      \
	main=src/Main.c       \
	manifest=Manifest.h   \
	include=../Jivai/src  \
	map=src:build/podfm   \
	map=../Jivai/src:build/Jivai \
	optimlevel=0          \
	inclhdr=config.h      \
	dbgsym=yes            \
	add=src/Providers/*.c \
	link=@bfd             \
	link=@m
