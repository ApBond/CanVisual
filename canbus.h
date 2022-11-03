#ifndef CANBUS_H
#define CANBUS_H
#include <Windows.h>
#include "VCI_CAN.h"
#include <QThread>
#include <QDebug>
#include <QWidget>
#include <QTextBrowser>
#include <QObject>

class CanBus : public QThread
{
    Q_OBJECT
private:
BYTE devPort;
BYTE devType;
DWORD CAN1_Baud;
DWORD CAN2_Baud;
char errorState;
signals:
    void reciveDataCallback(int16_t data1,int16_t data2,int16_t currentA,int16_t currentC,uint16_t ID);
    void reciveDriveState(uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4,uint16_t ID);
    void errorCallback(uint8_t error,uint8_t id);
public:
    CanBus(BYTE port=0,BYTE dev=I7565H2,DWORD baud1=CANBaud_250K,DWORD baud2=CANBaud_250K) :
        devPort(port), devType(dev), CAN1_Baud(baud1), CAN2_Baud(baud2)
    {
        errorState=VCI_OpenCAN_NoStruct(devPort,devType,CAN1_Baud,CAN2_Baud);
        errorState=VCI_Set_CANFID_AllPass(CAN1);
        this->start();
    }

    ~CanBus()
    {
        this->terminate();
        VCI_CloseCAN(devPort);
    }

    char getErrorState(void){return errorState;}

    void sendMsg(char *data,char dataLen,int ID)
    {
        _VCI_CAN_MSG sendMsg;
        sendMsg.DLC=dataLen;
        sendMsg.RTR=0;
        sendMsg.ID=ID;
        sendMsg.Mode=0;
        memcpy(sendMsg.Data,data,dataLen);
        VCI_SendCANMsg(CAN1,&sendMsg);

    }

    void run()
    {
        while(1)
        {
            DWORD msgCount;
            _VCI_CAN_MSG reciveMsg;
            VCI_Get_RxMsgCnt(CAN1,&msgCount);
            if(msgCount>0)
            {
                VCI_RecvCANMsg(CAN1,&reciveMsg);
                if(reciveMsg.ID==3)
                    emit errorCallback(reciveMsg.Data[0],reciveMsg.Data[1]);
                if(reciveMsg.ID>=100 && reciveMsg.ID<=200)
                    emit reciveDataCallback((int16_t)((uint8_t)reciveMsg.Data[0]+((uint8_t)reciveMsg.Data[1]<<8)),(int16_t)((uint8_t)reciveMsg.Data[2]+((uint8_t)reciveMsg.Data[3]<<8)),(int16_t)(reciveMsg.Data[4]+(reciveMsg.Data[5]<<8)),(int16_t)(reciveMsg.Data[6]+(reciveMsg.Data[7]<<8)),reciveMsg.ID);
                if(reciveMsg.ID>=1000 && reciveMsg.ID<=2000)
                    emit reciveDriveState(reciveMsg.Data[0],reciveMsg.Data[1],0,0,reciveMsg.ID);
            }
        }
    }


};

#endif // CANBUS_H
