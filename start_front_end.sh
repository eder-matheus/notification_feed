#!/bin/bash

if [ ! -d "front_end_env" ]; then
	mkdir front_end_env
fi

if [ -d "front_end_env" ]; then
	cd front_end_env
  if [ ! -f "front_end" ]; then
    ln -s ../build/src/front_end/front_end .
    ln -s ../config/servers_config.cfg
  fi

	if [ ! $# -eq 0 ]; then
    if [ -f "front_end" ]; then
      ./front_end "$1" "$2"
    fi
		cd -
	fi
fi
