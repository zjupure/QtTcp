#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class QTextEdit;
class QLabel;
class QTcpServer;
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
    QTcpServer *serverSock;
    QTcpSocket *clientSock;

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void acceptConnection();
    void sendMessage();
    void receiveData();
    void disConnection();
};

#endif // DIALOG_H
