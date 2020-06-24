#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clientchat.h"
#include <QHash>
#include <QListWidgetItem>
#include <QList>
#include <QMessageBox>

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
public:
    QString *serverAddress;//адресс сервера
    quint16 *serverPort;//порт сервера
    //int onlineCounter;//количество участников

};
#endif // MAINWINDOW_H
