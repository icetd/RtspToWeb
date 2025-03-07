## RtspToWeb
Read Rtsp url and send h264 raw stream to websocket frame by frame

## build
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

## use
example json send websocket client
```
{"cmd": "start","url": "rtsp://192.168.2.105:8555/unicast"}
{"cmd": "stop","url": "rtsp://192.168.2.105:8555/unicast"}
```

## configs
```
[server]
port = 9000    ; websocket port
threadnum = 4  ; EventLoop thread num

[log]
level = 4      ; log level
; TRACE = 0,
; DEBUG = 1,
; INFO = 2,
; WARN = 3,
; ERROR = 4,
; FATAL = 5,
basename = server ; log output base name
```