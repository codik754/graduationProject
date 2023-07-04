//Файл myclass.h
#pragma once

//Класс для проверки
class MyClass{
   int i_;
public:
   //Консструкторы
   MyClass() : i_(0) {}
   explicit MyClass(int i) : i_(i) {} 

   //Требуемые методы
   long hash() const {
      return static_cast<long>(i_);
   }
   
   std::string toString() const {
      return std::to_string(i_);
   }
   
   //Деструктор
   ~MyClass() = default;
};
