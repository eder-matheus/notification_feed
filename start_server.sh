#!/bin/bash

if [ ! -d "server_env" ]; then
	mkdir server_env
fi

if [ -d "server_env" ]; then
	cd server_env
	if [ ! -f "server" ]; then
		ln -s ../build/src/server/server .
	fi
	./server
	cd -
fi