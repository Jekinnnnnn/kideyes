#!/bin/bash
#build ffmpeg 64bit with msvc
./configure \
    --disable-doc \
    --disable-ffmpeg \
    --disable-ffplay \
    --disable-ffprobe \
    --disable-avdevice \
    --disable-encoders \
    --disable-symver \
    --disable-postproc \
    --disable-avfilter \
    --disable-network \
    --target-os=win64 \
    --arch=x86_64 \
    --toolchain=msvc \
    --enable-cross-compile \
    --prefix=./build_output \
&& echo "finish configure" \
&& make clean \
&& echo "finish make clean" \
&& make \
&& echo "finish make" \
&& make install \
&& echo "finish make install"
