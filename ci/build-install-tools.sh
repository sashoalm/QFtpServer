#!/bin/bash 

set -e
SOURCE_DIR="`pwd`"
echo $SOURCE_DIR
TOOLS_DIR=${SOURCE_DIR}/Tools

function function_install_yasm()
{
    #Install yasm
    mkdir -p ${TOOLS_DIR}/src
    cd ${TOOLS_DIR}/src
    wget -c -nv http://www.tortall.net/projects/yasm/releases/yasm-1.3.0.tar.gz 
    tar xzf yasm-1.3.0.tar.gz
    cd yasm-1.3.0/
    ./configure > /dev/null && sudo make install -j2 > /dev/null
    cd ${SOURCE_DIR}
}

function function_common()
{
    cd ${TOOLS_DIR}
    #Install new cmake
    #if [ "cmake" = "${QMAKE}" ]; then
    #    if [ ! -d "`pwd`/cmake" ]; then
    #        wget -nv --no-check-certificate http://www.cmake.org/files/v3.6/cmake-3.6.1-Linux-x86_64.tar.gz
    #        tar xzf cmake-3.6.1-Linux-x86_64.tar.gz
    #        mv cmake-3.6.1-Linux-x86_64 cmake
    #    fi
    #fi
    
    # Install qt：https://github.com/benlau/qtci  
    if [ "$DOWNLOAD_QT" = "TRUE" ]; then
        QT_DIR=`pwd`/Qt/${QT_VERSION}
        if [ ! -d "${QT_DIR}" ]; then
            if [ "${QT_VERSION}" = "5.6.3" ]; then
                wget -c --no-check-certificate -nv http://download.qt.io/official_releases/qt/${QT_VERSION_DIR}/${QT_VERSION}/qt-opensource-linux-x64-android-${QT_VERSION}.run
                bash ${SOURCE_DIR}/ci/qt-installer.sh qt-opensource-linux-x64-android-${QT_VERSION}.run ${QT_DIR}
                rm qt-opensource-linux-x64-android-${QT_VERSION}.run
            else
                wget -c --no-check-certificate -nv http://download.qt.io/official_releases/qt/${QT_VERSION_DIR}/${QT_VERSION}/qt-opensource-linux-x64-${QT_VERSION}.run
                bash ${SOURCE_DIR}/ci/qt-installer.sh qt-opensource-linux-x64-${QT_VERSION}.run ${QT_DIR}
                rm qt-opensource-linux-x64-${QT_VERSION}.run
            fi
        fi
    fi
}

function install_android()
{
    cd ${TOOLS_DIR}
    if [ ! -d "`pwd`/android-sdk" ]; then
        ANDROID_STUDIO_VERSION=191.5900203
        wget -c -nv https://dl.google.com/dl/android/studio/ide-zips/3.5.1.0/android-studio-ide-${ANDROID_STUDIO_VERSION}-linux.tar.gz
        tar xzf android-studio-ide-${ANDROID_STUDIO_VERSION}-linux.tar.gz
        export JAVA_HOME=`pwd`/android-studio/jre
        export PATH=${JAVA_HOME}/bin:$PATH
        wget -c -nv https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip
        mkdir android-sdk
        cd android-sdk
        cp ../sdk-tools-linux-4333796.zip .
        unzip -q sdk-tools-linux-4333796.zip
        echo "Install sdk and ndk ......"
        if [ -n "${ANDROID_API}" ]; then
            PLATFORMS="platforms;${ANDROID_API}"
        else
            PLATFORMS="platforms"
        fi
        if [ -z "${BUILD_TOOS_VERSION}" ]; then
            BUILD_TOOS_VERSION="28.0.3"
        fi
        (sleep 5 ; num=0 ; while [ $num -le 5 ] ; do sleep 1 ; num=$(($num+1)) ; printf 'y\r\n' ; done ) \
        | ./tools/bin/sdkmanager "platform-tools" "build-tools;${BUILD_TOOS_VERSION}" "${PLATFORMS}" "ndk-bundle"
        if [ ! -d ${TOOLS_DIR}/android-ndk ]; then
            ln -s ${TOOLS_DIR}/android-sdk/ndk-bundle ${TOOLS_DIR}/android-ndk
        fi
    fi
}

