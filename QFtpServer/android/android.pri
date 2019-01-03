#qthelp://org.qt-project.qtdoc.5120/qtdoc/deployment-android.html
ANDROID_PACKAGE_SOURCE_DIR = $$PWD  #ANDROID包的源码目录  

#include($$PWD/android_lib/android/jni/jni.pri)
include($$PWD/android_lib/android/android.pri)
OTHER_FILES += \
    $$PWD/AndroidManifest.xml \
    $$PWD/build.gradle
