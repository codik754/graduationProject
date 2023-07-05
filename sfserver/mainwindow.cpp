//Файл mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QString>
#include <QMessageBox>
#include <QItemDelegate>
#include "thisitembox.h"
#include <QtCore>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , users_()
    , messages_()
    , tcpServer_(new QTcpServer(this))
    , nextBlockSize_(0)
    , port_(55000)
    , dbName_("defaultdb")
    , db_(QSqlDatabase::addDatabase("QSQLITE"))
    , pSocket_(nullptr)
    , currentUser_("UNKNOWN")
{

}

MainWindow::MainWindow(const int port, const QString &dbName, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , users_()
    , messages_()
    , tcpServer_(new QTcpServer(this))
    , nextBlockSize_(0)
    , port_(port)
    , dbName_(dbName)
    , db_(QSqlDatabase::addDatabase("QSQLITE"))
    , pSocket_(nullptr)
    , currentUser_("UNKNOWN")
{
    ui->setupUi(this);

    QStringList nameColumn;
    nameColumn.push_back("User");
    nameColumn.push_back("Blocked");
    nameColumn.push_back("Banned");
    nameColumn.push_back("Auth");
    ui->tableUsers->setHorizontalHeaderLabels(nameColumn);

    QStringList nameColumnMessages;
    nameColumnMessages.push_back("From");
    nameColumnMessages.push_back("To");
    nameColumnMessages.push_back("Date and time");
    nameColumnMessages.push_back("Text");
    ui->tableMessages->setHorizontalHeaderLabels(nameColumnMessages);

    //Подключаемся к БД
    connectToDb();
    //Считываем пользователей из БД
    readUsersFromBd();
    //Считываем все сообщения из БД
    readMessagesFromBd();

    //Добавляем пользователей в таблицу
    showUserForTable();
    //Добавляем сообщения в таблицу
    showMessageToTable();

}

MainWindow::~MainWindow()
{
    delete ui;
}

//Вывести сообщение в поле журнала
void MainWindow::addToLog(const QString &str){
    QString m =  QDateTime::currentDateTime().toString("[dd.MM.yyyy hh:mm:ss] ") + str + "\n";
    ui->textLog->insertPlainText(m);
}

//Вывести сообщение в поле журнала выделеное красным цветом
void MainWindow::addToLogRed(const QString &str){
    QString m =  "<div style='color:red;'>" + QDateTime::currentDateTime().toString("[dd.MM.yyyy hh:mm:ss] ") + str + "</div><br>";
    ui->textLog->insertHtml(m);
}

//Вывести сообщение в поле журнала выделеное зеленым цветом
void MainWindow::addToLogGreen(const QString &str){
    QString m =  "<div style='color:green;'>" + QDateTime::currentDateTime().toString("[dd.MM.yyyy hh:mm:ss] ") + str + "</div><br>";
    ui->textLog->insertHtml(m);
}


void MainWindow::on_actionExit_triggered()
{
    //Закрываем приложение
    qApp->quit();
}

void MainWindow::on_actionStopToolBar_triggered()
{
    stopTcpServer();
}


void MainWindow::on_actionStartToolBar_triggered()
{
    runTcpServer();
}

void MainWindow::sendToClient(std::shared_ptr<QTcpSocket*> pSocket, const QString &str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    out << quint16(0) << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    (*pSocket)->write(arrBlock);
    (*pSocket)->waitForBytesWritten();
}


//Изменен статус блокировки в таблице
void MainWindow::changeStatusBlocked(const int index, const QString &login, const int row)
{
  //В зависимости от выбора в таблице производим соотетствующие действия
  if(index == 0){
        users_[row].resetBlock();
        //Добавляем запись в журнал
        addToLogGreen("The " + users_[row].getLogin() + " user is unblocked");
  }
  else{
        users_[row].setBlock();
        //Добавляем запись в журнал
        addToLogRed("The " + users_[row].getLogin() + " user is blocked");
        changeAuthStatusOnTableUsersOther(row, false);
        //currentUser_ = "UNKNOWN";
  }

  if(login != currentUser_){
      return;
  }

  if(users_[row].checkAuth() && (*pSocket_)->isOpen()){
    sendComandWithOneParam("Blocked", "Yes");
  }
  currentUser_ = "UNKNOWN";
}

//Изменен статус бана в таблице
void MainWindow::changeStatusBanned(const int index, const QString &login, const int row)
{
    //В зависимости от выбора в таблице производим соотетствующие действия
    if(index == 0){
          users_[row].resetBan();
          //Добавляем запись в журнал
          addToLogGreen("The " + users_[row].getLogin() + " user is unbanned");
    }
    else{
          users_[row].setBan();
          //Добавляем запись в журнал
          addToLogRed("The " + users_[row].getLogin() + " user is banned");
    }

    if(login != currentUser_){
        return;
    }

    if(!users_[row].checkAuth() && !(*pSocket_)->isOpen()){
       return;
    }

    //Отправляем значение
    if(index == 0){
        sendComandWithOneParam("UnBanned", "Yes");
    }
    else{
        sendComandWithOneParam("Banned", "Yes");
    }
}

void MainWindow::slotNewConnection()
{
    QTcpSocket *clientSocket = tcpServer_->nextPendingConnection();
    connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

    pSocket_ = std::make_shared<QTcpSocket*>(clientSocket);
    sendToClient(pSocket_, "Server Response: Connected)))");
}

void MainWindow::slotReadClient()
{
    QTcpSocket *clientSocket = (QTcpSocket*)sender();
    QDataStream in(clientSocket);
    QVector<QString> v;//вектор для получения сообщений
    in.setVersion(QDataStream::Qt_5_3);
    for(;;){
       if(!nextBlockSize_){
          if(clientSocket->bytesAvailable() < qint64(sizeof(quint16))){
             break;
          }
       in >> nextBlockSize_;
       }

       if(clientSocket->bytesAvailable() < nextBlockSize_){
           break;
       }

       QString str;
       in >> str;

       v.push_back(str);

       nextBlockSize_ = 0;

    }

    if(v.size() > 0){
        //Проверяем данные пользователя
        if(v[0] == "CheckUser"){
            checkUser(v[1], v[2]);
        }

        else if(v[0] == "AddUser"){
            //Добавляем пользователя
            addNewUser(v[1], v[2], v[3]);
        }
        else if(v[0] == "AddMessage"){
            //Добавляем сообщение
            addMessageToBd(v[1], v[2], v[3]);
            messages_.push_back(Message(v[1], v[2], v[3],  QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss")));
            addToMessagesTable(v[1], v[2], v[3],  QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"));
            addToLogGreen("Added a message from " + v[1]);
        }
        else if(v[0] == "UsersAndMessages"){
            //Отправляем данные пользователе и сообщений
            sendUsersAndMessages(v[1]);
        }
        else if(v[0] == "Exit"){
            //Обрабатывам команду по выходу пользователя
            int index = 0;

            //Узнаем индекс
            for(int i = 0; i < users_.size(); ++i){
                if(users_[i].checkLogin(v[1])){
                    index = i;
                    break;
                }
            }

            users_[index].resetAuth();//сбрасываем бит авторизации

            //Изменяем статус авторизации в таблице
          //  changeAuthStatusOnTableUsers(index);
            changeAuthStatusOnTableUsersOther(index, false);
            //Выводим сообщение в журнал
            addToLog("User " + users_[index].getLogin() + " is out");
        }
    }
}


//Подключение к БД
void MainWindow::connectToDb(){
    //Проверяем существует ли файл с базой данных
    //Если не существует, то создаем его
    QFileInfo checkFile(dbName_);
    if(!checkFile.exists() && !checkFile.isFile()){
        addToLog(QObject::tr("The database file does not exist"));
        createDataBase();//создаем БД с пустыми таблицами
        return;
    }

    //Открываем БД
    db_.setDatabaseName(dbName_);
    if(db_.open()){
        addToLogGreen("The connection to the database is established");
    }else{
        addToLogRed("Database opening error");
    }
}

//Cоздание базы данных
void MainWindow::createDataBase(){
    db_.setDatabaseName(dbName_);
    //Открываем БД
    if(db_.open()){
        addToLog(QObject::tr("The database file has been created"));
        addToLogGreen(QObject::tr("The connection to the database is established"));
    }else{
        addToLogRed(QObject::tr("Database creation error"));
        addToLogRed(QObject::tr("Database opening error"));
        return;
    }

    addToLog(QObject::tr("The creation of the necessary tables has begun"));

    QSqlQuery query;

    //Создаем таблицу пользователей
    //Формируем запрос
    QString str = "CREATE TABLE users("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "login VARCHAR(30),"
            "name VARCHAR(30),"
            "pass VARCHAR(100));";

    //Выполняем запрос
    if(!query.exec(str)){
        addToLogRed(QObject::tr("User table creation error"));
        return;
    }

    //Создаем таблицу сообщений
    //Формируем запрос
    str = "CREATE TABLE messages(id INT AUTO_INCREMENT PRIMARY KEY,"
          "`from` VARCHAR(30),"
          "`to` VARCHAR(30),"
          "`text` VARCHAR(500),"
          "`datetime` DATETIME);";

    //Выполняем запрос
    if(!query.exec(str)){
        addToLogRed(QObject::tr("Message table creation error"));
        return;
    }

    //Добавляем пользователя admin в таблицу
    //Формируем запрос
    str = "INSERT INTO users(login, name, pass) "
          "VALUES('%1', '%2', '%3');";

    //Задаем пароль
    QString pass = "a123";
    QByteArray ba = pass.toUtf8();

    QString str1 = QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex();
    //Добавляем значения в строку
    str = str.arg("admin")
            .arg("ADMIN")
            .arg(str1);

    //Выполняем запрос
    if(!query.exec(str)){
        addToLogRed(QObject::tr("Error adding a user"));
        return;
    }
    //Добавляем запись в журнал
    addToLogGreen(QObject::tr("All tables are created"));
}

//Прочитать всех пользователей из БД
void MainWindow::readUsersFromBd()
{
    QSqlQuery query;
    //Формируем запрос
    QString str = "SELECT * FROM users;";

    //Отправляем запрос
    if(!query.exec(str)){
        addToLogRed("Error reading users from the table");
        return;
    }

    //Считываем данные
    QSqlRecord r = query.record();

    //Добавляем в вектор
    while(query.next()){
        QString login = query.value(r.indexOf("login")).toString();
        QString name = query.value(r.indexOf("name")).toString();
        QString pass = query.value(r.indexOf("pass")).toString();

        users_.push_back(User(login, pass, name));
    }
}


//Прочитать все сообщения из БД
void MainWindow::readMessagesFromBd()
{
    QSqlQuery query;
    //Формируем запрос
    QString str = "SELECT `from`, `to`, `text`, strftime('%d-%m-%Y %H:%M:%S', `datetime`) as datetime FROM messages;";

    if(!query.exec(str)){
        addToLogRed("Error reading messages from the table");
        return;
    }

    //Считываем данные
    QSqlRecord r = query.record();

    //Добавляем в вектор
    while(query.next()){
        QString from = query.value(r.indexOf("from")).toString();
        QString to = query.value(r.indexOf("to")).toString();
        QString text = query.value(r.indexOf("text")).toString();
        QString dateandtime = query.value(r.indexOf("datetime")).toString();
        messages_.push_back(Message(from, to, text, dateandtime));
    }
}


//Добавить пользователя в таблицу
void MainWindow::addUserToBd(const QString &login, const QString &pass, const QString &name)
{
    QSqlQuery query;
    //Добавляем пользователя в таблицу
    //Формируем запрос
    QString str = "INSERT INTO users(login, name, pass) "
          "VALUES('%1', '%2', '%3');";

    //Добавляем значения в строку
    str = str.arg(login)
            .arg(name)
            .arg(pass);

    //Выполняем запрос
    if(!query.exec(str)){
        addToLogRed(QObject::tr("Error adding a user"));
        return;
    }
}

//Добавление сообщения в таблицу БД
void MainWindow::addMessageToBd(const QString &from, const QString &to, const QString &text)
{
    QSqlQuery query;
    //Добавляем сообщение в таблицу
    //Формируем запрос
    QString str = "INSERT INTO messages(`from`, `to`, `text`, `datetime`) "
          "VALUES('%1', '%2', '%3', datetime('now','localtime'));";

    //Добавляем значения в строку
    str = str.arg(from)
            .arg(to)
            .arg(text);

    //Выполняем запрос
    if(!query.exec(str)){
        addToLogRed(QObject::tr("Error adding a message"));
        return;
    }
}

//Добавить сроку в таблицу пользователей
void MainWindow::addToUsersTable(const QString &login, const bool isBlocked, const bool isBanned)
{
    //Считываем количество строк
    int countRow = ui->tableUsers->rowCount();
    //Увеличиваем количество строк в таблице на 1
    ui->tableUsers->setRowCount(countRow + 1);

    QTableWidgetItem *item1 = new QTableWidgetItem(login);

    //Создаем указатель
    ThisItemBox *combo1 = new ThisItemBox(login, countRow);

    //Соединяем сигнал слотом
    connect(combo1, SIGNAL(changeValue(const int, const QString&, const int)), this, SLOT(changeStatusBlocked(const int, const QString&, const int)));

    //Если статус уже заблокирован
    if(isBlocked){
        combo1->setCurrentIndex(1);
    }

    //Добавляем в таблицу
    ui->tableUsers->setCellWidget(countRow, 1, combo1);

    //Создаем указатель
    ThisItemBox *combo2 = new ThisItemBox(login, countRow);

    //Соединяем сигнал со слотом
    connect(combo2, SIGNAL(changeValue(const int, const QString&, const int)), this, SLOT(changeStatusBanned(const int, const QString&, const int)));

    //Если статус забанен
    if(isBanned){
        combo1->setCurrentIndex(1);
    }

    //Добавляем  в таблицу
    ui->tableUsers->setCellWidget(countRow, 2, combo2);

    //Создаем элемент
    QTableWidgetItem *item4 = new QTableWidgetItem("No");

    //Добавляем в таблицу
    ui->tableUsers->setItem(countRow, 0, item1);
    ui->tableUsers->setItem(countRow, 3, item4);

}

//Добавить строку в таблицу с сообщениями
void MainWindow::addToMessagesTable(const QString &from, const QString &to, const QString &text, const QString &datetime)
{
    //Считываем количество строк
    int countRow = ui->tableMessages->rowCount();
    //Увеличиваем количество строк в таблице на 1
    ui->tableMessages->setRowCount(countRow + 1);

    //Создаем элементы
    QTableWidgetItem *item1 = new QTableWidgetItem(from);
    QTableWidgetItem *item2 = new QTableWidgetItem(to);
    QTableWidgetItem *item3 = new QTableWidgetItem(datetime);
    QTableWidgetItem *item4 = new QTableWidgetItem(text);

    //Добавляем в таблицу
    ui->tableMessages->setItem(countRow, 0, item1);
    ui->tableMessages->setItem(countRow, 1, item2);
    ui->tableMessages->setItem(countRow, 2, item3);
    ui->tableMessages->setItem(countRow, 3, item4);
}

//Изменить статус авторизации в таблице
void MainWindow::changeAuthStatusOnTableUsers(const int index)
{
    QTableWidgetItem *temp = ui->tableUsers->takeItem(index, 3);

    if(temp->text() == "No"){
        temp->setText("Yes");
    }
    else{
        temp->setText("No");
    }

    ui->tableUsers->setItem(index, 3, temp);
}

//Изменить статус авторизации в таблице
void MainWindow::changeAuthStatusOnTableUsersOther(const int index, bool bit)
{
    QTableWidgetItem *temp = ui->tableUsers->takeItem(index, 3);

    if(bit){
        temp->setText("Yes");
    }
    else{
        temp->setText("No");
    }

    ui->tableUsers->setItem(index, 3, temp);
}

//Отобразить пользователей в таблице
void MainWindow::showUserForTable()
{
    //Проходим по всему вектору пользователей
   for(auto u : users_){
       addToUsersTable(u.getLogin(), false, false);
   }
}

//Отобразить сообщения в таблице
void MainWindow::showMessageToTable()
{
    //Проходим по всему вектору сообщений
   for(auto m : messages_){
       addToMessagesTable(m.getSendedFrom(), m.getSendedTo(), m.getText(), m.getDatetime());
   }
}

//Запустить TCP-сервер
void MainWindow::runTcpServer()
{
    if(!tcpServer_->listen(QHostAddress::Any, port_)){
       addToLogRed("Server Error Unable to start the server: " + tcpServer_->errorString());
       tcpServer_->close();
       return;
    }

    connect(tcpServer_.get(), SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    addToLogGreen("The server is running");
}

//Остановить TCP-сервер
void MainWindow::stopTcpServer()
{
    tcpServer_->close();

    disconnect(tcpServer_.get(), SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

    addToLogGreen("The server is stopped");
}

//Проверка данных пользователя полученных по сети
void MainWindow::checkUser(const QString &login, const QString &pass)
{
    bool b = false;

    int i = 0;
    for(i = 0; i < users_.size(); ++i){
        if(users_[i].checkLogin(login)){
            if(users_[i].checkPassword(pass)){
                b = true;
                break;
            }
        }
    }

    if(b){
        currentUser_ = login;

        if(users_[i].getBlock()){
            if((*pSocket_)->isOpen()){
                sendComandWithOneParam("CheckUser", "Block");
            }
            (*pSocket_)->close();
            currentUser_ = "UNKNOWN";
            return;
        }

        users_[i].setAuth();

        if(users_[i].getBan()){
            sendComandWithOneParam("CheckUser", "Ban");
            changeAuthStatusOnTableUsersOther(i, true);
            return;
        }

        sendComandWithOneParam("CheckUser", "Yes");
        //Меняем статус
        changeAuthStatusOnTableUsersOther(i, true);
        addToLog("User " + login + " has logged in");
    }
    else{
        sendComandWithOneParam("CheckUser", "No");
        addToLogRed("User " + login + " hasn't logged in");
    }
}

//Отправить команду с одним параметром
void MainWindow::sendComandWithOneParam(const QString &comand, const QString &param)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    out << quint16(0) << comand << quint16(0) << param;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    if((*pSocket_)->isOpen()){
        (*pSocket_)->write(arrBlock);
    }
}

//Отправить список пользователей клиенту
void MainWindow::sendUsers(int size)
{
    //Если количество пользователей совпадает
    if(users_.size() == size){
        return;
    }

    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    QString str = "Users";
    out <<  quint16(0) << str;

    for(int i = size; i < users_.size(); ++i){
        out << quint16(0) << users_[i].getLogin()  << quint16(0) << users_[i].getName();
    }

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    (*pSocket_)->write(arrBlock);
    (*pSocket_)->waitForBytesWritten();
}

//Отправить список сообщений клиенту
void MainWindow::sendMessages(const QString &login)
{
    //Объексты. Нужны для отправики
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    QString str = "Messages";
    out <<  quint16(0) << str;

    //Вектор сообщений временный
    QVector<Message> tmessages;

    //Формируем вектор сообщений
    for(auto m : messages_){
        if(m.checkToSendedTo(login) || m.checkToSendedTo("all") || m.checkToSendeFrom(login)){
            tmessages.push_back(m);
        }
    }

    //Добавляем сообщения
    out << quint16(0) << QString::number(tmessages.size());
    for(int i = 0; i < tmessages.size(); ++i){
       out << quint16(0) << messages_[i].getSendedFrom()  << quint16(0) << messages_[i].getSendedTo() << quint16(0) <<
                           messages_[i].getText() << quint16(0) << messages_[i].getDatetime();
    }
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    //Отправляем
    (*pSocket_)->write(arrBlock);
}

//Отправить список сообщений и пользователей клиенту
void MainWindow::sendUsersAndMessages(const QString &login)
{
    //Объект нужен для обработки
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);

    QString str = "UsersAndMessages";
    out <<  quint16(0) << str;

    //Добавляем количество пользователей
    out << quint16(0) << QString::number(users_.size());

    //Добавляем пользователей на отправку
    for(int i = 0; i < users_.size(); ++i){
            out << quint16(0) << users_[i].getLogin()  << quint16(0) << users_[i].getName();
    }

    //Вектор сообщений временный
    QVector<Message> tmessages;

    //Формируем вектор сообщений
    for(auto m : messages_){
        if(m.checkToSendedTo(login) || m.checkToSendedTo("all") || m.checkToSendeFrom(login)){
            tmessages.push_back(m);
        }
    }

    //Добавляем сообщения
    out << quint16(0) << QString::number(tmessages.size());
    for(int i = 0; i < tmessages.size(); ++i){
       out << quint16(0) << tmessages[i].getSendedFrom()  << quint16(0) << tmessages[i].getSendedTo() << quint16(0) <<
                           tmessages[i].getText() << quint16(0) << tmessages[i].getDatetime();
    }


    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    //Отправляем
    (*pSocket_)->write(arrBlock);
}

//Добавление нового пользователя
void MainWindow::addNewUser(const QString &login, const QString &name, const QString &pass)
{
    bool b = false;//переменная. Нужна для проверки
    //Проверяем есть ли пользователь такой
    for(auto u : users_){
        if(u.checkLogin(login)){
                b = true;
                break;
        }
    }
    //Если пользователь есть
    if(b){
        //Отправляем сообщение о неудаче
        sendComandWithOneParam("AddUser", "No");
        //Выводим запись в журнал
        addToLogRed("User " + login + " not added");
    }
    else{
        //Отправляем сообщение об успехи
        sendComandWithOneParam("AddUser", "Yes");
        //Добавляем пользователя
        addUserToBd(login, pass, name);
        //Добавляем в таблицу
        addToUsersTable(login, false, false);
        users_.push_back(User(login, pass, name));
        //Выводим запись в журнал
        addToLogGreen("User " + login + " added");
    }
}
