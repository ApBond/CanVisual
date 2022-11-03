#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->disconnectButton->setEnabled(false);
    wGraphic = new QCustomPlot();
    wGraphicSpeed = new QCustomPlot();
    ui->grid->addWidget(wGraphic,1,0,1,1);
    ui->grid2->addWidget(wGraphicSpeed,1,0,1,1);
    // создаем вектор для графика


        // Добавляем график на холст
        wGraphic -> addGraph ( wGraphic -> xAxis , wGraphic -> yAxis );
        wGraphic -> addGraph ( wGraphic -> xAxis , wGraphic -> yAxis );
        //wGraphic -> graph ( 0 )-> setData ( x1 , y1 ); // Устанавливаем координаты точек графика
        // Подписываем осы координат
        wGraphic -> xAxis -> setLabel ( "x" );
        wGraphic -> yAxis -> setLabel ( "y" );
        wGraphic -> graph (0)->setPen(QColor(Qt::red));
        // Устанавливаем максимальные и минимальные значения координат
        wGraphic -> xAxis -> setRange (0 , 200 );
        wGraphic -> yAxis -> setRange (-20 , 20 );

        // Отрисовываем содержание полотна
        wGraphic -> replot ();
        // Добавляем график на холст
        wGraphicSpeed -> addGraph ( wGraphicSpeed -> xAxis , wGraphicSpeed -> yAxis );
        wGraphicSpeed -> addGraph ( wGraphicSpeed -> xAxis , wGraphicSpeed -> yAxis );
        wGraphicSpeed -> graph (0)->setPen(QColor(Qt::red));
        wGraphicSpeed -> xAxis -> setLabel ( "t" );
        wGraphicSpeed -> yAxis -> setLabel ( "w" );
        wGraphicSpeed -> xAxis -> setRange (0 , 200 );
        wGraphicSpeed -> yAxis -> setRange (-200 , 200 );
        wGraphicSpeed -> replot ();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_connectButton_clicked()
{
    can = new CanBus(ui->COMPortNumber->value());
    QObject::connect(can,&CanBus::reciveDataCallback,this,&MainWindow::canReciveData,Qt::QueuedConnection);
    QObject::connect(can,&CanBus::reciveDriveState,this,&MainWindow::reciveDriveStateCallback,Qt::QueuedConnection);
    QObject::connect(can,&CanBus::errorCallback,this,&MainWindow::reciveError,Qt::QueuedConnection);
    if(can->getErrorState()!=0)
    {
        ui->textData->append("Ошибка подключения");
        delete can;
    }
    else
    {
        ui->textData->append("Устройство подключено");
        ui->disconnectButton->setEnabled(true);
        ui->connectButton->setEnabled(false);
    }

}


void MainWindow::on_disconnectButton_clicked()
{
    ui->disconnectButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->textData->append("Устройство отключено");
    delete can;
}


void MainWindow::on_startButton_clicked()
{
    char data[2];
    data[0]=0x1;
    data[1]=0x1;
    can->sendMsg(data,2,workCanId);
}



void MainWindow::on_stopButton_clicked()
{
    char data[2];
    data[0]=0x1;
    data[1]=0x0;
    can->sendMsg(data,2,workCanId);
}

void MainWindow::canReciveData(int16_t data1,int16_t data2,int16_t currentA,int16_t currentC,uint16_t ID)
{
    if(ID==workCanId)
    {
        static QVector <double> x,y1,y2,speedV,Uq;
        static double t=0;
        ui->speedIndicator->display((double)data1);
        ui->currentIndicator1->display((double)currentA/10000);
        ui->currentIndicator2->display((double)currentC/10000);
        y1.push_back((double)currentA/1000);
        y2.push_back((double)currentC/1000);
        speedV.push_back((double)data1);
        Uq.push_back((double)data2/1000);
        x.push_back(t);
        t++;
        wGraphic->graph(0)->setData(x,y1);
        wGraphic->graph(1)->setData(x,y2);
        wGraphicSpeed->graph(0)->setData(x,speedV);
        //wGraphicSpeed->graph(1)->setData(x,Uq);
        wGraphicSpeed -> replot ();
        wGraphic -> replot ();
        if(t==200)
        {
            t=0;
            x.clear();
            y1.clear();
            y2.clear();
            Uq.clear();
            speedV.clear();
        }
    }
}

void MainWindow::reciveDriveStateCallback(uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4,uint16_t ID)
{
    if(ID==workCanId*10)
    {
        ui->motorState->display((int)data2);
        ui->errorState->display((int)data1);
    }
}

void  MainWindow::reciveError(uint8_t error,uint8_t id)
{
    ui->errorDrive->display((static_cast<int>(id)));
    ui->errorNumber->display((static_cast<int>(error)));
}

void MainWindow::on_pushButton_clicked()
{
    int16_t speed;
    speed=ui->referenceSpeed->value();
    if(ui->reverse->isChecked()) speed*=-1;
    char data[3];
    data[0] = 0x03;
    data[1] = speed & 0xFF;
    data[2] = speed >> 8;
    can->sendMsg(data,3,workCanId);
}


void MainWindow::on_canIdApply_clicked()
{
    workCanId=ui->canId->value();
}


void MainWindow::on_applyAngle_clicked()
{
    int16_t angle;
    angle=ui->referenceSpeed_2->value();
    char data[3];
    data[0] = 0x05;
    data[1] = angle & 0xFF;
    data[2] = angle >> 8;
    can->sendMsg(data,3,workCanId);
}

