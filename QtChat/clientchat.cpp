#include "clientchat.h"


ClientChat::ClientChat()
{
    //сокет обязательно придется удалять, так как это указатель
    //и родителя у него нет
    tcpSocket = new QTcpSocket();
    /*Binds this socket to the address address and the port port.
     *  When bound, the signal readyRead() is emitted whenever a UDP
     * datagram arrives on the specified address and port. This function
     *  is useful to write UDP servers.*/
    tcpSocket->bind(14000, QTcpSocket::ShareAddress);
    nikname="DefaultNickName";



}

ClientChat::ClientChat(QString nickname)
{
    tcpSocket = new QTcpSocket();
    //принимает броадкастовский tcp пакет
    //tcpSocket->bind(port, QTcpSocket::ShareAddress);
    nikname=nickname;
    //-----------------------------------------------------------------------
    //настраиваем udp сокет
    udpSocket = new QUdpSocket(this);
    udpSocket->connectToHost("localHost",serverPortTCP);




    if(this->udpSocket->state()==QAbstractSocket::ConnectedState){
         qDebug() << "state of udpsocked is connected";
    }
    else{

         qDebug() << "state of udpSocket is not connected";
    }

    //-----------------------------------------------------------------------
    //настраиваем  формат аудио данных для отправки данных
    QAudioFormat inputDeviceFormat;
    QAudioDeviceInfo inputDeviceInfo;

    inputDeviceFormat.setSampleRate(44100);
    inputDeviceFormat.setChannelCount(2);
    inputDeviceFormat.setSampleSize(16);
    inputDeviceFormat.setCodec("audio/pcm");
    inputDeviceFormat.setByteOrder(QAudioFormat::LittleEndian);
    inputDeviceFormat.setSampleType(QAudioFormat::SignedInt);

    //If format isn't supported find the nearest supported
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(inputDeviceFormat))
        inputDeviceFormat = info.nearestFormat(inputDeviceFormat);

    inputDeviceInfoStr = info.deviceName();
    //Настраиваем устройство ввода
    input = new QAudioInput(inputDeviceFormat);
    input->setBufferSize(16384); //  set buffere size to solve sound distribution on windows but not solve this problem when use it in android
    //-----------------------------------------------------------------------
    QAudioFormat outputDeviceFormat;
    outputDeviceFormat.setSampleRate(44100);
    outputDeviceFormat.setChannelCount(2);
    outputDeviceFormat.setSampleSize(16);
    outputDeviceFormat.setCodec("audio/pcm");
    outputDeviceFormat.setByteOrder(QAudioFormat::LittleEndian);
    outputDeviceFormat.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info2(QAudioDeviceInfo::defaultOutputDevice());
    if (!info2.isFormatSupported(outputDeviceFormat))
        outputDeviceFormat = info2.nearestFormat(outputDeviceFormat);
    outputDeviceInfoStr = info2.deviceName(); // the supported output device

    output = new QAudioOutput(outputDeviceFormat);
    output->setBufferSize(16384);

    //-----------------------------------------------------------------------
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readDatagrammFromServer()));
    connect(tcpSocket, SIGNAL(hostFound()), this, SLOT(processSignaHostFounded()));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(processSignaClientConnected()));
    connect(tcpSocket, SIGNAL(errorOccurred), this, SLOT(processSignaErrorOccured()));
}

void ClientChat::setServerAddressAndPort(QString Address, quint16 port)
{
    this->serverAddress=Address;
    this->serverPortTCP=port;
}

void ClientChat::setServerUDPPort(quint16 port)
{
     this->serverPortUDP=port;
}


void ClientChat::connectToHostByTCP()
{
tcpSocket->connectToHost(this->serverAddress,
                             this->serverPortTCP);
if (tcpSocket->waitForConnected(1000))
    qDebug("Connected BY TCP");
    postConnectDatagramm();
}

void ClientChat::connectToHostByUDP()
{
udpSocket->connectToHost(this->serverAddress,
                             this->serverPortUDP);
if (udpSocket->waitForConnected(1000))
    qDebug("Connected BY UDP");
}



void ClientChat::alive()
{
      qDebug() << this->serverAddress<<":"<<this->serverPortTCP;
    //EVMp_ALIVE_<длина имени>_<имя> - сообщение о том, что клиент по-прежнему в сети.
    datagram =
            "EVMp_ALIVE_" +
            QByteArray::number(nikname.toUtf8().size())
            + "_"
            + nikname.toUtf8();
    if(this->tcpSocket->state()==QAbstractSocket::ConnectedState){
    tcpSocket->write( datagram.data(),
                      datagram.size());
    /*
    udpSocket->writeDatagram(
                datagram.data(),
                datagram.size(),
                QHostAddress(Address),
                port);
                */
    }
    else{

         qDebug() << "state of tcpSocket is not connected";
    }
}





