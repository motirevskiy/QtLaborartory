#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Настройка экспорта");
    ui->horizontalSliderDPI->setMaximum(800);
    ui->horizontalSliderDPI->setMinimum(50);
    ui->horizontalSliderDPI->setValue(180);
    ui->horizontalSliderSize->setMaximum(100);
    ui->horizontalSliderSize->setMinimum(10);
    ui->horizontalSliderSize->setValue(100);

}

Dialog::~Dialog()
{
    delete ui;
}




void Dialog::on_buttonBox_accepted()
{
     size= (double)(ui->horizontalSliderSize->value()*0.01);
     height= ui->lineEditHeight->text().toInt();
     width = ui->lineEditWidth->text().toInt();
     dpi = ui->horizontalSliderDPI->value();
}


void Dialog::on_buttonBox_rejected()
{
    delete ui;
}

