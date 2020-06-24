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
    connect(
                ui->pushButton,
                SIGNAL(clicked()),
                this,
                SLOT (sendMsg()));



    //ui->textEdit->setText("Welcome to chat!\n");
    ui->lineEdit_2->setText("134.19.130.129");
    ui->textEdit_3->setText("14000");

    //добавим пользователя в лист если его не было
    if (usersList.contains("sharedChat")==false){
        usersList<<"sharedChat";
        QListWidgetItem *newItem = new QListWidgetItem("sharedChat");
        ui->listWidget->insertItem(ui->listWidget->count(), newItem);
    }

    //*serverAddress="134.19.130.129";
    //*serverPort=14000;


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

    //выводит сообщение если мы подключились к серверу и сервер нас опознал
    connect(
                newclient,
                SIGNAL(accessIsAllowed()),
                this,
                SLOT (showMsgConnectedSuccesfuly()));
    //отобразим участника если он уже был подключен
    connect(
                newclient,
                SIGNAL(userEntry(QString)),
                this,
                SLOT (showUserInUserList(QString)));
    //отобразим участника если он подключился и новый
    connect(
                newclient,
                SIGNAL(userJoined(QString)),
                this,
                SLOT (showUserInUserList(QString)));
    //уберем пользователя из списка если он отключился
    connect(
                newclient,
                SIGNAL(userDisconnect(QString)),
                this,
                SLOT (removeUserFromUserList(QString)));
    //отрисуем сообщение, если пришло приватное сообщение
    connect(
                    newclient,
                    SIGNAL(recipientSendPrivateMsg(QString,QString)),
                    this,
                    SLOT (paintPrivateMsgFromRecipient(QString,QString)));
   //отрисуем сообщение, если пришло сообщение в общий чат
    connect(
                newclient,
                SIGNAL(recipientSendMsg(QString,QString)),
                this,
                SLOT (paintMsgFromRecipientToSharedChat(QString,QString)));

}

void ClientWindow::disconnectLocalClient()
{

      qDebug() <<"Try to disconnect...";
      newclient->disconnect();

      QMessageBox msgBox;
     msgBox.setWindowTitle("U are disconnected!");
     msgBox.setText("Congratulations, you are disconnected");
     msgBox.exec();

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
    //самих себя не будем отображать
    if (usersList.contains(username)==false&&username!=""&&username!=newclient->nikname){
        usersList<<username;
        QListWidgetItem *newItem = new QListWidgetItem(username);
        ui->listWidget->insertItem(ui->listWidget->count(), newItem);
    }




}

void ClientWindow::removeUserFromUserList(QString username)
{
    qDebug()<<"removeUserFromUserList";
    //добавим пользователя в лист если его не было
    if (usersList.contains(username)==true&&username!=""){
        usersList.removeOne(username);

       QList<QListWidgetItem *> items =  ui->listWidget->findItems(username,  Qt::MatchExactly);
       if (items.size() > 0)
           for (int i=0;i<items.size();i++){
            delete ui->listWidget->takeItem(ui->listWidget->row(items[i]) );
           }
    }
}

void ClientWindow::paintMsg(QString msg, Qt::Alignment a)
{
    if (msg.isEmpty())
        return;
    QTextCursor cursor(ui->textEdit->textCursor());
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextColor(Qt::blue);
    ui->textEdit->setAlignment(a);
    ui->textEdit->insertPlainText(msg);
    ui->textEdit->insertPlainText("\n");
}

void ClientWindow::paintMsgFromRecipientToSharedChat(QString msg, QString recipient)
{
    if (recipient!=newclient->nikname){
        paintMsg(recipient+":"+msg, Qt::AlignLeft);
    }
}

void ClientWindow::paintPrivateMsgFromRecipient(QString msg, QString recipient)
{
    //TO DO CHECK IN RECIPIENT IS NOT LOCAL CLIENT
    if (recipient!=newclient->nikname){
        paintMsg(recipient+"/private:"+msg,Qt::AlignLeft );
    }
}

void ClientWindow::sendMsg()
{
    QString msg = ui->lineEdit->text();
    if (ui->listWidget->item(0)->isSelected()){
        newclient->sendMessage(msg);
    }else{
           QString recipient = ui->listWidget->currentItem()->text();
            newclient->sendPrivateMessage(msg, recipient);
    }
    paintMsg(msg, Qt::AlignRight);
    ui->lineEdit->setText("");

}




