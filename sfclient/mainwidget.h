#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QString>
#include <QTcpSocket>
#include <QMutex>
#include <memory>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QCloseEvent>
#include "startdialog.h"
#include "user.h"
#include "message.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    MainWidget(const QString &host, const int port, QWidget *parent = nullptr);
    ~MainWidget();
    void showStartMenu();

private:
    Ui::MainWidget *ui;

    std::unique_ptr<StartDialog> sd;//стартовое диалоговое окно
    QString currentLogin_;//текущий логин пользователя
    QString currentChat_;//выбранный канал в чате
    QString currentChatUser_;//выбранный канал в чате пользователей
    QString host_;//хост для подключения
    int port_;//порт для подключения
    std::unique_ptr<QTcpSocket> tcpSocket_;//TCP-сокет для соединения
    quint16 nextBlockSize_;//переменная. Нужна для отправки данных
    bool isConnected_;//подключено
    QVector<User> users_;//вектор пользователей
    QVector<Message> messages_;//вектор сообщений
    bool isBegin_;//бит для отбрасывания срабатывания выбора пользователя при первом запуске

    //Отправить запрос на добавление пользователя  на сервер
    void sendUserToServer(const QString &login, const QString &name, const QString &pass);
    //Отправить данные пользователя на сервер
    void sendUserDataToServer(const QString &login, const QString &pass);
    //Отправить команду с одним параметром
    void sendComandWithOneParam(const QString &comand, const QString &param);
    void showSelfChat();//показать сообщения личного чата
    //Показать сообщения общего чата
    void showPublicChat();
    //Для обновления окна
    void updateUsers();
    //Отправить запрос на получение списка пользователей и списка сообщений
    void sendUserAndMessagesRequest();
    //Отправить запрос на получение списка сообщений
    void sendMessagesRequest();
    //Отправить запроc на добавления сообщенияна сервер
    void sendMessageToServer(const QString &from, const QString &to, const QString &text);
    //Получить список сообщений
    void receiveMessages(const QVector<QString> &v);
    //Получить список сообщений и пользователей
    void receiveUsersAndMessages(const QVector<QString> &v);
    //Метод срабатываемый при закрытии окна
    void closeEvent (QCloseEvent *event);

private slots:
    void checkEnterData(const QString &login, const QString &password, const QString &host, const QString &port);//слот для проверки входных данных
    void on_buttonAddUser_clicked();//слот для обработки нажатия кнопки добавить пользователя
    void on_buttonSend_clicked();//нажата кнопка отправить сообщение
    void slotError(QAbstractSocket::SocketError err);//ошибки с сетью
    void slotChagedComboBox(const QString &text);//слот для обработки выбора ComboBox
    void slotChagedComboUsers(const QString &text);//cлот для обработки выбора ComboBox с выбором пользователя
    void slotReadyRead();//слот для чтения сообщений от сервера

};
#endif // MAINWIDGET_H
