//Файл user.cpp
#include "user.h"

//Оператор копирования
User& User::operator =(const User &other) {
    if(this == &other){
        return *this;
    }

    login_ = other.login_;
    name_ = other.name_;

    return *this;
}

//Получить логин
const QString& User::getLogin() const {
    return login_;
}

//Получить имя
QString User::getName() const
{
    return name_;
}

//Проверка логина
bool User::checkLogin(const QString& login) const {
	return login_ == login;
}