//определяет тип датаграммы полученной от сервера
QString ClientChat::getTypeOfDatagramm(QString d)
{
     qDebug() << d;
    //EVMp_CONNECT_0_<длина имени>_<имя> — оповещение о пользователе, уже подключенном на момент входа («тихий» вход).
    if (d.startsWith("EVMp_CONNECT_0_")){
        qDebug() << "user alive";
        return "QuietEntry";

    }
    else{
    if (d.startsWith("EVMp_SENDMSG_")){
        return "SendMsg";
    }
    else {
        if (d.startsWith("EVMp_WELCOME"))
        {
              qDebug() << "client was connected to server";
              return "Welcome";
        }else{
            //EVMp_CONNECT_1_<длина имени>_<имя> — оповещение о новом пользователе.
            if (d.startsWith("EVMp_CONNECT_1_")){
                 qDebug() << "NewClient";
                  return "NewUser";
            }
            else{

                if (d.startsWith("EVMp_DISCONNECT_")){
                    qDebug() << "Disconnect";
                     return "Disconnect";
                }
                else{
                    //
                    if (d.startsWith("EVMp_PRIVATEMSG_")){
                        qDebug() << "SendPrivateMsg";
                        return "SendPrivateMsg";
                    }else{
                        if (d.startsWith("EVMp_CALLACCEPT_")){
                            qDebug() << "CallAccept";
                            return "CallAccept";
                        }
                        else{
                            if (d.startsWith("EVMp_CALLREJECT_")){
                                qDebug() << "CallReject";
                                return "CallReject";
                            }else{
                                if (d.startsWith("EVMp_CALL_")){
                                    qDebug() << "EVMpCall";
                                    return "EVMpCall";
                                }
                                else{
                                   return "Unknown";
                                }

                            }
                        }


                    }

                }
            }


        }

    }
    }

}

QStringList ClientChat::extractClientAddressAndName(QString d)
{
    QStringList paramsList;
    QString msg;
    QString ipAddress="";
    QString clientName="";
    msg = d;// user ip address  and user name
    //регулярным выражением отделим ip адрес от имени
    qDebug()<<msg;
    QRegExp rx("^\\d{1,3}.{1}\\d{1,3}.{1}\\d{1,3}.{1}\\d{1,3}");
    int pos = 0;
   QString regCap="";
    while ((pos = rx.indexIn(msg, pos)) != -1) {
        //совпадение найдено
        ipAddress= rx.cap();
        msg.remove(rx);
     }

    clientName = msg.left(msg.indexOf('_'));
    qDebug()<<"paramsList="<<clientName<<";"<<ipAddress;

    paramsList<<clientName;
    paramsList<<ipAddress;

    return paramsList;
}

QString ClientChat::extractClientName(QString d)
{
    QString msg;
    msg = d;//new user login and user name length
    //регулярным выражением отделим длину от имени
    qDebug()<<msg;
    QRegExp rx("^\\d{1,3}_{1}");//то есть число несколько раз потом нижнее подчеркивание
    int pos = 0;
   QString regCap="";
    while ((pos = rx.indexIn(msg, pos)) != -1) {
        //совпадение найдено
        msg.remove(rx);
     }

    qDebug()<<msg;
    return msg;
}
//разбирает строку вида <длина имени>_<отправитель>_<сообщение>
QStringList ClientChat::extractRecipientAndMsg(QString d)
{
    QStringList paramsList;

    QString msg;
    QString recipient;
    msg = d;//new user login and user name length
    //регулярным выражением отделим длину от имени
    qDebug()<<msg;
    QRegExp rx("^\\d{1,3}_{1}");//<длина имени>_<отправитель>_<сообщение>
    int pos = 0;
   QString regCap="";
    while ((pos = rx.indexIn(msg, pos)) != -1) {
        //совпадение найдено
        msg.remove(rx);
     }
    //qDebug()<<msg;
    //теперь в msg хранится толкьо <отправитель>_<сообщение>
    //получим имя отправителя
    QRegExp rx2("^\\w{1,}_{1}");
    pos = 0;
    while ((pos = rx2.indexIn(msg, pos)) != -1) {
        //qDebug()<<"совпадение найдено (отправитель))";
        recipient= rx2.cap();
        msg.remove(rx2);
    }

    recipient = recipient.left(recipient.indexOf('_'));
    qDebug()<<"paramsList="<<recipient<<";"<<msg;

    paramsList<<recipient;
    paramsList<<msg;
    return paramsList;
}

