#! /bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# run apeiron-core tests
printf -- "----- Testing apeiron-core -----\n"
pushd apeiron-core/ > /dev/null
pushd build/ > /dev/null
ctest $1
if [ $? -ne 0 ]
then
  printf -- "${RED}Some test(s) for apeiron-core failed!${NC}\n"
  exit -1
fi
printf -- "${GREEN}All tests for apeiron-core passed!${NC}\n"
popd > /dev/null
popd > /dev/null
printf -- "---------------"


exit 0
