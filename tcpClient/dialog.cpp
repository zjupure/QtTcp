#include "dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *qvBox = new QVBoxLayout(this);
    msgShow = new QTextEdit();
    msgText = new QTextEdit();
    QHBoxLayout *qhBox = new QHBoxLayout();
    status = new QLabel();
    sendBtn = new QPushButton(tr("发送"));
    quitBtn = new QPushButton(tr("关闭"));

    msgShow->setReadOnly(true);
    msgShow->setFixedHeight(400);
    msgShow->setFixedWidth(800);

    msgText->setFixedHeight(100);
    msgText->setFixedWidth(800);

    qhBox->addWidget(status);
    qhBox->addStretch(0);
    qhBox->addWidget(sendBtn);
    qhBox->addWidget(quitBtn);

    qvBox->addWidget(msgShow);
    qvBox->addWidget(msgText);
    qvBox->addLayout(qhBox);

    isConnect = false;
    //network
    //ip = QString("127.0.0.1");
    ip = QString("172.16.202.199");
    port = 1234;
    clientSock = new QTcpSocket(this);
    clientSock->connectToHost(QHostAddress(ip),port);

    status->setText(tr("正在连接服务器......"));
    //gui signal and slot
    connect(quitBtn,SIGNAL(clicked()),this,SLOT(close()));
    connect(sendBtn,SIGNAL(clicked()),this,SLOT(sendMessage()));

    //
    connect(clientSock,SIGNAL(connected()),this,SLOT(acceptConnection()));
    connect(clientSock,SIGNAL(disconnected()),this,SLOT(disConnection()));
    connect(clientSock,SIGNAL(readyRead()),this,SLOT(receiveData()));
    connect(clientSock,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(dispathError(QAbstractSocket::SocketError)));
}

Dialog::~Dialog()
{
    delete msgShow;
    delete msgText;
    delete sendBtn;
    delete quitBtn;
    delete clientSock;
}

void Dialog::acceptConnection()
{
    status->setText(tr("成功连接到服务器....."));
    isConnect = true;
}

void Dialog::disConnection()
{
    status->setText(tr("连接已经断开,正在尝试重连......"));
    isConnect = false;

    clientSock->connectToHost(QHostAddress(ip),port);
}

void Dialog::sendMessage()
{
    if(isConnect)
    {
        QString msg = msgText->toPlainText().trimmed();
        if(!msg.isEmpty())
        {
            clientSock->write(msg.toLocal8Bit());
            //
            QDateTime curTime = QDateTime::currentDateTime();
            QString showMsg = tr("客户端:    ") + curTime.toString("hh:mm:ss") + "\n    " + msg;
            msgShow->append(showMsg);
            msgText->setPlainText("");
        }
        else
        {
            QMessageBox::warning(NULL,NULL,tr("发送消息不能为空！"));
        }
    }
    else
    {
        QMessageBox::information(NULL,NULL,tr("客户端未连接！"));
    }
}

void Dialog::receiveData()
{
    qint64 len = 0;
    while((len = clientSock->bytesAvailable()) != 0)
    {
        QByteArray recData;
        recData = clientSock->readLine();
        QString msg = QString::fromLocal8Bit(recData);

        QDateTime curTime = QDateTime::currentDateTime();
        QString showMsg = tr("服务器:    ") + curTime.toString("hh:mm:ss") + "\n    " + msg;
        msgShow->append(showMsg);
    }
}

void Dialog::dispathError(QAbstractSocket::SocketError)
{
    static int cnt = 0;

    qDebug()<<clientSock->errorString();
    //QMessageBox::warning(NULL,NULL,tr("TCP连接出现错误"));
    clientSock->disconnectFromHost();

    status->setText(tr("连接服务器出错,正在尝试重连......"));
    isConnect = false;

    cnt++;
    if(cnt < 3)
    {
        clientSock->connectToHost(QHostAddress(ip),port);
    }
}
