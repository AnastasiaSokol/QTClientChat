#include "clientwindow.h"
#include "clientchat.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    connect(
                ui->pushButton_3,
                SIGNAL(clicked()),
                this,
                SLOT (connectToHost()));
    connect(
                ui->pushButton_2,
                SIGNAL(clicked()),
                this,
                SLOT (disconnectLocalClient()));



    ui->textEdit->setText("Welcome to chat!");
    ui->lineEdit_2->setText("134.19.130.129");
    ui->textEdit_3->setText("14000");

    //*serverAddress="134.19.130.129";
    //*serverPort=14000;

    newclient = new ClientChat("Nastya");

    connect(
                newclient,
                SIGNAL(accessIsAllowed()),
                this,
                SLOT (showMsgConnectedSuccesfuly()));


    connect(
                newclient,
                SIGNAL(userEntry(QString username)),
                this,
                SLOT (showUserInUserList(QString username)));




}

ClientWindow::~ClientWindow()
{
    delete ui;
}


void ClientWindow::connectToHost()
{
    newclient = new ClientChat(
                ui->textEdit_2->toPlainText());

    newclient->setServerAddressAndPort(
                ui->lineEdit_2->text(),
            ui->textEdit_3->toPlainText().toShort());

    //создает соединение и пытается отправить на сервер датаграмму CONNECT
    newclient->connectToHost();


    connect(
                newclient,
                SIGNAL(accessIsAllowed()),
                this,
                SLOT (showMsgConnectedSuccesfuly()));

    connect(
                newclient,
                SIGNAL(userEntry(QString)),
                this,
                SLOT (showUserInUserList(QString)));

}

void ClientWindow::disconnectLocalClient()
{

      qDebug() <<"Try to disconnect...";

}

void ClientWindow::userDisconnect(QString ipAddress)
{

      qDebug() <<"user "<< ipAddress <<" is disconnected";
      //удаляем пользователя из списка онлайн пользователей
      //TODO

}

void ClientWindow::showMsgConnectedSuccesfuly()
{
       QMessageBox msgBox;
      msgBox.setWindowTitle("Connection");
      msgBox.setText("Congratulations, you are connected");
      msgBox.exec();
}

void ClientWindow::showUserInUserList(QString username)
{
    qDebug()<<"showUserInUserList";
    //добавим пользователя в лист если его не было
    if (usersList.contains(username)==false){
        usersList<<username;
        QListWidgetItem *newItem = new QListWidgetItem(username);
        ui->listWidget->insertItem(ui->listWidget->count(), newItem);
    }



}


