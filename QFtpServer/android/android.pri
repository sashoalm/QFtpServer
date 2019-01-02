ANDROID_PACKAGE_SOURCE_DIR += $$PWD  #ANDROID包的源码目录  

include(jni/jni.pri)

OTHER_FILES += \
    $$PWD/AndroidManifest.xml \
    $$PWD/build.gradle

DISTFILES += \
    $$PWD/src/org/KangLinStudio/*
