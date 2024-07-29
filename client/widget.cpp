#include "widget.h"
#include "ui_widget.h"
#include <QIcon>
#include <QPixmap>
#include <QPalette>
#include <QFont>
#include "protocol.h"
#include <string.h>
//#include <QCryptographicHash>
#include <QMessageBox>
#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/new/prefix1/myimg/5.png"));

    this->setGeometry(600, 100, 400, 600);//设置窗口大小及位置

    QPalette pal;
    pal.setBrush(QPalette::Window, QBrush(QPixmap(":/new/prefix1/myimg/3.jpg")));
    this->setPalette(pal);

    this->setWindowFlags(Qt::FramelessWindowHint);

    okBtn = new QPushButton(this);
    okBtn->setText(QString("X"));
    okBtn->setFlat(true);
    okBtn->move(380, 0);
    okBtn->setFixedSize(20, 20);

    logCheckBox.setText(QString("已阅读并同意服务协议和QQ隐私保护指引"));
    logCheckBox.setParent(this);
    logCheckBox.move(80, 360);

    okBtn1 = new QPushButton(this);
    okBtn1->setText(QString("忘记密码"));
    okBtn1->setFlat(true);

    QFont font;
    font.setPixelSize(20);

    okBtn1->setFont(font);
    QPalette palButton1 = okBtn1->palette();
    palButton1.setColor(QPalette::ButtonText, QColor(0, 100, 200));
    okBtn1->setPalette(palButton1);
    okBtn1->move(100, 500);

    okBtn2 = new QPushButton(this);
    okBtn2->setText(QString("注册账号"));
    okBtn2->setFlat(true);
    okBtn2->setFont(font);
    QPalette palButton2 = okBtn2->palette();
    palButton2.setColor(QPalette::ButtonText, QColor(0, 100, 200));
    okBtn2->setPalette(palButton2);
    okBtn2->move(220, 500);

    imgLabel = new QLabel(this);
    imgLabel->setPixmap(QPixmap(":/new/prefix1/myimg/5.png"));
    imgLabel->move(120,60);

    okBtn3 = new QPushButton(this);
    okBtn3->setText(QString("登录"));
    okBtn3->setFlat(true);
    okBtn3->setFont(font);
    QPalette palButton3 = okBtn3->palette();
    palButton3.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    okBtn3->setPalette(palButton3);
    okBtn3->move(19, 420);
    okBtn3->setFixedSize(360, 66);
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
    okBtn3->setEnabled(false);

    plineedit = new QLineEdit(this);
    plineedit->setGeometry(25, 290, 350, 50);
    plineedit->setPlaceholderText("输入QQ密码");
    plineedit->setFont(font);
    plineedit->setAlignment(Qt::AlignHCenter);
    plineedit->setEchoMode(QLineEdit::Password);
    plineedit->setMaxLength(15);
    plineedit->setStyleSheet("border-radius:10px;");

    plineedit1 = new QLineEdit(this);
    plineedit1->setGeometry(25, 230, 350, 50);
    plineedit1->setPlaceholderText("输入QQ号");
    plineedit1->setFont(font);
    plineedit1->setAlignment(Qt::AlignHCenter);
    plineedit1->setMaxLength(10);
    plineedit1->setStyleSheet("border-radius:10px;");

    tcpSocket = new QTcpSocket(this);

    connect(okBtn, SIGNAL(clicked(bool)), this, SLOT(myclose(bool)));
    connect(okBtn2, SIGNAL(clicked(bool)), this, SLOT(onmore(bool)));
    connect(okBtn1, SIGNAL(clicked(bool)), this, SLOT(forget(bool)));
    connect(&logCheckBox, SIGNAL(clicked(bool)), this, SLOT(oncheck(bool)));
    connect(okBtn3, SIGNAL(clicked(bool)), this, SLOT(onloginPuhButtonSlot(bool)));
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Widget::socketReadyReadSlot);

    QLabel *link1 = new QLabel(this);
    link1->setOpenExternalLinks(true);//设置为true才能打开网页
    link1->setText("<a style='color: blue; text-decoration: none' href = https://rule.tencent.com/rule/preview/46a15f24-e42c-4cb6-a308-2347139b1201>服务协议");
    link1->setAlignment(Qt::AlignCenter);
    link1->move(170, 362);

    QLabel *link2 = new QLabel(this);
    link2->setOpenExternalLinks(true);//设置为true才能打开网页
    link2->setText("<a style='color: blue; text-decoration: none' href = https://rule.tencent.com/rule/3fd52bde-6555-453b-9ab8-c5f1f3d22c62>QQ隐私保护指引");
    link2->setAlignment(Qt::AlignCenter);
    link2->move(230, 362);


}

