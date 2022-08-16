#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include<QDebug>
#include <QFile>
#include <QIODevice>
#include<QTextStream>
#include<QtSerialPort/QSerialPort>
#include<QtSerialPort/QSerialPortInfo>
#include<QList>
#include<QMessageBox>
#include "dialog.h"
#include "ui_dialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      mPlot(0),
      mTag1(0),
      mTag2(0),
      mTag3(0),
      mTag4(0)
{
    ui->setupUi(this);
    this->setWindowTitle("Цифровая лаборатория");
    mPlot = new QCustomPlot;
    file =  new QFile;
    txtstr = new QTextStream;
    serial1 = new QSerialPort(this);
    serial2 = new QSerialPort(this);
    serial3 = new QSerialPort(this);
    serial4 = new QSerialPort(this);
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setRowCount(0);

    ui->widget->yAxis->setTickLabels(false);
    connect(ui->widget->yAxis2, SIGNAL(rangeChanged(QCPRange)), ui->widget->yAxis, SLOT(setRange(QCPRange))); // left axis only mirrors inner right axis
    ui->widget->yAxis2->setVisible(true);


    ui->widget->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(55); // add some padding to have space for tags
    ui->widget->axisRect()->addAxis(QCPAxis::atRight);
    ui->widget->axisRect()->axis(QCPAxis::atRight, 1)->setPadding(55); // add some padding to have space for tags
    ui->widget->axisRect()->addAxis(QCPAxis::atRight);
    ui->widget->axisRect()->axis(QCPAxis::atRight, 2)->setPadding(55); // add some padding to have space for tags
    ui->widget->axisRect()->addAxis(QCPAxis::atRight);
    ui->widget->axisRect()->axis(QCPAxis::atRight, 3)->setPadding(55); // add some padding to have space for tags
    // create graphs:

    clearTabAndGraph();

    // create tags with newly introduced AxisTag class (see axistag.h/.cpp):
    mTag1 = new AxisTag(mGraph1->valueAxis());
    mTag1->setPen(mGraph1->pen());
    mTag2 = new AxisTag(mGraph2->valueAxis());
    mTag2->setPen(mGraph2->pen());
    mTag3 = new AxisTag(mGraph3->valueAxis());
    mTag3->setPen(mGraph3->pen());
    mTag4 = new AxisTag(mGraph4->valueAxis());
    mTag4->setPen(mGraph4->pen());

    connect(serial1, SIGNAL(readyRead()), this, SLOT(getFromSerial()));
    connect(serial2, SIGNAL(readyRead()), this, SLOT(getFromSerial()));
    connect(serial3, SIGNAL(readyRead()), this, SLOT(getFromSerial()));
    connect(serial4, SIGNAL(readyRead()), this, SLOT(getFromSerial()));
    on_refreshCom_clicked();

    ui->Value1->setStyleSheet("color: #fa7800");
    ui->Value2->setStyleSheet("color: #00b43c");
    ui->Value3->setStyleSheet("color: #64003c");
    ui->Value4->setStyleSheet("color: #2d64ea");
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::getFromSerial()
{
    QSerialPort *serial=serial1;  // нужно узнать от какого сериал пришел сигнал
    if(serial1->isReadable())serial = serial1;
    if(serial2->isReadable())serial = serial2;
    if(serial3->isReadable())serial = serial3;
    if(serial4->isReadable())serial = serial4;
    serialString = serial->readLine();
    serial->clear();

    qDebug() << serialString;
    QString moduleName;
    QString units;
    float value=0;
    int input=0;
    QComboBox *cb; // указатель на комбо бокс
    QLabel *l;
    bool ok=0;

    id = (serialString.section("#",1,1)).toInt(&ok,10); // take the module ID from string
    if(!ok) return;
    if (id>100)
    {
        input = id%100;
        id = id - (input*100);
    }

    value = serialString.section("#",2,2).toFloat(&ok);
    if(!ok){value=0;}

    switch (id)
    {
    case ID_MODULE_ATM_PRESSURE:
        moduleName = "Атмосферное давление";
        units = "кПа";
        break;
    case ID_MODULE_HUMIDITY:
        moduleName = "Влажность";
        units = "%";
        break;
    case ID_MODULE_TIME:
        moduleName = "Секундомер";
        units = "с";
        break;
    case ID_MODULE_DIF_PRESSURE:
        moduleName = "Давление дифферренциальное";
        units = "кПа";
        break;
    case ID_MODULE_INDUCTANCE:
        moduleName = "Индуктивность";
        units = "мГ";
        if (value>=1000) {value/=1000; units ="Г";}
        break;
    case ID_MODULE_RADIATION:
        moduleName = "Радиация";
        units = "мкр/час";
        break;
    case ID_MODULE_WEIGHT:
        moduleName = "Масса";
        units = "г";
        break;
    case ID_MODULE_LIGHT:
        moduleName = "Освещение";
        units = "lux";
        break;
    case ID_MODULE_FORCE:
        moduleName = "Сила";
        units = "Н";
        break;
    case ID_MODULE_TEMP:
        moduleName = "Температура";
        units = "tC";
        break;
    case ID_MODULE_RESISTANCE:
        moduleName = "Сопротивление";
        units = "Ом";
        if(value>=1000) {value/=1000; units = "кОм";}
        break;
    case ID_MODULE_CAPACITY:
        moduleName = "Ёмкость";
        units = "нФ";
        if(value>=1000) {value/=1000; units = "мкФ";}
        break;
    case ID_MODULE_CURRENT_2MA:
        moduleName = "Ток 2мА";
        units = "мкА";
        break;
    case ID_MODULE_CURRENT_200MA:
        moduleName = "Ток 200мА";
        units = "мА";
        break;
    case ID_MODULE_CURRENT_10A:
        moduleName = "Ток 10А";
        units = "A";
        break;
    case ID_MODULE_CURRENT_1A_AC:
        moduleName = "Переменный ток 1А";
        units = "A";
        break;
    case ID_MODULE_VOLTAGE_200MV:
        moduleName = "Напряжение 200мВ";
        units = "mV";
        break;
    case ID_MODULE_VOLTAGE_30V:
        moduleName = "Напряжение 30В";
        units = "V";
        break;
    case ID_MODULE_VOLTAGE_30V_AC:
        moduleName = "Переменное напряжение 30В";
        units = "V";
        break;
    case ID_MODULE_SPIROMETER:
        moduleName = "Спирометр";
        units = "";
        break;
    case ID_MODULE_OXYGEN:
        moduleName = "Датчик кислорода";
        units = "%";
        break;
    case ID_MODULE_NITRATES:
        moduleName = "Датчик нитратов";
        units = "ммоль/литр";
        break;
    case ID_MODULE_CO_GAS:
        moduleName = "Датчик угарного газа";
        units = "ppm";
        break;
    case ID_MODULE_TEMP_FAST:
        moduleName = "Датчик температуры малоинерционный";
        units = "tC";
        break;
    case ID_MODULE_ULTRAV:
        moduleName = "Датчик ультрафиолета";
        units = "";
        break;
    default: moduleName = "";
        units = "";  break;
    }

    if(input>0)
    {
        moduleName = "вход "+QString::number(input)+" "+moduleName;
    }

    if(!ok) // if valueSection is not float
    {
        bool refreshComboBoxes=0;
        if (serialString.section("#",2,2)=="con")
        {
            if(id==ID_MODULE_FORCE||id==ID_MODULE_WEIGHT||id==ID_MODULE_DIF_PRESSURE)
            {
                for (int i =1; i<=3; i++)
                {
                    if(!sensorsList.contains("вход "+QString::number(i)+" "+moduleName))
                    {sensorsList.append("вход "+QString::number(i)+" "+moduleName);}
                    else return;
                }
            }
            else
            {
                if(!sensorsList.contains(moduleName)){sensorsList.append(moduleName);}
                else return;
            }
            refreshComboBoxes=1;
            if(id==ID_MODULE_TIME){ paused=1; timeLast=0;interruptOnTime=0;}

        }

        if (serialString.section("#",2,2)=="dis")  //disconnect
        {
            if(id==ID_MODULE_FORCE||id==ID_MODULE_WEIGHT||id==ID_MODULE_DIF_PRESSURE)
            {
                for (int i =1; i<=3; i++)
                {
                    if(sensorsList.contains("вход "+QString::number(i)+" "+moduleName))
                    {sensorsList.removeAll("вход "+QString::number(i)+" "+moduleName);}
                    else return;
                }
            }
            else
            {
                if(sensorsList.contains(moduleName)){sensorsList.removeAll(moduleName);}
                else return;
            }
            refreshComboBoxes=1;
            if(id==ID_MODULE_TIME) paused=0;
        }

        if(refreshComboBoxes)
        {
            for (int i=0;i<4;i++)
            {
                switch (i)
                {
                case 0 : cb = ui->combo1;
                    l = ui->Value1;
                    break;
                case 1 : cb = ui->combo2;
                    l = ui->Value2;
                    break;
                case 2 : cb = ui->combo3;
                    l = ui->Value3;
                    break;
                case 3 : cb = ui->combo4;
                    l = ui->Value4;
                    break;
                }
                cb->clear();
                l->clear();
                cb->addItem("...");
                cb->addItems(sensorsList);
            }

            return;  // go out from there
        }
    }


    if(id==ID_MODULE_TIME )
    {
        if(value != -1)
        {
            if(waitingForStartButton){timer.start();waitingForStartButton=0; }
            if(paused){timeLast=0; interruptOnTime=0;ui->timerStatus->setText("Секундомер запущен");}
            paused=0;
            interruptOnTime= value+timeLast;
            timeLast = interruptOnTime;
        }
        else {paused=1; ui->timerStatus->setText("Секундомер остановлен"); return;} // go out from there
    }



    for (int i=0;i<4;i++)
    {
        switch (i)
        {
        case 0 : cb = ui->combo1;
            l = ui->Value1;
            break;
        case 1 : cb = ui->combo2;
            l = ui->Value2;
            break;
        case 2 : cb = ui->combo3;
            l = ui->Value3;
            break;
        case 3 : cb = ui->combo4;
            l = ui->Value4;
            break;
        }
        if(cb->currentText()==moduleName)
        {
            val[i]=value;
            l->setText(QString::number(val[i],'f',3)+" "+units);
        }
    }

    if(!paused && !stopped)   { timerSlot(); }
}


void  MainWindow::timerSlot()
{    
    QPointer<QCPGraph> grph;
    QComboBox *cb;
    QStringList list;

    list.append(QString::number((float)timer.elapsed()/1000,'f',3));
    if(sensorsList.contains("Секундомер"))
    {
        if(id==ID_MODULE_TIME) {list.append(QString::number(interruptOnTime,'f',4).replace('.',','));}
        else{list.append("");}
    }

    for (int i=0;i<4;i++)
    {
        switch (i)
        {
        case 0 : cb = ui->combo1;
            grph = mGraph1;
            break;
        case 1 : cb = ui->combo2;
            grph = mGraph2;
            break;
        case 2 : cb = ui->combo3;
            grph = mGraph3;
            break;
        case 3 : cb = ui->combo4;
            grph = mGraph4;
            break;
        }
        if(cb->currentIndex()!=0)
        {
            list.append(QString::number(val[i],'f',4).replace('.',','));
            grph->addData((float)timer.elapsed()/1000,val[i]);
        }
    }

    fillTabletLine(list);

    // make key axis range scroll with the data:
    ui->widget->xAxis->rescale();
    mGraph1->rescaleValueAxis(false, true);
    mGraph2->rescaleValueAxis(false, true);
    mGraph3->rescaleValueAxis(false, true);
    mGraph4->rescaleValueAxis(false, true);


    ui->widget->xAxis->setRange(ui->widget->xAxis->range().upper, mGraph2->dataCount(), Qt::AlignRight);


    // update the vertical axis tag positions and texts to match the rightmost data point of the graphs:
    double graph1Value = mGraph1->dataMainValue(mGraph1->dataCount()-1);
    double graph2Value = mGraph2->dataMainValue(mGraph2->dataCount()-1);
    double graph3Value = mGraph3->dataMainValue(mGraph3->dataCount()-1);
    double graph4Value = mGraph4->dataMainValue(mGraph4->dataCount()-1);
    mTag1->updatePosition(graph1Value);
    mTag2->updatePosition(graph2Value);
    mTag3->updatePosition(graph3Value);
    mTag4->updatePosition(graph4Value);
    mTag1->setText(QString::number(graph1Value, 'f', 2));
    mTag2->setText(QString::number(graph2Value, 'f', 2));
    mTag3->setText(QString::number(graph3Value, 'f', 2));
    mTag4->setText(QString::number(graph4Value, 'f', 2));

    ui->widget->replot();

}


void MainWindow::on_checkGraph1_stateChanged(int arg1)
{

    ui->widget->graph(0)->setVisible(arg1);
    ui->widget->axisRect()->axis(QCPAxis::atRight, 0)->setVisible(ui->checkGraph1->checkState());
    ui->widget->replot();
}


void MainWindow::on_checkGraph2_stateChanged(int arg1)
{
    ui->widget->graph(1)->setVisible(arg1);
    ui->widget->axisRect()->axis(QCPAxis::atRight, 1)->setVisible(ui->checkGraph2->checkState());
    ui->widget->replot();

}


void MainWindow::on_checkGraph3_stateChanged(int arg1)
{
    ui->widget->graph(2)->setVisible(arg1);
    ui->widget->axisRect()->axis(QCPAxis::atRight, 2)->setVisible(ui->checkGraph3->checkState());
    ui->widget->replot();
}


void MainWindow::on_checkGraph4_stateChanged(int arg1)
{
    ui->widget->graph(3)->setVisible(arg1);
    ui->widget->axisRect()->axis(QCPAxis::atRight, 3)->setVisible(ui->checkGraph4->checkState());
    ui->widget->replot();
}



void MainWindow::on_addNoteButton_clicked()
{
    noteNeeded =1;
}






void MainWindow::on_newFileButton_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    filename = dialog.getSaveFileName(this, "Create New File","",tr("csv-file(*.csv)"));

    if(dialog.Accepted)
    {
        file->close();

        file->setFileName(filename);
        file->open(QIODevice::Append);
        txtstr->setDevice(file);
        clearTabAndGraph();
    }
}



