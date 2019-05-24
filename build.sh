#!/bin/bash
set -e
mkdir -p build
pushd build >> /dev/null
	cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/usr/local/Cellar/opencv@3/3.4.5_2/share/OpenCV/ ..
	make -j4
#	make
popd >> /dev/null

#mkdir -p dist
#mv build/calibration/calibration dist/calibration
#mv build/imove_scene/src/imove_scene dist/imove-scene
#mv build/imove_peopleextractor/src/imove_peopleextractor dist/imove-peopleextractor
