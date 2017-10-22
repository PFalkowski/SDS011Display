#include <SevenSeg.h>
#include <SDS011.h>

enum showMode
{
  pm25,
  pm10,
  humidity,
  errorCode  
};

showMode displayNow = showMode::pm25;

// seven segment display

SevenSeg disp(5, 9, A2, A4, A5, 6, A1);
const int numOfDigits=3;
int digitPins[numOfDigits]={4, 7, 8};
int errors = 0;

// SDS011
const int TxdPin = 3;
const int RxdPin = 2;
float p10, p25;
int error;

SDS011 my_sds;

void setup()
{
  
  // seven segment display setup

  disp.setDigitPins(numOfDigits,digitPins);
  disp.setTimer(2);
  disp.startTimer();
  disp.setDPPin(A3);
  
  // SDS011 setup

  my_sds.begin(TxdPin, RxdPin);
  Serial.begin(9600);
}

void loop()
{   
  error = my_sds.read(&p25,&p10);
  if (error)
  {
    if (errors < 10)   
      ++errors;
    else 
      displayNow = showMode::errorCode;
    Serial.println("Error. Pm 2.5: " + String(p25) + " Pm 10:  "+ String(p10));
  }
  else
  {
    errors = 0;
    Serial.println("P2.5: " + String(p25));
    Serial.println("P10:  " + String(p10));
  }
  if (p25 >= 999)
  {
    p25 = 999;
  }
  if (p10 >= 999)
  {
    p10 = 999;
  }
  switch(displayNow)
  {
    case showMode::pm25:
      disp.write("p2.5");
      delay(1000);
      disp.write(p25);
      displayNow = showMode::pm10;
    break;
    case showMode::pm10:
      disp.write("p10");
      delay(1000);
      disp.write(p10);
      displayNow = showMode::pm25;
    break;
    case showMode::errorCode:
      disp.write(String("ERR"));
      displayNow = showMode::pm25;
    break;      
  }  
  delay(2000);
}

ISR(TIMER2_COMPA_vect)
{
  disp.interruptAction();
}
