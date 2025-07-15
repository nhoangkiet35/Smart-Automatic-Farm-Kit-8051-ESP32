#include <SoftwareSerial.h>

int soilPin = A0;
int value = 0;
int TBvalue = 0;

SoftwareSerial UART1(17, 16);

void setup(){
  Serial.begin(9600);
  UART1.begin(9600);
  delay(10);
  
  }

void loop(){
 
  value = 0;

  for(int i=0; i<10; i++){

    value += analogRead(soilPin); // đọc tín hiệu analog từ chân AO 10 lần
    delay(10);
  }
  TBvalue = value/10; // Tính TB giá trị 
  
  int percent = map(TBvalue, 0, 4095, 100, 0); // chuyển giá trị analog thành giá trị %
  int truePercent = 100 - percent; // tính giá trị % thực để chuyển từ khô qua ẩm

  Serial.printf("MOISTURE:%d\n", truePercent);
  UART1.printf("MOISTURE:%d\n", truePercent);
  delay(1000);

  }
