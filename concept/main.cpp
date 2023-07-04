//Файл main.cpp
#include <iostream>
#include "complexconcept.h"
#include "myclass.h"
#include "func.h"

int main(){
   //Проверяем удовлетворяет ли наш класс созданному концепту
   std::cout << "MyClass satisfies: " <<  std::boolalpha << ComplexConcept<MyClass> << std::endl;
   
   //Создаем объект
   MyClass A(5);
   //Пробуем передать в функцию
   check(A);

   return 0;
}
