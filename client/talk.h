#ifndef TALK_H
#define TALK_H

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QTextEdit>
#include <QComboBox>
#include "protocol.h"

namespace Ui {
class talk;
}

class talk : public QWidget
{
    Q_OBJECT

public:
    explicit talk(QWidget *parent = nullptr);
    ~talk();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:
    void sendto(struct login_st m);

public slots :
    void myclose(bool checked = false);
    void mysend(bool checked = false);
    void receiveWidgetSlot(struct login_st);
    void receivelist(struct login_st);


private:
    Ui::talk *ui;
    QPushButton *okBtn;
    QPushButton *okBtn3;
    QPoint m_p;
    QTextEdit *text1;
    QTextEdit *text2;
    QComboBox *c1;
};

#endif // TALK_H