void ClientChat::readDatagrammFromServer()
{
    QString msg ="";

    QDataStream in(tcpSocket);
    QString d;
    QString typeOfDatagramm;
       while (tcpSocket->bytesAvailable())
       {


           if (blockSize == 0)
           {
              if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
              return;
              in >> blockSize;
              qDebug()<<"blockSize:"<<blockSize;
           }
           if (tcpSocket->bytesAvailable() < blockSize) return;
               blockSize = 0;



           in>>d;
           typeOfDatagramm=getTypeOfDatagramm(d);
           qDebug()<<"Data from server:"<<d;

           //EVMp_SENDMSG_<длина имени>_<отправитель>_<сообщение>
           if(typeOfDatagramm=="SendMsg"){
                qDebug()<<"typeOfDatagramm:"<<"SendMsg";
               QString headerDatagram="EVMp_SENDMSG_";
               int headerDatagramLength = headerDatagram.length();
               d.remove(0,headerDatagramLength); //delete header of datagram EVMp_SENDMSG_
               msg = d;
               qDebug() << msg;
               //msg хранит только <длина имени>_<отправитель>_<сообщение>
               QStringList params = extractRecipientAndMsg(msg);
               //имитируем сигнал клиента
               emit recipientSendMsg(params.takeLast(), params.takeFirst());
           }


           if(typeOfDatagramm=="SendPrivateMsg"){
                qDebug()<<"typeOfDatagramm:"<<"SendPrivateMsg";
               QString headerDatagram="EVMp_PRIVATEMSG_";
               int headerDatagramLength = headerDatagram.length();
               d.remove(0,headerDatagramLength); //delete header of datagram EVMp_SENDMSG_
               msg = d;
               qDebug() << msg;
               //msg хранит только <длина имени>_<отправитель>_<сообщение>
               QStringList params = extractRecipientAndMsg(msg);
               //имитируем сигнал клиента
               emit recipientSendPrivateMsg(params.takeLast(), params.takeFirst());
           }

           if(typeOfDatagramm=="QuietEntry"){
               qDebug()<<"typeOfDatagramm:"<<"QuietEntry";
               QString headerDatagram="EVMp_CONNECT_0_";
                QString userName ="";
               int headerDatagramLength = headerDatagram.length();
               d.remove(0,headerDatagramLength); //delete header of datagram EVMp_SENDMSG_
               msg = d;
               qDebug() << msg;
               //msg хранит только <длина имени>_<отправитель>_<сообщение>
               userName=extractClientName(d);//выделит только имя
               //имитируем сигнал клиента
               emit userEntry(userName);
               qDebug()<<"emit signal userEntry";
           }


           if(typeOfDatagramm=="NewClient"){
               qDebug()<<"typeOfDatagramm:"<<"NewClient";
               QString newConnectedUser ="";
               QString headerDatagram="EVMp_CONNECT_1_";
               int headerDatagramLength = headerDatagram.length();
               d.remove(0,headerDatagramLength); //delete header of datagram EVMp_CONNECT_
               //d - new user login and user name length
               //регулярным выражением отделим длину от имени
               newConnectedUser=extractClientName(d);//выделит только имя
               qDebug()<<newConnectedUser;
               emit userJoined(newConnectedUser);
           }

           if(typeOfDatagramm=="Disconnect"){
               qDebug()<<"typeOfDatagramm:"<<"Disconnect";
               QString clientNick ="";
               QString headerDatagram="EVMp_DISCONNECT_";
               int headerDatagramLength = headerDatagram.length();
               d.remove(0,headerDatagramLength); //delete header of datagram EVMp_CONNECT_
               //d - new user login and user name length
               //регулярным выражением отделим длину от имени
               clientNick=extractClientName(d);//выделит только имя
               qDebug()<<clientNick;
               emit userDisconnect(clientNick);
           }

           if(typeOfDatagramm=="Welcome"){
                qDebug()<<"typeOfDatagramm:"<<"Welcome";

                emit accessIsAllowed();
                qDebug()<<"emit signal accessIsAllowed";
           }
           if(typeOfDatagramm=="CallAccept"){
                qDebug()<<"typeOfDatagramm:"<<"CallAccept";
                QString headerDatagram="EVMp_CALLACCEPT_";
                QString userName ="";
                QString userAddress ="";
                int headerDatagramLength = headerDatagram.length();
                d.remove(0,headerDatagramLength); //delete header of datagram EVMp_CALLACCEPT_
                msg = d;
                qDebug() << msg;
                //Теперь надо отделить ip адрес от имени пользователя
                QStringList params = extractClientAddressAndName(msg);
                emit callAccept(params.takeFirst(), params.takeLast());

                qDebug()<<"emit signal callAccept";
           }
           if(typeOfDatagramm=="CallReject"){
                qDebug()<<"typeOfDatagramm:"<<"CallReject";
                QString headerDatagram="EVMp_CALLREJECT_";
                QString userName ="";
                int headerDatagramLength = headerDatagram.length();
                d.remove(0,headerDatagramLength); //delete header of datagram EVMp_CALLACCEPT_
                msg = d;
                userName=msg;
                qDebug() << msg;

                emit callReject(userName);

                qDebug()<<"emit signal CallReject";
           }

           if(typeOfDatagramm=="EVMpCall"){
                qDebug()<<"typeOfDatagramm:"<<"EVMpCall";
                QString headerDatagram="EVMp_CALL_";
                QString userName ="";
                int headerDatagramLength = headerDatagram.length();
                d.remove(0,headerDatagramLength); //delete header of datagram EVMp_CALLACCEPT_
                msg = d;
                userName=msg;
                qDebug() << msg;

                emit eVMpCall(userName);

                qDebug()<<"emit signal Call";
           }





       }//end while
    //-----------------------------------------------------------------
}

