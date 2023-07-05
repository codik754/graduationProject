//Файл user.h
#ifndef USER_H
#define USER_H

#include <QString>

//Класс для пользователя
class User{
   QString login_;//поле для логина
   QString name_; //имя пользователя

public:
   //Конструктор по умолчанию
   User() : login_("UNKNOWN"), name_("UNKNOWN") {}
   //Параметризированные конструкторы
   User(const QString& login, const QString &name)
      : login_(login), name_(name) {}

   //Конструктор копирования
   User(const User &other) : login_(other.login_), name_(other.name_) {}

   //Оператор копирования
   User& operator =(const User &other);

   ~User() = default;//деструктор класса

   //Проверка логина
   bool checkLogin(const QString &login) const;

   //Получить логин
   const QString& getLogin() const;

   //Получить имя
   QString getName() const;
};

#endif // USER_H
