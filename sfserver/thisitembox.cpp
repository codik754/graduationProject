#include "thisitembox.h"

//Конструктор по умолчанию
ThisItemBox::ThisItemBox()
{

}

//Конструктор с параметрами
ThisItemBox::ThisItemBox(const QString &login, const int row) : login_(login), row_(row)
{
    //Добавляем пункты
    addItem(tr("No"));
    addItem(tr("Yes"));

    //Соединяем сигнал со слотом
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(emitedChangedSignal(const int)));
}

//Слот для обработки изменения значения
void ThisItemBox::emitedChangedSignal(int index)
{
    //Испускаем сигнал
    emit changeValue(index, login_, row_);
}