void ClientChat::processSignaHostFounded()
{
    qDebug()<<"Host was founded!";
    emit hostFounded();
}

void ClientChat::processSignaClientConnected()
{
    qDebug()<<"Cliect is connected!";
    emit cliectIsConnected();
}

void ClientChat::processSignaErrorOccured()
{
    qDebug()<<"Cant connect to server!";
    emit errorOccured();
}

void ClientChat::callToServer()
{
    if(this->udpSocket->state()==QAbstractSocket::ConnectedState){
         qDebug() << "state of udpsocked is connected";
         //отправка голоса прямо на сервер в общий голосовой чат
         input->start(udpSocket);
    }
    else{

         qDebug() << "state of udpSocket is not connected";
    }

}

void ClientChat::stopSendVoiceToServer()
{
    if(this->udpSocket->state()==QAbstractSocket::ConnectedState){
         qDebug() << "state of udpsocked is connected";
         //прекращение отправки голосовых данных на сервер
         input->stop();
    }
    else{

         qDebug() << "state of udpSocket is not connected";
    }

}

QString ClientChat::getSupportedInputDevice(){

    return this->inputDeviceInfoStr;
}


QString ClientChat::getSupportedOutputDevice(){

    return this->outputDeviceInfoStr;
}






//============================================================================
//ОТ КЛИЕНТА К СЕРВЕРУ
void ClientChat::postConnectDatagramm()
{

    qDebug() << this->serverAddress<<this->serverPortTCP;
    //EVMp_CONNECT_<длина имени>_<имя> - подключение клиента.

    //----------------------------------------------------------------
    //qDebug() << "nikname="+nikname;
    QString datagrammConnect="EVMp_CONNECT_"+ QByteArray::number (nikname.toUtf8().size())+"_"+ nikname.toUtf8();
    QByteArray l;
    QDataStream stream (&l, QIODevice::WriteOnly);
    stream << quint16(0);
    stream << datagrammConnect;
    stream.device()->seek(0);
    stream << quint16(l.size() - sizeof(quint16));
    if(this->tcpSocket->state()==QAbstractSocket::ConnectedState){
    tcpSocket->write(l);
    tcpSocket->waitForBytesWritten();

    //----------------------------------------------------------------
\
    qDebug() << "postConnectDatagramm("<<datagrammConnect<<")";
   }
   else{

        qDebug() << "state of tcpSocket have not connected state. I cant post connect datagram";

   }

}

void ClientChat::disconnect()
{

    if(this->tcpSocket->state()==QAbstractSocket::ConnectedState){
        tcpSocket->disconnectFromHost();
        emit userDisconnect(this->nikname);
    }
}
/**Метод отправляет сообщение в общий чат**/
void ClientChat::sendMessage(QString msg)
{
      qDebug() << this->serverAddress<<this->serverPortTCP;
    //EVMp_SENDMSG_<сообщение> - отправка сообщения.
    if (msg.isEmpty())
        return;


      QString datagrammSendMsg="EVMp_SENDMSG_"+ msg.toUtf8();
      QByteArray l;
      QDataStream stream (&l, QIODevice::WriteOnly);
      stream << quint16(0);
      stream << datagrammSendMsg;
      stream.device()->seek(0);
      stream << quint16(l.size() - sizeof(quint16));

      if(this->tcpSocket->state()==QAbstractSocket::ConnectedState){
          tcpSocket->write(l);
          tcpSocket->waitForBytesWritten();
            qDebug() << "postConnectDatagramm("<<datagrammSendMsg<<")";
      }
      else{

           qDebug() << "state of tcpSocket is not connected";
      }
}

