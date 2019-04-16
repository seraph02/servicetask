cd POC 
sh ../ln.sh
chmod +x ./*
cd ..
rm -rf ../build
mkdir -p ../build
cd ../build
cmake ../servicetask
make -j24
ln -s servicetask/POC POC 
cp ../servicetask/servicecore.conf ./
rm -rf B* C* D* M* c* e*
mv POC ../

