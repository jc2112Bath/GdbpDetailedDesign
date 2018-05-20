#include "mbed.h"
 
Ticker ticker;
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
DigitalOut Charger1(p21);
DigitalOut Charger2(p22);
DigitalOut Shutdown(p25);
DigitalIn Speed(p6);
DigitalIn Reset(p5);
CAN can1(p9, p10);
CANMessage ControlMsg;
CANMessage msg1;
int Voltage = 300;
int Current = 12;
int ChargerVoltage;
int ChargerCurrent;
int SoC;
int Temp;
    

void send2Charger() {
    
    if (Speed){
        Current = 16;
    }
    else{
        Current = 12;
    }
    
    int DecV = Voltage * 10;
    int DecC = Current * 10;
    int Top8V = DecV >> 8;
    int Top8C = DecC >> 8;
    int bot8var = 255;
    int Bot8V = bot8var & DecV;
    int Bot8C = bot8var & DecC;  
      
    ControlMsg.format = CANExtended;// or  CANExtended;  // standard or extended ID (can be skipped for standard)
    ControlMsg.id = 0x1806E5F4;
    ControlMsg.len = 4;//length in bytes (1 to 8);
    ControlMsg.data[0] = Top8V; // repeat for each byte.
    ControlMsg.data[1] = Bot8V;
    ControlMsg.data[2] = Top8C;
    ControlMsg.data[3] = Bot8C;
    ControlMsg.data[4] = 0;
    ControlMsg.data[5] = 0;
    ControlMsg.data[6] = 0;
    ControlMsg.data[7] = 0;
    if (can1.write(ControlMsg)){
    led1 = !led1;}
};

