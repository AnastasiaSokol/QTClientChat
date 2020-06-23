#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include <QObject>
#include <QWidget>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QtMultimedia/qaudioformat.h>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioInput>


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
    void        connectToHost();//Создает соединение с сервером
    //выделяет из строки <длина имени>_<имя> имя
    QString     extractClientName(QString d);
    //разбирает строку вида <длина имени>_<отправитель>_<сообщение>
    QStringList extractRecipientAndMsg(QString d);
    void        alive();
    void       readDatagrammFromServer(); //обрабатывает запросы от сервера
    void       processSignaHostFounded();//обрабатывает событие что сервер найден
    void       processSignaClientConnected();//обрабатывает событие клиенту удалось установть соединение
    void       processSignaErrorOccured();//обрабатывает ошибку установки соединения с сервером

    //---------------------------------------------------
    //запросы к серверу
    void        postConnectDatagramm(); // отправляет запрос приветствие серверу, что он соединился
    void        disconnect();
    void        sendMessage(QString msg);//отправка сообщения от клиента в общий чат
    void        sendPrivateMessage(QString msg,  QString addressRecipient);//отправка сообщения от клиента в приватный чат
    void        sendVoice();// reads voice from the microphone and sends it via UDP socket

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

  public:

   QTcpSocket   *tcpSocket; //tcp сокет для текстового чата
   quint16      blockSize = 0;

   QString      nikname;
   QByteArray   datagram;//датаграмма клиента
   QString      serverAddress;//адрес сервера
   quint16      serverPort;//порт сервера
};

#endif // CLIENTCHAT_H
