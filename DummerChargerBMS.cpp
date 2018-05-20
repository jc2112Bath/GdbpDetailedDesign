#include "mbed.h"
 
Ticker BMSticker1;
Ticker BMSticker2;
Ticker SoCSP1ticker;
Ticker TempSP1ticker;
Ticker Chargerticker;
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
DigitalOut Normal(p21);
DigitalOut Fast(p22);
DigitalOut OKError(p25);
DigitalOut ChargeStateError(p26);
DigitalOut VoltageError(p27);
DigitalIn OKSwitch(p5);
DigitalIn ChargeStateSwitch(p6);
DigitalIn VoltageSwitch(p7);
DigitalIn Charger(p8);
CAN can1(p9, p10);
CAN can2(p30, p29);
CANMessage BMSMsg;
CANMessage BMSOK;
CANMessage BMSSoCSP1;
CANMessage BMSTSP1;
CANMessage ChargerMsg;
int Voltage;
int Current;
 
void DummyBMSOK() {
    
    int OK;
    
    if(OKSwitch){
        OKError = 1;
        OK = 1;
    }
    else{
        OKError = 0;
        OK = 0;
    }
    
    BMSOK.format = CANExtended;// or  CANExtended;  // standard or extended ID (can be skipped for standard)
    BMSOK.id = 0x11111111;
    BMSOK.len = 8;//length in bytes (1 to 8);
    
    if (OK == 0){
        BMSOK.data[0] = 0;
        BMSOK.data[1] = 0;
        BMSOK.data[2] = 0;
        BMSOK.data[3] = 0;
        BMSOK.data[4] = 0;
        BMSOK.data[5] = 0;
        BMSOK.data[6] = 0;
        BMSOK.data[7] = 0;
    }
    else{
        BMSOK.data[0] = 255;
        BMSOK.data[1] = 255;
        BMSOK.data[2] = 255;
        BMSOK.data[3] = 255;
        BMSOK.data[4] = 255;
        BMSOK.data[5] = 255;
        BMSOK.data[6] = 255;
        BMSOK.data[7] = 255;
    }

    if (can1.write(BMSOK)){
    led1 = !led1;
    }
};

void DummyBMSSoCSP1(){
    
    BMSSoCSP1.format = CANExtended;// or  CANExtended;  // standard or extended ID (can be skipped for standard)
    BMSSoCSP1.id = 0x150C5100;
    BMSSoCSP1.len = 8;//length in bytes (1 to 8);
    
    int s1 = 55;
    int s2 = 51;
    int s3 = 52;
    int s4 = 58;
    int s5 = 49;
    int s6 = 53;
    int s7 = 54;
    int s8 = 56;
    
    int s1h = s1 * 2;
    int s2h = s2 * 2;
    int s3h = s3 * 2;
    int s4h = s4 * 2;
    int s5h = s5 * 2;
    int s6h = s6 * 2;
    int s7h = s7 * 2;
    int s8h = s8 * 2;
    
    
    
    BMSSoCSP1.data[0] = s1h;
    BMSSoCSP1.data[1] = s2h;
    BMSSoCSP1.data[2] = s3h;
    BMSSoCSP1.data[3] = s4h;
    BMSSoCSP1.data[4] = s5h;
    BMSSoCSP1.data[5] = s6h;
    BMSSoCSP1.data[6] = s7h;
    BMSSoCSP1.data[7] = s8h;

    if (can1.write(BMSSoCSP1)){
    //led1 = !led1;
    }
};

void DummyBMSTempSP1() {
    
    BMSTSP1.format = CANExtended;// or  CANExtended;  // standard or extended ID (can be skipped for standard)
    BMSTSP1.id = 0x18075100;
    BMSTSP1.len = 8;//length in bytes (1 to 8);
    
    int s1 = 29;
    int s2 = 29;
    int s3 = 30;
    int s4 = 31;
    int s5 = 32;
    int s6 = 32;
    int s7 = 32;
    int s8 = 33;
    
    int s1h = s1 * 3;
    int s2h = s2 * 3;
    int s3h = s3 * 3;
    int s4h = s4 * 3;
    int s5h = s5 * 3;
    int s6h = s6 * 3;
    int s7h = s7 * 3;
    int s8h = s8 * 3;
    
    
    
    BMSTSP1.data[0] = s1h;
    BMSTSP1.data[1] = s2h;
    BMSTSP1.data[2] = s3h;
    BMSTSP1.data[3] = s4h;
    BMSTSP1.data[4] = s5h;
    BMSTSP1.data[5] = s6h;
    BMSTSP1.data[6] = s7h;
    BMSTSP1.data[7] = s8h;

    if (can1.write(BMSTSP1)){
    led2 = !led2;
    }
};