void MainWindow::clearTabAndGraph()
{
    while(ui->tableWidget->rowCount()!=0)  ui->tableWidget->removeRow(0);

    ui->tableWidget->clear();


    ui->widget->clearGraphs();
    mGraph1 = ui->widget->addGraph(ui->widget->xAxis, ui->widget->axisRect()->axis(QCPAxis::atRight, 0));
    mGraph2 = ui->widget->addGraph(ui->widget->xAxis, ui->widget->axisRect()->axis(QCPAxis::atRight, 1));
    mGraph3 = ui->widget->addGraph(ui->widget->xAxis, ui->widget->axisRect()->axis(QCPAxis::atRight, 2));
    mGraph4 = ui->widget->addGraph(ui->widget->xAxis, ui->widget->axisRect()->axis(QCPAxis::atRight, 3));

    mGraph1->setPen(QPen(QColor(250, 120, 0)));
    mGraph2->setPen(QPen(QColor(0, 180, 60)));
    mGraph3->setPen(QPen(QColor(100, 0, 60)));
    mGraph4->setPen(QPen(QColor(45, 100, 234)));
    ui->widget->replot();

    ui->checkGraph1->setChecked(1);
    ui->checkGraph2->setChecked(1);
    ui->checkGraph3->setChecked(1);
    ui->checkGraph4->setChecked(1);
    ui->checkGraph1->setChecked(0);
    ui->checkGraph2->setChecked(0);
    ui->checkGraph3->setChecked(0);
    ui->checkGraph4->setChecked(0);

}


