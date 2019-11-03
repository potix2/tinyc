#!/usr/bin/env bash

exec docker run --rm -v $HOME/ws/tinyc:/tinyc -w /tinyc compilerbook "$@"