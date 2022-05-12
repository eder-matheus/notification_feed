#!/bin/bash

if [ "$1" = "clear" ]; then
	rm -rf build
elif [ "$1" = "nuke" ]; then
	rm -rf build
	rm -rf client_env
	rm -rf server_env
  rm -rf front_end_env
else
	if [ ! -d "build" ]; then
		mkdir build
	fi
	cd build
	cmake ..
	make -j 4
	cd -
fi
