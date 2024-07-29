#include "form.h"
#include "ui_form.h"
#include <QDebug>
#include <QPalette>
#include <QMessageBox>
#include <QLabel>

Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);

    QPalette pal;
    pal.setBrush(QPalette::Window, QBrush(QPixmap(":/new/prefix1/myimg/3.jpg")));
    this->setPalette(pal);

    this->resize(300, 400);

    okBtn = new QPushButton(this);
    okBtn->setText(QString("X"));
    okBtn->setFlat(true);
    okBtn->move(280, 0);
    okBtn->setFixedSize(20, 20);

    this->setWindowFlags(Qt::FramelessWindowHint);

    QFont font;
    font.setPixelSize(20);

    okBtn3 = new QPushButton(this);
    okBtn3->setText(QString("注册"));
    okBtn3->setFlat(true);
    okBtn3->setFont(font);
    QPalette palButton3 = okBtn3->palette();
    palButton3.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    okBtn3->setPalette(palButton3);
    okBtn3->move(25, 300);
    okBtn3->setFixedSize(250, 50);
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
    plineedit->setGeometry(20, 100, 260, 40);
    plineedit->setPlaceholderText("输入QQ密码");
    plineedit->setFont(font);
    plineedit->setAlignment(Qt::AlignHCenter);
    plineedit->setEchoMode(QLineEdit::Password);
    plineedit->setMaxLength(15);
    plineedit->setStyleSheet("border-radius:10px;");

    plineedit1 = new QLineEdit(this);
    plineedit1->setGeometry(20, 40, 260, 40);
    plineedit1->setPlaceholderText("输入QQ号");
    plineedit1->setFont(font);
    plineedit1->setAlignment(Qt::AlignHCenter);
    plineedit1->setMaxLength(10);
    plineedit1->setStyleSheet("border-radius:10px;");

    plineedit2 = new QLineEdit(this);
    plineedit2->setGeometry(20, 160, 260, 40);
    plineedit2->setPlaceholderText("确认QQ密码");
    plineedit2->setFont(font);
    plineedit2->setAlignment(Qt::AlignHCenter);
    plineedit2->setEchoMode(QLineEdit::Password);
    plineedit2->setMaxLength(15);
    plineedit2->setStyleSheet("border-radius:10px;");

    logCheckBox.setText(QString("已阅读并同意服务协议和QQ隐私保护指引"));
    logCheckBox.setParent(this);
    logCheckBox.move(30, 230);

    QLabel *link1 = new QLabel(this);
    link1->setOpenExternalLinks(true);//设置为true才能打开网页
    link1->setText("<a style='color: blue; text-decoration: none' href = https://rule.tencent.com/rule/preview/46a15f24-e42c-4cb6-a308-2347139b1201>服务协议");
    link1->setAlignment(Qt::AlignCenter);
    link1->move(120, 233);

    QLabel *link2 = new QLabel(this);
    link2->setOpenExternalLinks(true);//设置为true才能打开网页
    link2->setText("<a style='color: blue; text-decoration: none' href = https://rule.tencent.com/rule/preview/46a15f24-e42c-4cb6-a308-2347139b1201>QQ隐私保护指引");
    link2->setAlignment(Qt::AlignCenter);
    link2->move(180, 233);

    connect(okBtn, SIGNAL(clicked(bool)), this, SLOT(myclose(bool)));
    connect(&logCheckBox, SIGNAL(clicked(bool)), this, SLOT(oncheck(bool)));
    connect(okBtn3, SIGNAL(clicked(bool)), this, SLOT(onloginPuhButtonSlot(bool)));
}

Form::~Form()
{
    delete ui;
}

void Form::onloginPuhButtonSlot(bool clicked)
{
    struct login_st s;
    QString cntStr = plineedit1->text();
    QString pwdStr = plineedit->text();
    strncpy(s.count, cntStr.toLatin1().data(), CNTSIZE);
    strncpy(s.passwd, pwdStr.toLatin1().data(), PWDSIZE);
    s.login_state = REGISTER_IN;
    if(plineedit1->text().isEmpty() || plineedit->text().isEmpty())
        QMessageBox::information(this, QString("注册"), QString("注册失败，账号或密码不能为空"), QMessageBox::Yes);
    else if(plineedit2->text() != plineedit->text())
        QMessageBox::information(this, QString("注册"), QString("注册失败，两次输入的密码不一样"), QMessageBox::Yes);
    else
        emit lineEditText(s);
}

void Form::myclose(bool checed)
{
    close();
}

void Form::oncheck(bool checked)
{
    if(logCheckBox.isChecked())
        okBtn3->setEnabled(true);
    else
        okBtn3->setEnabled(false);
}

void Form::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        m_p = event->globalPos() - this->geometry().topLeft();
}

void Form::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
        move(event->globalPos() - m_p);
}
