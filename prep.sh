#!/bin/sh

conan remote add conan-center "https://conan.bintray.com"

git clone https://github.com/darcamo/conan-cxxopts || exit 1
cd conan-cxxopts && conan create . local/stable || exit 1
cd .. && rm -rf conan-cxxopts

#git clone https://github.com/msaf1980/conan-plog || exit 1
#cd conan-plog && conan create . local/stable || exit 1
#cd .. && rm -rf conan-plog

mkdir contrib
cd contrib && {
    git submodule add https://github.com/cameron314/concurrentqueue
    git submodule add https://github.com/SergiusTheBest/plog
    git submodule update --init
}
