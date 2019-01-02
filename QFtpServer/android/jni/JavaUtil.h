/* Java util
 * Author: KangLin(kl222@!26.com) 
 */

#ifndef JAVAUTIL_H
#define JAVAUTIL_H

#include <QObject>

class CJavaUtil : public QObject
{
    Q_OBJECT
    
public:
    CJavaUtil(QObject *parent = nullptr);
    virtual ~CJavaUtil();
    
    static int InitPermissions();
};

#endif // JAVAUTIL_H
