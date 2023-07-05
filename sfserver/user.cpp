//Файл user.cpp
#include "user.h"

//Оператор копирования присваивания
User& User::operator =(const User &other) {
    //Проверка на самоприсваивание
    if(this == &other){
        return *this;
    }
    //Присваиваем значения
    login_ = other.login_;
    password_ = other.password_;
    name_ = other.name_;
    isAuth_ = false;
    isClosed_ = false;
    isBanned_ = false;

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

//Проверка пароля
bool User::checkPassword(const QString &password) const {
	return password_ == password;
}

//Установить бит авторизации
void User::setAuth(){
   isAuth_ = true;
}

//Сбросить бит авторизации
void User::resetAuth(){
   isAuth_ = false;
}

//Получить статус авторизации
bool User::checkAuth() const{
   return isAuth_;
}

//Установить блокировку
void User::setBlock(){
    isClosed_ = true;
}

//Установить бан
void User::setBan(){
    isBanned_ = true;
}

//Сбросить блокировку
void User::resetBlock(){
    isClosed_ = false;
}

//Сбросить бан
void User::resetBan(){
    isBanned_ = false;
}

//Проверить статус бана
bool User::getBan() const{
    return isBanned_;
}

//Проверить статус блокировки
bool User::getBlock() const{
    return isClosed_;
}
