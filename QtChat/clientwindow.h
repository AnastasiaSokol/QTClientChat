#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clientchat.h"
#include <QHash>
#include <QListWidgetItem>
#include <QList>
#include <QMessageBox>
#include "emiplib/mipaudiosession.h"
#include "emiplib/mippainputoutput.h"
#include "emiplib/mipconfig.h"
#include "jrtplib3/rtpipv4address.h"
#include <iostream>
#include <jrtplib3/rtpipv4address.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

private:
    Ui::MainWindow *ui;
    ClientChat *newclient; //текущий клиент запустивший приложение
    //QHash<QString, QString> usersList; //список учатников чата
    QStringList usersList;//список учаcтников чата
    QMap<QString, QHostAddress> userCalList;//список пользователей разрешивших звонок сначала username потом ipAddress
    MIPAudioSessionParams audioSessParams;
    MIPAudioSession audioSess;

public slots:
    void connectToHost();
    void disconnectLocalClient();
    void userDisconnect(QString userName);
    void showMsgConnectedSuccesfuly();//выводит сообщени что клиент подключился к серверу
    void showUserInUserList(QString username);//отображает пользователя в колонке слева
    void removeUserFromUserList(QString username);//убирает пользователя из колонки слева
    void paintMsg(QString msg,Qt::Alignment a);//отрисовка сообщения от локального клиета
    void paintPrivateMsgFromRecipient(QString msg,QString recipient);//отрисовка сообщения пришедшего от клиента в приватный чат
    void paintMsgFromRecipientToSharedChat(QString msg,QString recipient);//отрисовка сообщения пришедшего от клиента
    void sendMsg();//отправка сообщения
    void callToHost();//отправка сообщения
    void callToUser();//отправка сообщения пользователю что мы хотим связаться с ним
    void outputSounds();//вывод звука из сокета
    void callPtP(QHostAddress userAddress, quint16 audioPort);
    void stopCall();

    void showMsgCallAccept(QString username, QString idAddress);//вывод сообщения что запрос на звонок разрешен
    void showMsgCallReject(QString username);//вывод сообщения что звонок отклонен пользователем
    void showMsgRequestForCalling(QString username);//запрос от пользователя на входящий звонок
public:
    QString *serverAddress;//адресс сервера
    quint16 *serverPort;//порт сервера для tcp
    quint16 *serverPortUDP;//порт сервера для tcp
    //int onlineCounter;//количество участников

};
#endif // MAINWINDOW_H
