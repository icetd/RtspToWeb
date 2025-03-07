## RtspToWeb

```
sudo apt install -y \
libavcodec-dev \
libavformat-dev \
libavutil-dev 

git submodule update --init

mkdir build && cd build
cmake ..
make -j8
```