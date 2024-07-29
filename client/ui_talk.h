/********************************************************************************
** Form generated from reading UI file 'talk.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TALK_H
#define UI_TALK_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_talk
{
public:

    void setupUi(QWidget *talk)
    {
        if (talk->objectName().isEmpty())
            talk->setObjectName("talk");
        talk->resize(678, 480);

        retranslateUi(talk);

        QMetaObject::connectSlotsByName(talk);
    } // setupUi

    void retranslateUi(QWidget *talk)
    {
        talk->setWindowTitle(QCoreApplication::translate("talk", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class talk: public Ui_talk {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TALK_H
