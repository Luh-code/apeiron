#! /bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# build apeiron-core
printf -- "----- Building apeiron-core -----\n"
pushd apeiron-core/ > /dev/null
cmake -DGLFW_USE_WAYLAND=ON -DENABLE_WAYLAND=ON -G Ninja -B ./build . 
if [ $? -ne 0 ]
then
  printf -- "${RED}Failed to generate build files for apeiron-core!${NC}\n"
  exit -1
fi
printf -- "${GREEN}Successfully generated build files for apeiron-core!${NC}\n"
cmake --build ./build 
if [ $? -ne 0 ]
then
  printf -- "${RED}Failed to build apeiron-core!${NC}\n"
  exit -1
fi
printf -- "${GREEN}Successfully built apeiron-core!${NC}\n"
popd > /dev/null
printf -- "---------------\n"


exit 0