void ClientChat::sendPrivateMessage(QString msg, QString recipient)
{
     qDebug() << this->serverAddress<<":"<<this->serverPortTCP;
    if (msg.isEmpty())
        return;


    QString datagrammSendPrivateMsg="EVMp_PRIVATEMSG_"
            + QByteArray::number (recipient.toUtf8().size())
            +"_"
            + recipient.toUtf8()
            +"_"
            + msg.toUtf8();
    QByteArray l;
    QDataStream stream (&l, QIODevice::WriteOnly);
    stream << quint16(0);
    stream << datagrammSendPrivateMsg;
    stream.device()->seek(0);
    stream << quint16(l.size() - sizeof(quint16));

    if(this->tcpSocket->state()==QAbstractSocket::ConnectedState){
        tcpSocket->write(l);
        tcpSocket->waitForBytesWritten();
        qDebug() << "postConnectDatagramm("<<datagrammSendPrivateMsg<<")";
    }
    else{

         qDebug() << "state of tcpSocket is not connected";
    }

}

void ClientChat::sendRequestForCalling(QString recipient)
{
   // qDebug() << this->serverAddress<<":"<<this->serverPort;
   if (recipient.isEmpty())
       return;


   QString datagrammRequest="EVMp_CALL_"+recipient.toUtf8();

   QByteArray l;
   QDataStream stream (&l, QIODevice::WriteOnly);
   stream << quint16(0);
   stream << datagrammRequest;
   stream.device()->seek(0);
   stream << quint16(l.size() - sizeof(quint16));

   if(this->tcpSocket->state()==QAbstractSocket::ConnectedState){
       tcpSocket->write(l);
       tcpSocket->waitForBytesWritten();
       qDebug() << "postDatagramm("<<datagrammRequest<<")";
   }
   else{

        qDebug() << "state of tcpSocket is not connected";
   }
}

void ClientChat::acceptCall(QString recipient)
{
    // qDebug() << this->serverAddress<<":"<<this->serverPort;
    if (recipient.isEmpty())
        return;


    QString datagrammRequest="EVMp_CALLACCEPT_"+recipient.toUtf8();

    QByteArray l;
    QDataStream stream (&l, QIODevice::WriteOnly);
    stream << quint16(0);
    stream << datagrammRequest;
    stream.device()->seek(0);
    stream << quint16(l.size() - sizeof(quint16));

    if(this->tcpSocket->state()==QAbstractSocket::ConnectedState){
        tcpSocket->write(l);
        tcpSocket->waitForBytesWritten();
        qDebug() << "postDatagramm("<<datagrammRequest<<")";
    }
    else{

         qDebug() << "state of tcpSocket is not connected";
    }
}

void ClientChat::rejectCall(QString recipient)
{
    // qDebug() << this->serverAddress<<":"<<this->serverPort;
    if (recipient.isEmpty())
        return;


    QString datagrammRequest="EVMp_CALLREJECT_"+recipient.toUtf8();

    QByteArray l;
    QDataStream stream (&l, QIODevice::WriteOnly);
    stream << quint16(0);
    stream << datagrammRequest;
    stream.device()->seek(0);
    stream << quint16(l.size() - sizeof(quint16));

    if(this->tcpSocket->state()==QAbstractSocket::ConnectedState){
        tcpSocket->write(l);
        tcpSocket->waitForBytesWritten();
        qDebug() << "postDatagramm("<<datagrammRequest<<")";
    }
    else{

         qDebug() << "state of tcpSocket is not connected";
    }
}

void ClientChat::sendVoice()
{
    /*
           QAudioFormat format;
           format.setSampleRate(128000);
           format.setChannelCount(1);
           format.setSampleSize(16);//Устанавливает размер выборки в указанный sampleSize в битах
           format.setCodec("audio/pcm");
           format.setByteOrder(QAudioFormat::LittleEndian);
           format.setSampleType(QAudioFormat::UnSignedInt);
           QAudioInput* input = new QAudioInput(format);
           QUdpSocket* socket = new QUdpSocket();
           socket->connectToHost("127.0.0.1", 1002);
           input->start(socket);*/
}

void ClientChat::outputSoundsFromServer()
{
    output->start();
}





