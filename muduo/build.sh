#!/bin/sh

MUDUO_DIRECTORY="/home/shobhit/work/muduoInstall/build/debug-install"
MUDUO_INCLUDE="$MUDUO_DIRECTORY/include"
MUDUO_LIBRARY="$MUDUO_DIRECTORY/lib"
SRC="."

CXXFLAGS="-g -O0 -Wall -Wextra -Werror \
	   -Wconversion -Wno-unused-parameter \
	   -Wold-style-cast -Woverloaded-virtual \
	   -Wpointer-arith -Wshadow -Wwrite-strings \
	   -march=native -rdynamic"

LDFLAGS="-lmuduo_net -lmuduo_base -lpthread -lrt"

clear
echo "Compiling *.cc to $1"
OP="g++ *.cc -o $1  $CXXFLAGS   -I $MUDUO_INCLUDE -L $MUDUO_LIBRARY $LDFLAGS"

echo $OP
exec $OP

