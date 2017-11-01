#!/bin/sh
mkdir -p ../build-host
pushd ../build-host
cmake ../test
make -j8 && ./soundrecordingtest
popd