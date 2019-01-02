/* Android Directory
 * Author: KangLin(kl222@!26.com) 
 */

#include "AndroidDirectory.h"
#include <QAndroidJniObject>
#include <QtAndroid>

CAndroidDirectory::CAndroidDirectory(QObject *parent) :  QObject(parent)
{
}

CAndroidDirectory::~CAndroidDirectory()
{}

QString CAndroidDirectory::GetExternalStorageDirectory()
{
    QAndroidJniObject d = QAndroidJniObject::callStaticObjectMethod(
                "android/os/Environment",
                "getExternalStorageDirectory",
                "()Ljava/io/File;"
                );
    return d.toString();
}

QString CAndroidDirectory::GetDataDirectory()
{
    QAndroidJniObject dataDir = QAndroidJniObject::callStaticObjectMethod(
                    "android/os/Environment",
                    "getDataDirectory",
                    "()Ljava/io/File;"
                    );
    return dataDir.toString();
}

QString CAndroidDirectory::GetExternalStoragePublicDirectory(QString name)
{
    QAndroidJniObject n = QAndroidJniObject::getStaticObjectField(
                "android/os/Environment",
                name.toStdString().c_str(),
                "Ljava/lang/String;"
                );
    
    QAndroidJniObject dir = QAndroidJniObject::callStaticObjectMethod(
                "android/os/Environment",
                "getExternalStoragePublicDirectory",
                "(Ljava/lang/String;)Ljava/io/File;",
                n.object<jstring>()
                );
    return dir.toString();
}

QString CAndroidDirectory::GetPictureDirectory()
{
    return GetExternalStoragePublicDirectory("DIRECTORY_DCIM");
}

QString CAndroidDirectory::GetMusicDirectory()
{
    return GetExternalStoragePublicDirectory("DIRECTORY_MUSIC");
}

QString CAndroidDirectory::GetMoviesDirectory()
{
    return GetExternalStoragePublicDirectory("DIRECTORY_MOVIES");
}

QString CAndroidDirectory::GetRingtonesDirectory()
{
    return GetExternalStoragePublicDirectory("DIRECTORY_RINGTONES");
}

QString CAndroidDirectory::GetAppFilesDirectory()
{
    return QtAndroid::androidActivity().callObjectMethod(
                "getFilesDir",
                "()Ljava/io/File;"
                ).toString();
}

QString CAndroidDirectory::GetAppCachedDirectory()
{
    return QtAndroid::androidActivity().callObjectMethod(
                "getCacheDir",
                "()Ljava/io/File;"
                ).toString();
}
