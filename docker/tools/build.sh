echo "Building..."
rm -rf build; mkdir -p build; cd build
ln -s /mnt/tmk/testdb/libdclalgo data
ls -la data
cmake ..
make -j${nproc}
echo "Testing..."
ctest --output-on-failure
cd ..; rm -rf build
