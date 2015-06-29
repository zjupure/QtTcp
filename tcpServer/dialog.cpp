#include "dialog.h"
#include <QtNetwork>
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

    //network
    serverSock = new QTcpServer(this);
    clientSock = NULL;
    if(!serverSock->listen(QHostAddress::Any,1234));
    {
        qDebug()<<serverSock->errorString();
        close();
    }

    status->setText(tr("服务器正在监听......"));

    //gui signal and slot
    connect(quitBtn,SIGNAL(clicked()),this,SLOT(close()));
    connect(sendBtn,SIGNAL(clicked()),this,SLOT(sendMessage()));

    //network signal and slot
    connect(serverSock,SIGNAL(newConnection()),this,SLOT(acceptConnection()));

}

Dialog::~Dialog()
{
    delete msgShow;
    delete msgText;
    delete sendBtn;
    delete quitBtn;
    delete serverSock;
}

void Dialog::acceptConnection()
{
    //get the client socket
    clientSock = serverSock->nextPendingConnection();

    QHostAddress clientAddr = clientSock->localAddress();
    qint16 clientPort = clientSock->localPort();
    QString msg = tr("来自") + clientAddr.toString() + tr("的客户端连接到服务器......");
    status->setText(msg);
    //client socket signal ans slots
    connect(clientSock,SIGNAL(disconnected()),this,SLOT(disConnection()));
    connect(clientSock,SIGNAL(readyRead()),this,SLOT(receiveData()));
}

void Dialog::disConnection()
{
    clientSock->deleteLater();
    clientSock = NULL;
    //
    status->setText(tr("连接已经断开,等待客户端重连......"));
}

void Dialog::sendMessage()
{
    if(clientSock != NULL)
    {
        QString msg = msgText->toPlainText().trimmed();
        if(!msg.isEmpty())
        {
            clientSock->write(msg.toLocal8Bit());
            //
            QDateTime curTime = QDateTime::currentDateTime();
            QString showMsg = tr("服务器:    ") + curTime.toString("hh:mm:ss") + "\n    " + msg;
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

        //
        QDateTime curTime = QDateTime::currentDateTime();
        QString showMsg = tr("客户端:    ") + curTime.toString("hh:mm:ss") + "\n    " + msg;
        msgShow->append(showMsg);
    }
}
