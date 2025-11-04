#!/bin/sh
export LD_LIBRARY_PATH=.
export LDPRELOAD=libft_malloc.so
$@