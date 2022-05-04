#!/bin/bash

if [ ! -d "server_env" ]; then
	mkdir server_env
fi

if [ -d "server_env" ]; then
	cd server_env
	if [ ! -f "server" ]; then
		ln -s ../build/src/server/server .
	fi
  if [ ! -f "servers_config.cfg" ]; then
    ln -s ../config/servers_config.cfg
  fi
  if [ ! $# -eq 0 ]; then
    for (( i = 1; i <= $1; i++ )); do
	    ./server $i &;
    done
  fi
	cd -
fi