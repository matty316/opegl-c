#!/bin/bash

cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
make
cp -r ../shaders .
./$1
