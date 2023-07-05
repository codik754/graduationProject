#ifndef THISITEMBOX_H
#define THISITEMBOX_H

#include <QComboBox>
#include <QString>

class ThisItemBox : public QComboBox
{
    Q_OBJECT
public:
    ThisItemBox();
    ThisItemBox(const QString &login, const int row);

private:
    QString login_;//логин пользователя
    int row_;//строка в таблице

signals:
    //Слот для обработки изменения значения
    void changeValue(const int index, const QString &login, const int row);

private slots:
    void emitedChangedSignal(int index);//сигнал, который будет испускаться в слоте при изменении значения
};

#endif // THISITEMBOX_H
