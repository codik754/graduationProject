//Файл message.h
#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>

//Класс для сообщений
class Message {
   QString sendedFrom_;//от кого отправлено сообщение
   QString sendedTo_;  //кому отправлено сообщение
   QString text_;   //текст сообщения
   QString dateandtime_; //дата отправки сообщения
public:
   //Конструктор по умолчанию
   Message();
   //Конструктор с параметрами
   Message(const QString &from, const QString &to, const QString &text);
   //Конструктор с параметрами
   Message(const QString &from, const QString &to, const QString &text, const QString &dateandtime);

   //Конструктор копирования
   Message(const Message& other);

   //Оператор присваивания
   Message &operator= (const Message& other);

   //Деструктор
   ~Message() = default;


   //Узнать от кого отправлено сообщение
   const QString& getSendedFrom() const;
   //Узнать кому отправлено сообщение
   const QString& getSendedTo() const;
   //Получить текст сообщения
   const QString& getText() const;
   //Получить дату и время
   const QString& getDatetime() const;
   //Соответсвует ли поле кому отправлено сообщение переданной строке
   bool checkToSendedTo(const QString &login) const;
   //Соответсвует ли поле от кого переданной строке
   bool checkToSendeFrom(const QString &login) const;
};

#endif // MESSAGE_H
