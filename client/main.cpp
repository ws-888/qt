#include "widget.h"//包含自己创建的头文件

#include <QApplication>//包含一个应用程序类的头文件

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);//实例化应用程序类对象[在Qt中有且仅有一个]
    Widget w;//实例化Widget类对象
    w.show();//调用show()方法显示
    return a.exec();//让应用程序对象进入消息循环
}
