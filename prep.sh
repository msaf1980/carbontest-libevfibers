#!/bin/sh

conan remote add conan-center "https://conan.bintray.com"

git clone https://github.com/darcamo/conan-cxxopts || exit 1
cd conan-cxxopts && conan create . local/stable || exit 1
cd .. && rm -rf conan-cxxopts

git clone https://github.com/msaf1980/conan-libevfibers || exit 1
cd conan-libevfibers || exit 1
git checkout 0.4.1-57-g8f0a419 || exit 1
conan create . local/stable || exit 1
cd .. && rm -rf conan-libevfibers

#git clone https://github.com/msaf1980/conan-plog || exit 1
#cd conan-plog && conan create . local/stable || exit 1
#cd .. && rm -rf conan-plog

#cd contrib && {
#  git submodule add https://github.com/cameron314/concurrentqueue
#  git submodule add https://github.com/SergiusTheBest/plog
#  git submodule add https://github.com/msaf1980/c_procs
#  git submodule add https://github.com/msaf1980/cpp_procs
#}

git submodule update --init

