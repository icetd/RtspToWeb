#! /usr/bin/bash
CURRENT_DIR=$(pwd)
FFMPEG_VERSION=n4.3
FFMPEG_SRC=FFmpeg-${FFMPEG_VERSION}
FFMPEG_SRC_DIR=${CURRENT_DIR}/thirdparty/${FFMPEG_SRC}
FFMPEG_URL=https://github.com/FFmpeg/FFmpeg/archive/refs/tags/${FFMPEG_VERSION}.tar.gz
FFMPEG_PREFIX=${CURRENT_DIR}/thirdparty/FFmpeg

cd ${CURRENT_DIR}/thirdparty

if [ ! -d "${FFMPEG_PREFIX}" ]
then
	(wget -O "${FFMPEG_SRC}.tar" ${FFMPEG_URL} \
		&& tar -xf "${FFMPEG_SRC}.tar") || exit
	rm -f "${FFMPEG_SRC}.tar"
else
	echo "already build FFmpeg"
fi

if [ ! -d "${FFMPEG_PREFIX}" ]
then
	cd ${FFMPEG_SRC_DIR}
    (./configure --prefix=${FFMPEG_PREFIX} \
    --cc="gcc" --cxx="g++" \
    --target-os=none \
    --cpu=generic --enable-gpl \
    --enable-version3 --disable-avdevice  \
    --disable-postproc --disable-avfilter \
    --disable-programs --disable-logging \
    --disable-everything --enable-avformat  \
    --enable-decoder=hevc --enable-decoder=h264 --enable-decoder=h264_qsv \
    --enable-decoder=hevc_qsv \
    --enable-decoder=aac \
    --disable-ffplay --disable-ffprobe  --disable-asm \
    --disable-doc \
    --disable-hwaccels \
    --disable-parsers --disable-bsfs --disable-debug \
    --enable-protocol=file --disable-indevs --disable-outdevs \
    --enable-parser=hevc --enable-parser=h264 --enable-shared) || exit
	(make -j4 && make install) || exit
	cd ${CURRENT_DIR}
	(rm -rf "${FFMPEG_SRC_DIR}") || exit
else
	echo "already build FFmpeg"
fi