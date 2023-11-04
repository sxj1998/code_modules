cd build
cmake ..
make
cp TESTC_CPP ../debug/
scp TESTC_CPP root@192.168.2.8:/userdata/testcode
cd ../debug/
./arm-linux-gnueabihf-objdump -ld TESTC_CPP > TESTC_CPP.txt

