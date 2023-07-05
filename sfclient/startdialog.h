#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>

namespace Ui {
class StartDialog;
}

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    ~StartDialog();

    //Установка надписи в поле
    void setErrorMessage(const QString &message);

    //Установить кнопку неактивной или активной
    void setButtonEnabled(bool b);

private slots:
    void on_enterButton_clicked();//cлот для проверки и отправки данных для входа

signals:
    //Отправка данных для проверки
    void sendData(const QString &login, const QString &password, const QString &host, const QString &port);

private:
    Ui::StartDialog *ui;

    void reject();//функция срабатывающая при закрытии окна
};

#endif // STARTDIALOG_H
