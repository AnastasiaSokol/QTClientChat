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
public:
    QString *serverAddress;//адресс сервера
    quint16 *serverPort;//порт сервера

};
#endif // MAINWINDOW_H
