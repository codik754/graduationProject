//Файл func.h
#pragma once

#include "complexconcept.h"

//Функция для проверки
void check(ComplexConcept auto &l){
   std::cout << std::endl;
   std::cout << "hash(): " << l.hash() << std::endl;
   std::cout << "toString(): " << l.toString() << std::endl;
}
