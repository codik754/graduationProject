//Файл user.h
#ifndef USER_H
#define USER_H

#include <QString>

//Класс для пользователя
class User{
   QString login_;//поле для логина
   QString password_;//поле для пароля
   QString name_; //имя пользователя
   bool isAuth_;//пользователь авторизирован
   bool isClosed_;//пользователь заблокирован
   bool isBanned_;//пользователь забанен
public:
   //Конструктор по умолчанию
   User() : login_("UNKNOWN"), password_("UNKNOWN"), name_("UNKNOWN"), isAuth_(false), isClosed_(false), isBanned_(false) {}
   //Параметризированные конструкторы
   User(const QString& login, const QString& password, const QString &name)
      : login_(login), password_(password), name_(name),isAuth_(false), isClosed_(false), isBanned_(false) {}

   //Конструктор копирования
   User(const User &other) : login_(other.login_), password_(other.password_), name_(other.name_),isAuth_(false), isClosed_(false), isBanned_(false) {}

   //Оператор копирования присваивания
   User& operator =(const User &other);

   ~User() = default;//деструктор класса

   //Проверка логина
   bool checkLogin(const QString &login) const;

   //Проверка пароля
   bool checkPassword(const QString &password) const;

   //Получить логин
   const QString& getLogin() const;

   //Получить имя
   QString getName() const;

   //Установить бит авторизации
   void setAuth();

   //Установить блокировку
   void setBlock();

   //Установить бан
   void setBan();

   //Сбросить блокировку
   void resetBlock();

   //Сбросить бан
   void resetBan();

   //Проверить статус бана
    bool getBan() const;

    //Проверить статус блокировки
    bool getBlock() const;


   //Сбросить бит авторизации
   void resetAuth();

   //Получить статус авторизации
   bool checkAuth() const;
};

#endif // USER_H
