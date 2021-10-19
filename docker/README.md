# docker
Collection of 'Dockerfile's for building and testing

# Build
For example, docker image for build and test TMK_DCL:
```
docker build --file Dockerfile_ubuild --tag tmk/build:v0
```
# Run
These dockers are for build and test application in the known or new enviroments. 

Please, if you run it directly, be sure that your container (but not image) deleted after usage.
```
id=$(date +%Y-%m-%d_%H-%M-%S)
export TESTDB_DIR="/mnt/tmk/testdb"
docker run --rm --name build_${id} -v ${TESTDB_DIR}:/mnt/tmk/testdb -v $(pwd):/tmk_dcl tmk/build:v0  /bin/bash -c 'cd tmk_dcl && docker/build.sh'
#docker rm -f build_${id}
```