void MainWindow::on_startButton_clicked()
{    
    if (!file->isOpen())
    {
        QMessageBox::warning(this,"Файл не выбран","Создайте или откройте файл для сохранения результатов");
        on_newFileButton_clicked();
        return;
    }

    lockControls(1);

    QStringList list;
    QComboBox *cb;
    list.append("текущее время, с");
    ui->timerStatus->clear();
    if(sensorsList.contains("Секундомер"))
    {
        list.append("сигнал на фотодатчике ");
        paused =  1;
        waitingForStartButton = 1;
        ui->timerStatus->setText("Ожидается запуск секундомера");
    }
    else  waitingForStartButton = 0;

    for (int i=0;i<4;i++)
    {

        switch (i)
        {
        case 0 : cb = ui->combo1;
            break;
        case 1 : cb = ui->combo2;
            break;
        case 2 : cb = ui->combo3;
            break;
        case 3 : cb = ui->combo4;
            break;
        }
        if(cb->currentIndex()!=0) list.append(cb->currentText());
        else list.append("");
    }


    fillTabletLine(list);

    if(!waitingForStartButton)timer.start();
    stopped = 0;
}


void MainWindow::fillTabletLine(QStringList list)
{
    QString s;
    if(list.length()>ui->tableWidget->columnCount()){ui->tableWidget->setColumnCount(list.length());}

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    for(int i =0; i<list.length(); i++)
    {
        s.append(list[i]+";");
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,i, new QTableWidgetItem(list[i]));
    }

    if(noteNeeded)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0, new QTableWidgetItem(ui->lineEdit->text()));
        s.append(ui->lineEdit->text());
        noteNeeded=0;
    }

    s.append("\n");
    txtstr->operator<<(s);
}



