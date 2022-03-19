#!/bin/bash

if [ ! -d "client_env" ]; then
	mkdir client_env
fi

if [ -d "client_env" ]; then
	cd client_env
	if [ ! -f "client" ]; then
		ln -s ../build/src/client/client .
	fi

	if [ ! $# -eq 0 ]; then
		if [ -f "client" ]; then
			./client $1 $2 $3
		fi
		cd -
	fi
fi