int main() {
    printf("main()\n\r");
    ticker.attach(&send2Charger, 0.5);
    CANMessage msg;
    while(1) {
        if (can1.read(msg)) {
            if (msg.id == 0x11111111){ //BMS OK
                int OK = msg.data[0] + msg.data[1] + msg.data[2] + msg.data[3] + msg.data[4] + msg.data[5] + msg.data[6] + msg.data[7];
                if (OK == 0){
                    printf("ERROR: BMS NOT OK!\n\r\n\r");
                    Shutdown = 1;
                }

            }
            if (msg.id == 0x150C5100){ //SoC Sub Pack 1
                int s1 = msg.data[0] / 2;
                int s2 = msg.data[1] / 2;
                int s3 = msg.data[2] / 2;
                int s4 = msg.data[3] / 2;
                int s5 = msg.data[4] / 2;
                int s6 = msg.data[5] / 2;
                int s7 = msg.data[6] / 2;
                int s8 = msg.data[7] / 2;
                printf("Sub Pack 1: String 1: SoC: %d\n\r", s1);
                printf("Sub Pack 1: String 2: SoC: %d\n\r", s2);
                printf("Sub Pack 1: String 3: SoC: %d\n\r", s3);
                printf("Sub Pack 1: String 4: SoC: %d\n\r", s4);
                printf("Sub Pack 1: String 5: SoC: %d\n\r", s5);
                printf("Sub Pack 1: String 6: SoC: %d\n\r", s6);
                printf("Sub Pack 1: String 7: SoC: %d\n\r", s7);
                printf("Sub Pack 1: String 8: SoC: %d\n\r", s8);
                led2 = !led2;

            }
            if (msg.id == 0x18075100){ //Temp Sub Pack 1
                
                int s1 = msg.data[0] / 3;
                int s2 = msg.data[1] / 3;
                int s3 = msg.data[2] / 3;
                int s4 = msg.data[3] / 3;
                int s5 = msg.data[4] / 3;
                int s6 = msg.data[5] / 3;
                int s7 = msg.data[6] / 3;
                int s8 = msg.data[7] / 3;
                printf("Sub Pack 1: String 1: Temp: %d\n\r", s1);
                printf("Sub Pack 1: String 2: Temp: %d\n\r", s2);
                printf("Sub Pack 1: String 3: Temp: %d\n\r", s3);
                printf("Sub Pack 1: String 4: Temp: %d\n\r", s4);
                printf("Sub Pack 1: String 5: Temp: %d\n\r", s5);
                printf("Sub Pack 1: String 6: Temp: %d\n\r", s6);
                printf("Sub Pack 1: String 7: Temp: %d\n\r", s7);
                printf("Sub Pack 1: String 8: Temp: %d\n\r", s8);
                led3 = !led3;
            }
            if (msg.id == 0x18FF50E5){
                int HexVoltageTop = msg.data[0];
                int HexVoltageBot = msg.data[1];
                int HexCurrentTop = msg.data[2];
                int HexCurrentBot = msg.data[3];
                int d6 = msg.data[6];
                int d7 = msg.data[7];
                Charger1 = 1;
                Charger2 = 0;
                
                
                switch(d6) {
                    case 138 : if (d7 == 119){
                                    printf("Charger Error: No Battery!\n\r");
                                    Shutdown = 1; 
                                }
                                break;
                    case 8 : if (d7 == 7){
                                    printf("Charger Error: Over Heating!\n\r");
                                    Shutdown = 1; 
                                }
                                break; 
                }
                               
                int TopVoltCharger = HexVoltageTop << 8;
                ChargerVoltage = 0 | TopVoltCharger;
                ChargerVoltage = ChargerVoltage | HexVoltageBot;
                ChargerVoltage = ChargerVoltage/10;
                
                int TopCurrentCharger = HexCurrentTop << 8;
                ChargerCurrent = 0 | TopCurrentCharger;
                ChargerCurrent = ChargerCurrent | HexCurrentBot;
                ChargerCurrent = ChargerCurrent/10;
                
                if (ChargerVoltage > Voltage){
                    printf("ERROR: Charger Voltage Too High! \n\r");
                    printf("Max Voltage: %d\n\r", Voltage);
                    printf("Charger Voltage: %d\n\r", ChargerVoltage); //This needs adapting as this can happen in constant current!!!
                    ticker.detach();
                    Shutdown = 1;
                }
                else if (ChargerCurrent > Current){
                    printf("ERROR: Charger Current Too High! \n\r");
                    printf("Max Current: %d\n\r", Current);
                    printf("Charger Current: %d\n\r", ChargerCurrent); //This needs adapting as this can happen in constant current!!!
                    ticker.detach();
                    Shutdown = 1;
                }
                else{
                    printf("Charger Voltage: %d\n\r", ChargerVoltage);
                    printf("Charger Current: %d\n\r", ChargerCurrent);
                    ticker.attach(&send2Charger, 0.5);
                    
                }
                
                led4 = !led4;
            }
            
            
            
            if (msg.id == 0x18FF50E7){
                int HexVoltageTop = msg.data[0];
                int HexVoltageBot = msg.data[1];
                int HexCurrentTop = msg.data[2];
                int HexCurrentBot = msg.data[3];
                int d6 = msg.data[6];
                int d7 = msg.data[7];
                Charger1 = 0;
                Charger2 = 1;
                
                switch(d6) {
                    case 138 : if (d7 == 119){
                                    printf("Charger Error: No Battery!\n\r");
                                    Shutdown = 1; 
                                }
                                break;
                    case 8 : if (d7 == 7){
                                    printf("Charger Error: Over Heating!\n\r");
                                    Shutdown = 1; 
                                }
                                break; 
                }
                               
                int TopVoltCharger = HexVoltageTop << 8;
                ChargerVoltage = 0 | TopVoltCharger;
                ChargerVoltage = ChargerVoltage | HexVoltageBot;
                ChargerVoltage = ChargerVoltage/10;
                
                int TopCurrentCharger = HexCurrentTop << 8;
                ChargerCurrent = 0 | TopCurrentCharger;
                ChargerCurrent = ChargerCurrent | HexCurrentBot;
                ChargerCurrent = ChargerCurrent/10;
                
                if (ChargerVoltage > Voltage){
                    printf("ERROR: Charger Voltage Too High! \n\r");
                    printf("Max Voltage: %d\n\r", Voltage);
                    printf("Charger Voltage: %d\n\r", ChargerVoltage); //This needs adapting as this can happen in constant current!!!
                    ticker.detach();
                    Shutdown = 1;
                }
                else if (ChargerCurrent > Current){
                    printf("ERROR: Charger Current Too High! \n\r");
                    printf("Max Current: %d\n\r", Current);
                    printf("Charger Current: %d\n\r", ChargerCurrent); //This needs adapting as this can happen in constant current!!!
                    ticker.detach();
                    Shutdown = 1;
                }
                else{
                    printf("Charger Voltage: %d\n\r", ChargerVoltage);
                    printf("Charger Current: %d\n\r", ChargerCurrent);
                    ticker.attach(&send2Charger, 0.5);
                }
                
                led4 = !led4;
            }
        if(Reset){
            ticker.attach(&send2Charger, 1);
            Shutdown = 0;
        }

    }
    
}
