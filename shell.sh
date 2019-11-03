#!/usr/bin/env bash

exec docker run --rm -it -v $HOME/ws/tinyc:/tinyc -w /tinyc compilerbook
