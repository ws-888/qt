#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QMouseEvent>
#include "protocol.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:
    void lineEditText(struct login_st s);
    //void login(struct login_st);

public slots :
    void onloginPuhButtonSlot(bool clicked = false);
    void myclose(bool checed = false);
    void oncheck(bool checked = false);

private:
    Ui::Form *ui;
    QPushButton *okBtn;
    QPushButton *okBtn3;
    QLineEdit *plineedit;
    QLineEdit *plineedit1;
    QLineEdit *plineedit2;
    QCheckBox logCheckBox;
    QPoint m_p;

};

#endif // FORM_H
