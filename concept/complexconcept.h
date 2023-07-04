//Файл complexconcept.h
#pragma once

#include <string>

//Описываем концепт
template<typename T>
concept ComplexConcept = requires(const T v){
   //Метод hash(), возвращает тип конвертируемый в long 
   { v.hash() }
   ->std::convertible_to<long>;
   
   //Метод toString(), возвращает std::string
   { v.toString() }
   ->std::same_as<std::string>;
   
   //Отсутсвует виртуальный деструктор
} && !std::has_virtual_destructor_v<T>;
