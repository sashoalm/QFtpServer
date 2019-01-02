/* Java util
 * Author: KangLin(kl222@!26.com) 
 */

#include "JavaUtil.h"
#include <QtAndroidExtras>
#include <QtAndroid>

CJavaUtil::CJavaUtil(QObject *parent) :  QObject(parent)
{
}

CJavaUtil::~CJavaUtil()
{
}

int CJavaUtil::InitPermissions()
{
    int nRet = 0;
    static bool inited = false;
    if(inited)
        return 0;
    inited = true;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    QStringList lstPermission;
    QtAndroid::PermissionResult r;
    r = QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE");
    if(QtAndroid::PermissionResult::Denied == r)
    {
        lstPermission << "android.permission.READ_EXTERNAL_STORAGE";
    }
    r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    if(QtAndroid::PermissionResult::Denied == r)
    {
        lstPermission << "android.permission.WRITE_EXTERNAL_STORAGE";
    }
    if(!lstPermission.isEmpty())
        QtAndroid::requestPermissionsSync(lstPermission);
#else
    /* Checks if the app has permission to read and write to device storage
     * If the app does not has permission then the user will be prompted to
     * grant permissions, When android > 6.0(SDK API > 23)
     */
    QAndroidJniObject mainActive = QtAndroid::androidActivity();
    if(mainActive.isValid())
        QAndroidJniObject::callStaticMethod<void>(
                "org/KangLinStudio/Utils",
                "verifyStoragePermissions",
                "(Ljava/lang/Object;)V",
                mainActive.object<jobject>());
    else {
        qDebug() << "QtAndroid::androidActivity() isn't valid\n";
    }
#endif
    
    return nRet;
}
