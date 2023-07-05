#include "startdialog.h"
#include "ui_startdialog.h"
#include <QString>

StartDialog::StartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartDialog)
{
    ui->setupUi(this);
    setFixedSize(437, 342);//делаем фиксированный размер окна
}

StartDialog::~StartDialog()
{
    delete ui;
}

//Установка надписи
void StartDialog::setErrorMessage(const QString &message)
{
    //Добавление надписи в пол
    ui->resultLabel->setStyleSheet("color: red;");
    ui->resultLabel->setText(message);
}

//Установить кнопку неактивной или активной
void StartDialog::setButtonEnabled(bool b)
{
    ui->enterButton->setEnabled(b);
}

//Слот для проверки и отправки данных для входа
void StartDialog::on_enterButton_clicked()
{
    //Устанавливаем границы полей в начальное значение
    ui->loginLineEdit->setStyleSheet("border:0px;");
    ui->loginLineEdit->setStyleSheet("border-bottom:1px solid black;");
    ui->passwordLineEdit->setStyleSheet("border:0px;");
    ui->passwordLineEdit->setStyleSheet("border-bottom:1px solid black;");
    ui->hostLineEdit->setStyleSheet("border:0px;");
    ui->hostLineEdit->setStyleSheet("border-bottom:1px solid black;");
    ui->portLineEdit->setStyleSheet("border:0px;");
    ui->portLineEdit->setStyleSheet("border-bottom:1px solid black;");

    //Очищаем поле с сообщением
    ui->resultLabel->setText("");

    //Считываем значения
    QString login = ui->loginLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString host = ui->hostLineEdit->text();
    QString port = ui->portLineEdit->text();

    //Проверяем все ли поля заполнены
    if(login.isEmpty() || password.isEmpty() || host.isEmpty() || port.isEmpty()){


        //Если поле логина пустое
        if(login.isEmpty()){
            //Выделяем красным цветом поле
            ui->loginLineEdit->setStyleSheet("border:1px solid red;");
        }
        //Если поле пароля пустое
        if(password.isEmpty()){
            //Выделяем красным цветом поле
            ui->passwordLineEdit->setStyleSheet("border:1px solid red;");
        }
        //Если поле хоста пустое
        if(host.isEmpty()){
            //Выделяем красным цветом поле
            ui->hostLineEdit->setStyleSheet("border:1px solid red;");
        }
        //Если поле порта пустое
        if(port.isEmpty()){
            //Выделяем красным цветом поле
            ui->portLineEdit->setStyleSheet("border:1px solid red;");
        }

        ui->resultLabel->setStyleSheet("color: red;");
        ui->resultLabel->setText(tr("Некоторые поля пустые"));

        return;//выходим из слота
    }
    //Отправляем сигнал
    emit sendData(login, password, host, port);
}


//Функция срабатывающая при закрытии окна
void StartDialog::reject(){
    QDialog::reject();
    qApp->quit();
}

