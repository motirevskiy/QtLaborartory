#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QElapsedTimer>
#include <QTableWidget>
#include "qcustomplot.h"
#include "axistag.h"
#include <QTextStream>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QElapsedTimer timer;
    QTableWidget *tableWidget;

    enum modulesId
    {
        ID_MODULE_ATM_PRESSURE=1,
        ID_MODULE_HUMIDITY,
        ID_MODULE_TIME,
        ID_MODULE_DIF_PRESSURE,
        ID_MODULE_INDUCTANCE,
        ID_MODULE_RADIATION,
        ID_MODULE_WEIGHT,
        ID_MODULE_LIGHT,
        ID_MODULE_FORCE,
        ID_MODULE_TEMP,
        ID_MODULE_RESISTANCE,
        ID_MODULE_CAPACITY,
        ID_MODULE_CURRENT_2MA,
        ID_MODULE_CURRENT_200MA,
        ID_MODULE_CURRENT_10A,
        ID_MODULE_CURRENT_1A_AC,
        ID_MODULE_VOLTAGE_200MV,
        ID_MODULE_VOLTAGE_30V,
        ID_MODULE_VOLTAGE_30V_AC,
        ID_MODULE_SPIROMETER,
        ID_MODULE_OXYGEN,
        ID_MODULE_NITRATES,
        ID_MODULE_CO_GAS,
        ID_MODULE_TEMP_FAST,
        ID_MODULE_ULTRAV
    };
    QString filename, serialString;

    float val[4]={0};
    QCustomPlot *customPlot;
    QCustomPlot *mPlot;
    QPointer<QCPGraph> mGraph1;
    QPointer<QCPGraph> mGraph2;
    QPointer<QCPGraph> mGraph3;
    QPointer<QCPGraph> mGraph4;
    AxisTag *mTag1;
    AxisTag *mTag2;
    AxisTag *mTag3;
    AxisTag *mTag4;
    QFile *file;
    QSerialPort     *serial1;
    QSerialPort     *serial2;
    QSerialPort     *serial3;
    QSerialPort     *serial4;
    QTextStream *txtstr;

    int indexRecButton=0;

    QStringList sensorsList;
    bool waitingForStartButton=0;
    bool stopped=1;
    bool paused=0;
    bool noteNeeded=0;
    int comPrevIndex;
    int id =0;
    float timeLast=0;
    float interruptOnTime=0;

    void clearTabAndGraph();
    void fillTabletLine(QStringList list);
    void lockControls(bool isStarted);

private slots:
    void timerSlot();
    void on_checkGraph1_stateChanged(int arg1);
    void on_checkGraph2_stateChanged(int arg1);
    void on_checkGraph3_stateChanged(int arg1);
    void on_checkGraph4_stateChanged(int arg1);

    void on_newFileButton_clicked();

    void on_startButton_clicked();
    void on_stopButton_clicked();

    void on_pushButton_clicked();

    void on_refreshCom_clicked();

    void getFromSerial();

    void on_addNoteButton_clicked();
};
#endif // MAINWINDOW_H
