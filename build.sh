#!/bin/bash

if [ "$1" = "clear" ]; then
	rm -rf build
elif [ "$1" = "nuke" ]; then
	rm -rf build
	rm -rf client_env
	rm -rf server_env
else
	mkdir build
	cd build
	cmake ..
	make -j 4
	cd -
fi