Widget::~Widget()
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        m_p = event->globalPos() - this->geometry().topLeft();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
        move(event->globalPos() - m_p);
}

void Widget::oncheck(bool checked)
{
    if(logCheckBox.isChecked())
        okBtn3->setEnabled(true);
    else
        okBtn3->setEnabled(false);
}

void Widget::onloginPuhButtonSlot(bool clicked)
{
    struct login_st sndbuf;
    QString cntStr = plineedit1->text();
    QString pwdStr = plineedit->text();

    if(plineedit1->text().isEmpty() || plineedit->text().isEmpty())
        QMessageBox::information(this, QString("登录"), QString("账号或密码不能为空"), QMessageBox::Yes);
    //发送的数据包
    else
    {
        strncpy(sndbuf.count, cntStr.toLatin1().data(), CNTSIZE);

        //QByteArray pwdArray = QCryptographicHash::hash(pwdStr.toLatin1(), QCryptographicHash::Sha512);
        strncpy(sndbuf.passwd, pwdStr.toLatin1().data(), PWDSIZE);

        tcpSocket->connectToHost(QString(SERVER_IP), SERVER_PORT);
        tcpSocket->write((const char *)&sndbuf, sizeof(sndbuf));
    }
}

void Widget::onmore(bool clicked)
{
    f1 = new Form;
    f1->show();
    connect(f1, SIGNAL(lineEditText(struct login_st)), this, SLOT(getSecondWidgetSlot(struct login_st)));
    //connect(f1, SIGNAL(login(struct login_st)), this, SLOT(getSecondWidgetSlot(struct login_st)));
}

void Widget::forget(bool clicked)
{
    QDesktopServices::openUrl(QUrl("https://accounts.qq.com/find/password?from=11"));
}

void Widget::myclose(bool checed)
{
    close();
}

void Widget::socketReadyReadSlot()
{
    struct login_st buf;//存储接收到的数据
    tcpSocket->read((char *)&buf, sizeof(buf));//接收数据
    switch(buf.login_state)
    {
        case LOGIN_SUCCESS :
        QMessageBox::information(this, QString("登录"), QString("登陆成功"), QMessageBox::Yes);
        t1 = new talk;
        t1->show();
        myacount = buf.count;
        connect(t1, SIGNAL(sendto(struct login_st)), this, SLOT(sendWidgetSlot(struct login_st)));
        connect(this, SIGNAL(sendmessage(struct login_st)), t1, SLOT(receiveWidgetSlot(struct login_st)));
        connect(this, SIGNAL(sendlist(struct login_st)), t1, SLOT(receivelist(struct login_st)));
        this->close();
        break;
        case LOGIN_COUT_NOT_EXIST :
        QMessageBox::information(this, QString("登录"), QString("账号不存在"), QMessageBox::Yes);
        plineedit1->clear();
        plineedit->setFocus();
        break;
        case LOGIN_ERROR_PASSWD :
        QMessageBox::information(this, QString("登录"), QString("密码错误"), QMessageBox::Yes);
        plineedit->clear();
        plineedit->setFocus();
        break;
        case LOGIN_DOUBLE :
        QMessageBox::information(this, QString("登录"), QString("不能重复登录"), QMessageBox::Yes);
        break;
        case REGISTER_IN :
        if(QMessageBox::question(f1, QString("注册"), QString("注册成功，是否直接登录？"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        {
            buf.login_state = -1;
            getSecondWidgetSlot(buf);
            f1->close();
        }
        else
            f1->close();
        break;
        case REGISTER_ERROR :
        QMessageBox::information(f1, QString("注册"), QString("注册失败，账号已存在"), QMessageBox::Yes);
        case MESSAGE :
        emit sendmessage(buf);
        case LIST :
        emit sendlist(buf);
        break;
    }
}

void Widget::getSecondWidgetSlot(struct login_st s)
{
    tcpSocket->connectToHost(QString(SERVER_IP), SERVER_PORT);
    tcpSocket->write((const char *)&s, sizeof(s));
}

void Widget::sendWidgetSlot(struct login_st m)
{
    m.login_state = MESSAGE;
    strncpy(m.count, myacount.toLatin1().data(), CNTSIZE);
    //tcpSocket->connectToHost(QString(SERVER_IP), SERVER_PORT);
    tcpSocket->write((const char *)&m, sizeof(m));
}


