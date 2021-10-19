# libdclalgo

Algorithms for flaw detection

# Build

```
rm -rf build; mkdir -p build; cd build
cmake ..
make -j4
```

# Run tests

From the build directory:

```
ctest --output-on-failure
```

## You need to see "100% tests passed"

# Tools description

## 1. Expander

This tool is intended to expand mono8/mono16 image into 4 images using simple algorithm to delete angle difference inside one pixel.

## 2. getImageMaps

This tool is intended to create azimuth, zenith and DoP maps from images.
Usage: getImageMaps <folder_with_images>
Also you can enter flags before folder:

  --no-expand
  
  --no-azimuth
  
  --no-zenith
  
  --no-polar_degree
  
  --only-expandDebayer
  
  --only-expandLite
  
For example `getImageMaps --only-expandDebayer --no-zenith ~/cats_images` will make expanded images using only expandDebayer (without expandLite), save expanded images (as there is no option --no-expandDebayer) and will create azimuth maps and maps of DoP's of all cat pictures in folder ~/cats\_images
