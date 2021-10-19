# dclweb
TMK_DCL nginx web server

# Build and run server
```
docker build --file Dockerfile --tag test/nginx:v0 .
docker run -d --name nginx -p 5080:80 -ti -v /mnt/tmk/testdb:/nginx/media/testdb -v /mnt/tmk/fsdb:/nginx/media/fsdb test/nginx:v0
```

# Download
Ssh-port forward, view directory with fs-data and download image
```
ssh -p 9922 -L 5080:localhost:5080 kat@85.26.137.119
http://localhost:5088/media/
http://localhost:5080/media/testdb/20000/SPA0.bmp
```
For test webserver GET request `/media/alive`, should return `'Media server is OK'`
