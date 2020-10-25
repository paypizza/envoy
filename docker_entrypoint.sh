#!/usr/bin/env sh
set -e

# if the first argument look like a parameter (i.e. start with '-'), run Envoy
if [ "${1#-}" != "$1" ]; then
	set -- envoy "$@"
fi

if [ "$1" = 'envoy' ]; then
	# set the log level if the LOGLEVEL variable is set
	if [ -n "$LOGLEVEL" ]; then
		set -- "$@" --log-level "$LOGLEVEL"
	fi
fi

exec "$@"