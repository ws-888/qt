#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QTcpSocket>
#include <QMouseEvent>
#include "form.h"
#include "talk.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
signals :
    void sendmessage(struct login_st m);
    void sendlist(struct login_st m);

public slots:
    void oncheck(bool checked = false);
    void onloginPuhButtonSlot(bool clicked = false);
    void onmore(bool clicked = false);
    void forget(bool clicked = false);
    void myclose(bool checed = false);
    void socketReadyReadSlot();
    void getSecondWidgetSlot(struct login_st s);
    void sendWidgetSlot(struct login_st m);

private:
    Ui::Widget *ui;
    QPushButton *okBtn;
    QPushButton *okBtn1;
    QPushButton *okBtn2;
    QPushButton *okBtn3;
    QCheckBox logCheckBox;
    QLabel *imgLabel;
    QLineEdit *plineedit;
    QLineEdit *plineedit1;
    QTcpSocket *tcpSocket;//TCP套接字对象
    QPoint m_p;
    Form *f1;
    talk *t1;
    QString myacount;

};
#endif // WIDGET_H