function install_android_sdk_and_ndk()
{
    cd ${SOURCE_DIR}/Tools
    
    #Install android ndk  
    if [ ! -d "`pwd`/android-ndk" ]; then
        if [ "$QT_VERSION_DIR" = "5.8" ]; then
            wget -c -nv http://dl.google.com/android/ndk/android-ndk-r10e-linux-x86_64.bin
            chmod u+x android-ndk-r10e-linux-x86_64.bin
            ./android-ndk-r10e-linux-x86_64.bin > /dev/null
            mv android-ndk-r10e android-ndk
            rm android-ndk-r10e-linux-x86_64.bin
        else
            NDK_VERSION=r20
            wget -c -nv https://dl.google.com/android/repository/android-ndk-${NDK_VERSION}-linux-x86_64.zip
            unzip android-ndk-${NDK_VERSION}-linux-x86_64.zip
            mv android-ndk-${NDK_VERSION} android-ndk
            rm android-ndk-${NDK_VERSION}-linux-x86_64.zip
        fi
    fi

    cd ${TOOLS_DIR}

    #Download android sdk  
    if [ ! -d "`pwd`/android-sdk" ]; then
        SDK_VERSION=r24.4.1
        wget -c -nv https://dl.google.com/android/android-sdk_${SDK_VERSION}-linux.tgz
        tar xf android-sdk_${SDK_VERSION}-linux.tgz 
        mv android-sdk-linux android-sdk
        rm android-sdk_${SDK_VERSION}-linux.tgz 
        (sleep 5 ; while true ; do sleep 1 ; printf 'y\r\n' ; done ) \
        | android-sdk/tools/android update sdk -u -t tool,${ANDROID_API},extra,platform,platform-tools,build-tools-28.0.3,build-tools-28.0.2
    fi
}

function function_android()
{
    cd ${TOOLS_DIR}
    
    sudo apt-get update -y -qq
    #sudo apt-get install -qq -y openjdk-11-jdk
    # install oracle jdk
    #sudo add-apt-repository ppa:linuxuprising/java -y
    #sudo apt update
    #(sleep 5 ; while true ; do sleep 1 ; printf '\r\n' ; done ) | sudo apt install oracle-java11-installer -qq -y
    
    #sudo apt install oracle-java11-set-default -qq -y

    install_android
    
    sudo apt-get install ant -qq -y
    sudo apt-get install libicu-dev -qq -y
    sudo apt-get install -y -qq libglu1-mesa-dev \
        libxkbcommon-x11-dev   
        
    function_common
    cd ${SOURCE_DIR}
}

function function_unix()
{
    #Install yasm
    #function_install_yasm

    if [ "$DOWNLOAD_QT" != "TRUE"  ]; then
        #See: https://launchpad.net/~beineri
        sudo add-apt-repository ppa:beineri/opt-qt-${QT_VERSION}-`lsb_release -c|awk '{print $2}'` -y
    fi

    sudo apt-get update -y -qq
    sudo apt-get install debhelper fakeroot -y -qq
    sudo apt-get install -y -qq libglu1-mesa-dev \
        libxkbcommon-x11-dev        

    if [ "$DOWNLOAD_QT" != "TRUE" ]; then
        sudo apt-get install -y -qq qt${QT_VERSION_DIR}base \
            qt${QT_VERSION_DIR}tools 
    fi
    function_common

    cd ${SOURCE_DIR}
}

function function_mingw()
{
    #Install yasm
    #function_install_yasm

    cd ${SOURCE_DIR}
    if [ "true" == "$RABBITIM_BUILD_THIRDLIBRARY" ]; then
        export RABBITIM_BUILD_CROSS_HOST=i686-w64-mingw32 #i586-mingw32msvc
    fi

    function_common
    cd ${SOURCE_DIR}
}

case ${BUILD_TARGERT} in
    android)
        function_android
        ;;
    unix)
        function_unix
        ;;
    windows_mingw)
        function_mingw
        ;;
    *)
    echo "There aren't ${BUILD_TARGERT}"
        ;;
esac

cd ${SOURCE_DIR}