void DummyCharger(){
    
    int VoltageNew;
    int CurrentNew = 2.4;
    
    if(VoltageSwitch){
        VoltageError = 1;
        VoltageNew = 500;
    }
    else{
        VoltageError = 0;
        VoltageNew = 300;
    }
    
    int State_No_Batt = 0;
    int State_Over_Heat = 0;
    int d6 = 0x00;
    int d7 = 0x00;
    
    if (ChargeStateSwitch){
        ChargeStateError = 1;
        //State_No_Batt = 1;
        State_Over_Heat = 1;
    }
    else{
        ChargeStateError = 0;
        State_No_Batt = 0;
        State_Over_Heat = 0;
    }
    
    if (State_No_Batt == 1){
        d6 = 0x8A;
        d7 = 0x77;
        printf("No Battery\n\r");
    }
    else if (State_Over_Heat == 1){
        d6 = 0x08;
        d7 = 0x07;
    }
    else {
        d6 = 0x00;
        d7 = 0x00;
    };
    
    int DecV = VoltageNew * 10;
    int DecC = CurrentNew * 10;
    int Top8V = DecV >> 8;
    int Top8C = DecC >> 8;
    int Bot8var = 255;
    int Bot8V = Bot8var & DecV;
    int Bot8C = Bot8var & DecC;  
      
    ChargerMsg.format = CANExtended;// or  CANExtended;  // standard or extended ID (can be skipped for standard)
    if (Charger){
        ChargerMsg.id = 0x18FF50E5;
    }
    else{
        ChargerMsg.id = 0x18FF50E7;
    }
    ChargerMsg.len = 8;//length in bytes (1 to 8);
    ChargerMsg.data[0] = Top8V; // repeat for each byte.
    ChargerMsg.data[1] = Bot8V;
    ChargerMsg.data[2] = Top8C;
    ChargerMsg.data[3] = Bot8C;
    ChargerMsg.data[4] = 0;
    ChargerMsg.data[5] = 0;
    ChargerMsg.data[6] = d6;
    ChargerMsg.data[7] = d7;
    if (can2.write(ChargerMsg)){
    led3 = !led3;
    }
};

int main() {
    printf("main()\n\r");
    BMSticker1.attach(&DummyBMSSoC, 1);
    BMSticker2.attach(&DummyBMSOK, 0.5);
    SoCSP1ticker.attach(&DummyBMSSoCSP1, 1);
    TempSP1ticker.attach(&DummyBMSTempSP1, 1);
    Chargerticker.attach(&DummyCharger, 1);
    CANMessage msg;
    while(1) {
        if (can1.read(msg)) {
            if (msg.id == 0x1806E5F4){
                int HexVoltageTop = msg.data[0];
                int HexVoltageBot = msg.data[1];
                int HexCurrentTop = msg.data[2];
                int HexCurrentBot = msg.data[3];
                
                
                int TopVoltage = HexVoltageTop << 8;
                Voltage = 0 | TopVoltage;
                Voltage = Voltage | HexVoltageBot;
                Voltage = Voltage/10;
                
                int TopCurrent = HexCurrentTop << 8;
                Current = 0 | TopCurrent;
                Current = Current | HexCurrentBot;
                Current = Current/10;
                printf("Charger Current: %d\n\r", Current);
                if (Current == 16){
                    Fast = 1;
                    Normal = 0;
                }
                else{
                    Fast = 0;
                    Normal = 1;
                }
                led4 = !led4;
            }

        }
    }
    
}
