# libdclfilters
Filters for TMK_DCL projects group

# Build

```
rm -rf build; mkdir -p build; cd build
cmake ..
make -j4
```

# Run

Run module
```
python3 mandel_detection -f /mnt/tmk/testdb/40000
```
or test
```
python3 test.py
```
Attention: hard code </mnt/tmk/testdb> in the test_case.json

# Test

Run from te build directory

```
ctest --output-on-failure
```
or with pytest
```
python3 -m pytest test.py
```

# Dependencies

Install dependencies:
```
apt-get update && apt-get install -y libgl-dev
pip3 install imutils opencv-contrib-python
```
for tests:
```
pip3 install pytest
```
# Parameters
```
  "H1" -- first height (int)
  "H2" -- second height (int)
  "DILATE_ITERATIONS" -- number of dilate iterations (int)
  "BLOCK_SIZE_THRESH" -- Size of block for threshold function (int, odd) 
  "C_THRESH" -- Threshold const (int)
  "MAX_VALUE_THRESH" -- Max value for threshold function (int)
  "GAMMA" -- parameter for gamma-correction (float)
  "SUB_CONST" -- subtraction or addition constant (int)
  "EPS" -- min radii difference
  "R_QUE_LEN" -- size of queue for radii
  "M_QUE_LEN": -- size of queue for mean radii
  "SCALE" -- frame scaling percentage
 ```
