//Файл message.cpp
#include "message.h"

//Конструктор по умолчанию
Message::Message() : sendedFrom_("UNKNOWN"), sendedTo_("UNKNOWN"), text_("UNKNOWN"), dateandtime_() {}

//Конструктор с параметрами
Message::Message(const QString &from, const QString &to, const QString &text) : sendedFrom_(from), sendedTo_(to), text_(text), dateandtime_() {}

//Конструктор с параметрами
Message::Message(const QString &from, const QString &to, const QString &text, const QString &dateandtime) : sendedFrom_(from), sendedTo_(to), text_(text), dateandtime_(dateandtime) {}

//Конструктор копирования
Message::Message(const Message& other) : sendedFrom_(other.sendedFrom_), sendedTo_(other.sendedTo_), text_(other.text_), dateandtime_(other.dateandtime_) {}

//Оператор присваивания
Message &Message::operator=(const Message &other)
{
    //Проверка на самоприсваивание
    if(this == &other){
        return *this;
    }

    //Присваиваем значения
    sendedFrom_ = other.sendedFrom_;
    sendedTo_ = other.sendedTo_;
    text_ = other.text_;
    dateandtime_ = other.dateandtime_;

    return *this;
}

//Узнать от кого отправлено сообщение
const QString& Message::getSendedFrom() const {
   return sendedFrom_;
}

//Узнать кому отправлено сообщение
const QString& Message::getSendedTo() const {
   return sendedTo_;
}

//Получить текст сообщения
const QString& Message::getText() const {
   return text_;
}

//Получить дату и время
const QString& Message::getDatetime() const {
   return dateandtime_;
}

//Соответсвует ли поле кому отправлено сообщение переданной строке
bool Message::checkToSendedTo(const QString &login) const {
    return sendedTo_ == login;
}

//Соответсвует ли поле от кого переданной строке
bool Message::checkToSendeFrom(const QString &login) const
{
    return sendedFrom_ == login;
}
