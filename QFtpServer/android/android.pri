#qthelp://org.qt-project.qtdoc.5120/qtdoc/deployment-android.html
ANDROID_PACKAGE_SOURCE_DIR = $$PWD

include($$PWD/android_lib/android/android.pri)
OTHER_FILES += \
    $$PWD/AndroidManifest.xml \
    $$PWD/build.gradle \
    $$PWD/settings.gradle
