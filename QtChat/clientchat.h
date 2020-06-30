#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include <QObject>
#include <QWidget>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QAudioOutput>
#include <QAudioInput>
#include <QAudioDeviceInfo>
#include <QAudioFormat>

class ClientChat : public QObject
{
    Q_OBJECT
public:
    ClientChat();
    ClientChat(QString nickname);
private slots:
    //---------------------------------------------------
    //определяет тип датаграммы от сервера
    QString     getTypeOfDatagramm(QString d);//определяет тип датаграммы - CONNECT, ALIVE or SENDMSG

public slots:


    void        setServerAddressAndPort(QString Address, quint16 port);// Установка сведений о сервере
     void       setServerUDPPort( quint16 port);
    void        connectToHostByTCP();//Создает соединение с сервером по TCP
    void        connectToHostByUDP();//создает соединение с сервером по UDP
    //выделяет из строки <длина имени>_<имя> имя
    QString     extractClientName(QString d);
    //разбирает строку вида <длина имени>_<отправитель>_<сообщение>
    QStringList extractRecipientAndMsg(QString d);
    void        alive();
    void       readDatagrammFromServer(); //обрабатывает запросы от сервера
    void       processSignaHostFounded();//обрабатывает событие что сервер найден
    void       processSignaClientConnected();//обрабатывает событие клиенту удалось установть соединение
    void       processSignaErrorOccured();//обрабатывает ошибку установки соединения с сервером
    void       callToServer();//осуществить звонок на сервер
    void       stopSendVoiceToServer();//прекращение передачи голоса на сервер
    QString    getSupportedInputDevice();//получить информацию о  устройстве ввода
    QString    getSupportedOutputDevice();//получить информацию о  устройстве выввода
    //---------------------------------------------------
    //запросы к серверу
    void        postConnectDatagramm(); // отправляет запрос приветствие серверу, что он соединился
    void        disconnect();
    void        sendMessage(QString msg);//отправка сообщения от клиента в общий чат
    void        sendPrivateMessage(QString msg,  QString addressRecipient);//отправка сообщения от клиента в приватный чат

    //запросы к серверу для осуществления звонков
    void        sendRequestForCalling(QString recipient);//отправка сообщения от клиента что он хочет позвонить кому-то
    void        acceptCall(QString recipient);//подтверждение согласия на соединение
    void        rejectCall(QString recipient);//отклонение  соединения
    QStringList extractClientAddressAndName(QString d);

    void        sendVoice();// reads voice from the microphone and sends it via UDP socket
    void        outputSoundsFromServer();// reads voice from the server and output

    //---------------------------------------------------

signals:
    void        userJoined(QString username); // добавление нового пользователя в чат
    void        userEntry(QString username);//пользователь авторизовавшийся (тихий вход)
    void        accessIsAllowed();//сигнал что доступ клиету разрешен от сервера
    void        recipientSendMsg(QString msg, QString recipient);//сообщение от сервера поступило что пользователь В отправил сообщение
    void        recipientSendPrivateMsg(QString msg, QString recipient);//сообщение что userB прислал приватное сообщение
    void        userDisconnect(QString username);//пользователь отключился от чата
    void        hostFounded();//клиент нашел хост
    void        cliectIsConnected();
    void        errorOccured();
    //сигналы относязие к реализации звонков
    void        callAccept(QString username, QString idAddress);
    void        callReject(QString userName);
    void        eVMpCall(QString userName);

  public:

   QTcpSocket   *tcpSocket; //tcp сокет для текстового чата
   QUdpSocket   *udpSocket; //udp сокет для голосового чата
   quint16      blockSize = 0;

   QString      nikname;
   QByteArray   datagram;//датаграмма клиента
   QString      serverAddress;//адрес сервера
   quint16      serverPortTCP;//порт сервера для tcp
   quint16      serverPortUDP;//порт сервера для udp

   QAudioInput*  input;
   QAudioOutput* output;
   QString inputDeviceInfoStr;
   QString outputDeviceInfoStr;


};

#endif // CLIENTCHAT_H
