#!/bin/bash

if [ ! -d "server_env" ]; then
	mkdir server_env
fi

if [ -d "server_env" ]; then
	cd server_env
	ln -s ../build/src/server/server .
	./server
	cd -
fi