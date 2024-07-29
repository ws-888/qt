#include "talk.h"
#include "ui_talk.h"
#include <stdio.h>
#include <QDebug>

talk::talk(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::talk)
{
    ui->setupUi(this);

    QPalette pal;
    pal.setBrush(QPalette::Window, QBrush(QPixmap(":/new/prefix1/myimg/3.jpg")));
    this->setPalette(pal);

    okBtn = new QPushButton(this);
    okBtn->setText(QString("X"));
    okBtn->setFlat(true);
    okBtn->move(480, 0);
    okBtn->setFixedSize(20, 20);

    this->setWindowFlags(Qt::FramelessWindowHint);

    this->resize(500, 400);

    QFont font;
    font.setPixelSize(15);

    okBtn3 = new QPushButton(this);
    okBtn3->setText(QString("发送"));
    okBtn3->setFlat(true);
    okBtn3->setFont(font);
    okBtn3->move(370, 325);
    okBtn3->setFixedSize(80, 40);
    okBtn3->setStyleSheet("QPushButton {"
                          "  border: 2px solid #8f8f91;"
                          "  border-radius: 20px;"
                          "  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #00ffff, stop: 1 #bbbaba);"
                          "}"
                          "QPushButton:hover {"
                          "  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #00f0f0, stop: 1 #dadffc);"
                          "}"
                          "QPushButton:pressed {"
                          "  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadffc, stop: 1 #f6f7fa);"
                          "}");

    text1 = new QTextEdit(this);
    text2 = new QTextEdit(this);
    text1->setGeometry(45, 70, 410, 200);
    text2->setGeometry(45, 270, 410, 100);
    text1->setFont(font);
    text2->setFont(font);

    c1 = new QComboBox(this);
    c1->setGeometry(45, 25, 200, 40);
    c1->addItem("Option 1");
    c1->setStyleSheet("QComboBox {"
                      "  background-color: rgb(237, 242, 255);"
                      "  border-radius: 15px;"
                      "  background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #00ffff, stop: 1 #bbbaba);"
                      "}"
                      );
    c1->setFont(font);

    okBtn3->raise();

    connect(okBtn, SIGNAL(clicked(bool)), this, SLOT(myclose(bool)));
    connect(okBtn3, SIGNAL(clicked(bool)), this, SLOT(mysend(bool)));
}

talk::~talk()
{
    delete ui;
}

void talk::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        m_p = event->globalPos() - this->geometry().topLeft();
}

void talk::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
        move(event->globalPos() - m_p);
}

void talk::myclose(bool checed)
{
    close();
}

void talk::mysend(bool checked)
{
    struct login_st m;
    QString message = text2->toPlainText();
    QString tocount = c1->currentText();
    strncpy(m.message, message.toLatin1().data(), CNTSIZE);
    strncpy(m.tocount, tocount.toLatin1().data(), CNTSIZE);

    emit sendto(m);
    text1->insertPlainText("                                                            自己：");
    text1->insertPlainText("\n");
    text1->insertPlainText("                                                            ");
    text1->insertPlainText(text2->toPlainText());
    text1->insertPlainText("\n");
    text2->clear();
}

void talk::receiveWidgetSlot(struct login_st m)
{
    //qDebug() << "message" << m.message;
    text1->insertPlainText("来自");
    text1->insertPlainText(m.count);
    text1->insertPlainText("：");
    text1->insertPlainText("\n");
    text1->insertPlainText(m.message);
    text1->insertPlainText("\n");
}

void talk::receivelist(struct login_st m)
{
    //qDebug() << "list" << m.message;
    int i = 0;
    char *point = m.message;
    char *p = NULL;
    char name[10];
    if(m.login_state == LIST)
    {
        c1->clear();
        while (1)
        {
            p = strtok(point," ");
            if(p == NULL)
                break;
            qDebug() << "list1" << p;
            memset(name, 0, 10);
            strcpy(name,p);
            c1->addItem(name);
            point = NULL;
        }
    }
}
