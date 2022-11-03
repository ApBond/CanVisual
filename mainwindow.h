#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "canbus.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void canReciveData(int16_t data1,int16_t data2,int16_t currentA,int16_t currentC,uint16_t ID);
    void reciveDriveStateCallback(uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4,uint16_t ID);
    void reciveError(uint8_t error,uint8_t id);
private slots:
    void on_connectButton_clicked();

    void on_disconnectButton_clicked();

    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_pushButton_clicked();

    void on_canIdApply_clicked();

    void on_applyAngle_clicked();

private:
    Ui::MainWindow *ui;
    CanBus *can;
    QCustomPlot *wGraphic;      // Объявляем объект QCustomPlot
    QCustomPlot *wGraphicSpeed;
    QCPCurve *verticalLine;     // Объявляем объект для вертикальной линии
    QCPItemTracer *tracer;      // Трасировщик по точкам графика
    uint16_t workCanId;
};
#endif // MAINWINDOW_H
