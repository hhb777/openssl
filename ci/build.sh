#!/bin/bash
# by KangLin(kl222@126.com)

set -e

cd $1

PROJECT_DIR=`pwd`

cd ${PROJECT_DIR}
mkdir build
cd build

OPENSSL_BUILD_PREFIX=${PROJECT_DIR}/setup

if [ "$Configuration" = "shared" ]; then
    MODE=shared
else
    MODE="no-shared no-pic"
fi

case ${BUILD_TARGERT} in
    windows_msvc)
        pacman -S --noconfirm nasm
        export PATH=/c/Perl/site/bin:/c/Perl/bin:$PATH
        rm /usr/bin/link
        if [ "${Platform}" = "x64" ]; then
            TARGET=VC-WIN64A-masm
        else
            TARGET=VC-WIN32
        fi
        MAKE="nmake"
    ;;
    windows_mingw)
        pacman -S --noconfirm nasm
        case ${TOOLCHAIN_VERSION} in
            630)
                if [ "${Platform}" = "x64" ]; then
                    MINGW_PATH=/C/mingw-w64/x86_64-6.3.0-posix-seh-rt_v5-rev1/mingw64
                else
                    MINGW_PATH=/C/mingw-w64/i686-6.3.0-posix-dwarf-rt_v5-rev1/mingw32
                fi
            ;;
            530)
                if [ "${Platform}" = "x86" ]; then
                    MINGW_PATH=/C/mingw-w64/i686-5.3.0-posix-dwarf-rt_v4-rev0/mingw32
                else
                    echo "Don't support ${TOOLCHAIN_VERSION} ${Platform} in appveyor."
                    cd ${PROJECT_DIR}
                    exit 0
                fi
            ;;
        esac
            
        if [ "${Platform}" = "x64" ]; then
            export BUILD_CROSS_HOST=x86_64-w64-mingw32
            TARGET=mingw64
        else
            export BUILD_CROSS_HOST=i686-w64-mingw32
            TARGET=mingw
        fi
        export BUILD_CROSS_SYSROOT=${MINGW_PATH}/${BUILD_CROSS_HOST}
        export PATH=${MINGW_PATH}/bin:$PATH
    
        MAKE="make -j`cat /proc/cpuinfo |grep 'cpu cores' |wc -l`"
    ;;
    android*)
        if [ "${APPVEYOR_BUILD_WORKER_IMAGE}" = "Visual Studio 2017" ]; then
            export ANDROID_NDK=/C/ProgramData/Microsoft/AndroidNDK64/android-ndk-r17
            HOST=windows-x86_64
        else
            export ANDROID_NDK=/C/ProgramData/Microsoft/AndroidNDK/android-ndk-r10e
            HOST=windows
        fi
           
        case ${BUILD_TARGERT} in
            android_arm)
                if [ "${Platform}" = "x64" ]; then
                    export BUILD_CROSS_HOST=aarch64-linux-android
                    export BUILD_CROSS_SYSROOT=${ANDROID_NDK}/platforms/android-${ANDROID_API}/arch-arm64
                    TARGET=android-arm64
                else
                    export BUILD_CROSS_HOST=arm-linux-androideabi
                    export BUILD_CROSS_SYSROOT=${ANDROID_NDK}/platforms/android-${ANDROID_API}/arch-arm
                    TARGET=android-armeabi                    
                fi
            ;;
            android_x86)
                if [ "${Platform}" = "x64" ]; then
                    export BUILD_CROSS_HOST=x86_64
                    export BUILD_CROSS_SYSROOT=${ANDROID_NDK}/platforms/android-${ANDROID_API}/arch-x86_64
                    TARGET=android-x86_64
                else
                    export BUILD_CROSS_HOST=x86
                    export BUILD_CROSS_SYSROOT=${ANDROID_NDK}/platforms/android-${ANDROID_API}/arch-x86
                    TARGET=android-x86
                fi
            ;;
        esac
        ANDROID_TOOLCHAIN_NAME=${BUILD_CROSS_HOST}-${TOOLCHAIN_VERSION}
        TOOLCHAIN_ROOT=${ANDROID_NDK}/toolchains/${ANDROID_TOOLCHAIN_NAME}/prebuilt/${HOST}
        export PATH=${TOOLCHAIN_ROOT}/bin:$PATH
        CONFIG_PARA="-D__ANDROID_API__=${ANDROID_API}"
        MAKE="make -j`cat /proc/cpuinfo |grep 'cpu cores' |wc -l`"
    ;;
esac

echo "PATH:$PATH"

perl ../Configure \
    --prefix=${OPENSSL_BUILD_PREFIX} \
    --openssldir=${OPENSSL_BUILD_PREFIX} \
    ${MODE} ${TARGET} ${CONFIG_PARA}
perl configdata.pm --dump
echo "${MAKE}" # build_all_generated"
${MAKE} #build_all_generated
echo "${MAKE} test"
if [ "${BUILD_TARGERT}" != "android" ]; then
    ${MAKE} test V=1
fi
echo "${MAKE} install"
${MAKE} install

cd ${PROJECT_DIR}
