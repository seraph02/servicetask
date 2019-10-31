mkdir -p ../build
cd ../build
rm -rf ServiceCore lib dataclassify
cmake ../servicetask
make -j24
rm -rf B* C* D* M* c* ela* db* expcla*


