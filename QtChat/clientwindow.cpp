#include "clientwindow.h"
#include "clientchat.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);


    //output sound from socket
    connect(
                ui->checkBox,
                SIGNAL(clicked()),
                this,
                SLOT (outputSounds()));


    connect(
                ui->pushButton_6,
                SIGNAL(clicked()),
                this,
                SLOT (stopCall()));
    //click call to server
    connect(
                ui->pushButton_4,
                SIGNAL(clicked()),
                this,
                SLOT (callToHost()));


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


        //request call to user
        connect(
                    ui->pushButton_5,
                    SIGNAL(clicked()),
                    this,
                    SLOT (callToUser()));

    //ui->textEdit->setText("Welcome to chat!\n");
    ui->lineEdit_2->setText("134.19.130.129");
    ui->textEdit_3->setText("14000");//TCP PORT
    ui->textEdit_6->setText("14002");//UDP PORT
    //visible
      ui->textEdit_5->setVisible(false);
      ui->textEdit_4->setVisible(false);
      ui->label_4->setVisible(false);
       ui->label_5->setVisible(false);
       ui->pushButton_4->setVisible(false);
       ui->checkBox->setVisible(false);

    //добавим пользователя в лист если его не было
    if (usersList.contains("sharedChat")==false){
        usersList<<"sharedChat";
        QListWidgetItem *newItem = new QListWidgetItem("sharedChat");
        ui->listWidget->insertItem(ui->listWidget->count(), newItem);
    }

    //*serverAddress="134.19.130.129";
    //*serverPort=14000;

    //тестим
    /*
    userCalList.insert("sokel1", QHostAddress("192.168.3.1"));
     userCalList.insert("sokel2", QHostAddress("192.168.3.2"));
      userCalList.insert("sokel3", QHostAddress("192.168.3.3"));
       QHostAddress userAddress;
    foreach (QString key, userCalList.keys()){
        if (key=="sokel3"){
            userAddress = userCalList.value(key);
            qDebug()<<"userAddress"<< userCalList.find("sokel3").value();
            break;
        }
    }*/


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


    newclient->setServerUDPPort(ui->textEdit_6->toPlainText().toShort());

    //создает соединение и пытается отправить на сервер датаграмму CONNECT
    newclient->connectToHostByTCP();
    newclient->connectToHostByUDP();

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
    //запрос на звонок одобрен QString username, QString idAddress
    connect(
                newclient,
                SIGNAL(callAccept(QString,QString)),
                this,
                SLOT (showMsgCallAccept(QString,QString)));
    //запрос на звонок отклонен QString username
    connect(
                newclient,
                SIGNAL(callReject(QString)),
                this,
                SLOT (showMsgCallReject(QString)));
    //входящий звонок
    connect(
                newclient,
                SIGNAL(eVMpCall(QString)),
                this,
                SLOT (showMsgRequestForCalling(QString)));



   ui->textEdit_4->setText(newclient->getSupportedInputDevice());
   ui->textEdit_5->setText(newclient->getSupportedOutputDevice());

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
    QString recipient = ui->listWidget->currentItem()->text();

    QString msg = ui->lineEdit->text();
    if (ui->listWidget->item(0)->isSelected()){
        newclient->sendMessage(msg);
        paintMsg(msg, Qt::AlignRight);
    }else{

          newclient->sendPrivateMessage(msg, recipient);
          paintMsg("@"+recipient+" send private:"+msg, Qt::AlignRight);
    }

    ui->lineEdit->setText("");

}

void ClientWindow::callToHost()
{
  if (ui->pushButton_4->text()=="Speak"){
   newclient->callToServer();//вызываем метод ядра для передачи данных
   ui->pushButton_4->setText("Stop");
  }else{
      if (ui->pushButton_4->text()=="Stop"){
          newclient->stopSendVoiceToServer();
          ui->pushButton_4->setText("Speak");
      }
  }

}

void ClientWindow::callToUser()
{
    QString recipient = ui->listWidget->currentItem()->text();


    if (ui->listWidget->item(0)->isSelected()){
       //TO DO
    }else{
          newclient->sendRequestForCalling(recipient);
    }

    ui->lineEdit->setText("");
}

void ClientWindow::outputSounds()
{
       if (ui->checkBox->isChecked()){
           newclient->outputSoundsFromServer();
       }

}

void ClientWindow::showMsgCallAccept(QString username, QString idAddress)
{
   QMessageBox msgBox;
   msgBox.setWindowTitle("User "+ username+" accept calling, his ip="+ idAddress);
   msgBox.setText("Congratulations, call accept!");
   msgBox.exec();

   //добавим ip адрес пользователя в массив связку

   userCalList.insert(username, QHostAddress(idAddress));
   //начинаем передачу данных
    callPtP(QHostAddress(idAddress), newclient->serverPortUDP);
}

void ClientWindow::showMsgCallReject(QString username)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("User "+ username+" reject calling");
    msgBox.setText("call reject!");
    msgBox.exec();

}

void ClientWindow::showMsgRequestForCalling(QString username)
{
    QMessageBox msgBox;
    msgBox.setText("Request for caling from "+ username);
                 msgBox.setInformativeText("Choose action");
                 msgBox.setIcon(QMessageBox::Question);
                 QPushButton *accept = msgBox.addButton("Accept", QMessageBox::ActionRole);
                 QPushButton *reject = msgBox.addButton("Reject", QMessageBox::ActionRole);
                 msgBox.setDefaultButton(accept);
                 msgBox.exec();
                 if(msgBox.clickedButton()== accept)
                 {
                    newclient->acceptCall(username);
                    //определяем связку ip и имя
                    //Передача аудио
                    QHostAddress userAddress;
                    bool userExist=false;
                    foreach (QString key, userCalList.keys()){
                        if (key==username){
                            userAddress = userCalList.value(key);
                            qDebug()<<"userAddress"<< userCalList.find(username).value();
                            userExist=true;
                            break;
                        }
                    }
                    if (userExist==false){
                         qDebug()<<"User with name "+username+" is not exist in userCalList";
                    }
                    else{
                         callPtP( userAddress, newclient->serverPortUDP );
                    }



                 }
                 else if(msgBox.clickedButton()== reject)
                 {
                    newclient->rejectCall(username);
                 }
}


void ClientWindow::callPtP(QHostAddress userAddress, quint16 audioPort)
{
    qDebug()<<"userAddress"<<userAddress<<"audioPort"<<audioPort;



    WSADATA dat;
#ifdef WIN32
    WSAStartup(MAKEWORD(2,2),&dat);
#endif // WIN32


    audioSessParams.setPortbase(audioPort);
    //audioSessParams.setSpeexIncomingPayloadType(97);
    //audioSessParams.setOpusIncomingPayloadType(98);




    auto ret  = audioSess.init(&audioSessParams);

    if (!ret)
    {
        std::cerr << audioSess.getErrorString() << std::endl;
        exit(-1);
    }


    uint32_t ipaddress = userAddress.toIPv4Address();
    this->audioSess.addDestination(jrtplib::RTPIPv4Address(ipaddress, audioPort,false));


}

void ClientWindow::stopCall()
{

   this->audioSess.destroy();

    #ifdef WIN64
    WSACleanup();
    #endif // WIN64


}





