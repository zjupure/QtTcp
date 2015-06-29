#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtNetwork>

class QTextEdit;
class QLabel;
class QTcpSocket;

class Dialog : public QDialog
{
    Q_OBJECT
    QTextEdit *msgShow;
    QTextEdit *msgText;
    QLabel *status;
    QPushButton *sendBtn;
    QPushButton *quitBtn;

    //network
    QTcpSocket *clientSock;
    //
    QString ip;
    qint16 port;
    bool isConnect;
public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void acceptConnection();
    void disConnection();
    void sendMessage();
    void receiveData();
    void dispathError(QAbstractSocket::SocketError);
};

#endif // DIALOG_H