void MainWindow::on_stopButton_clicked()
{
    file->close();
    stopped = 1;
    lockControls(0);
}

void MainWindow::lockControls(bool isStarted)
{

    ui->startButton->setDisabled(isStarted);
    ui->newFileButton->setDisabled(isStarted);
    ui->combo1->setDisabled(isStarted);
    ui->combo2->setDisabled(isStarted);
    ui->combo3->setDisabled(isStarted);
    ui->combo4->setDisabled(isStarted);
    ui->pushButton->setDisabled(isStarted);//export
    ui->refreshCom->setDisabled(isStarted);
}

void MainWindow::on_pushButton_clicked()//export
{

    QString filenameExport;
    QFileDialog fdialog;
    Dialog d;
    d.exec();

    fdialog.setFileMode(QFileDialog::AnyFile);
    filenameExport = fdialog.getSaveFileName(NULL, "Create New File","",tr("jpg(*.jpg)"));
    if(fdialog.Accepted)
    {
        ui->widget->saveJpg(filenameExport,d.width,d.height,d.size,-1,d.dpi);
    }
}


void MainWindow::on_refreshCom_clicked()
{
    QString s;
    QSerialPort *port;
    QList<QSerialPortInfo>  comList;
    comList = QSerialPortInfo::availablePorts();
    qDebug()<< comList.length();

    int portsConnected=0;

    for (int i=0; i<comList.length();i++)
    {
        s = comList.at(i).description();

        qDebug()<< s;
        if(s=="STMicroelectronics Virtual COM Port")
        {
            portsConnected++;
            if (portsConnected>4)break;
            switch (portsConnected)
            {
            case 1: port = serial1; break;
            case 2: port = serial2; break;
            case 3: port = serial3; break;
            case 4: port = serial4; break;
            }
            port->close();
            port->setPortName(comList.at(i).portName());
            port->setBaudRate(9600);
            port->setDataBits(QSerialPort::Data8);
            port->setFlowControl(QSerialPort::NoFlowControl);
            port->setStopBits(QSerialPort::OneStop);
            port->open(QIODevice::ReadOnly);
            port->reset();
        }
    }

    s = "Обнаружено устройств: "+QString::number(portsConnected);
    if(portsConnected == 0)s.append("\nпроверьте подключение и нажмите 'обновить'");
    ui->portStatusLabel->setText(s);
}





