#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define sensorPin 9
#define buttonPin1 6
#define buttonPin2 7
#define relayPin 8
#define potpin A4

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)

#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)

//Constants
const int rs = 12, en = 11, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

OneWire oneWire(sensorPin);
DallasTemperature Tempsensor(&oneWire);

bool pressed=false, pressed2=false, setupDone =false;
int mode=0, clockMode=0, timeMode=2, tempMode=0;
int hour=0, min=0 ,chronoHour=0,chronoMin=0, temp=30;

float SetTemperature()
{
  int read = analogRead(potpin);
  return map(read, 2, 1021, 30, 60);
}

int SetHours()
{
  int read = analogRead(potpin);
  return map(read, 2, 1021, 0, 12);
}
int SetMinutes()
{
  int read = analogRead(potpin);
  return map(read, 2, 1021, 0, 59);
}

void BoilerTurnOn(int mode){
    long val = millis() / 1000;
     int hours = numberOfHours(val);
     int minutes = numberOfMinutes(val);
  if(mode==1){
      if(hours-hour-chronoHour>=0 && minutes-min-chronoMin>=0){
        digitalWrite(relayPin, HIGH);
      }
  }else{
    if(hours>=hour && minutes>=min){
        digitalWrite(relayPin, HIGH);
      }
  }
}

void printDigits(byte digits){
  if(digits < 10){
    lcd.print('0');
  }
  lcd.print(digits,DEC);  
}

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  Tempsensor.begin();
  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin,LOW);
  
  pinMode(potpin, INPUT); 
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
}

void loop()
{
  Tempsensor.requestTemperatures();
  
  switch(mode){
    case 0:{
      float curTemp = Tempsensor.getTempCByIndex(0);
      
      lcd.setCursor(0, 0);
      if(curTemp>=-40){
        lcd.print("Current temp:");
        lcd.setCursor(0, 1);
        lcd.print(curTemp,1);
        lcd.print((char)223);
        lcd.print("C");
      }
      delay(200);
      break;
    }
    case 1:{
      if(tempMode==1){
        temp=SetTemperature();
      }
      lcd.setCursor(0, 0);
      lcd.print("Set temperatue:");
      lcd.setCursor(0, 1);
      lcd.print(temp);
      lcd.print((char)223);
      lcd.print("C");
      if(digitalRead(buttonPin2)==LOW && pressed2==false ){
        if(tempMode==0){
          tempMode++;
        }else{tempMode=0;}
        pressed2=true;
        lcd.clear();
        delay(100);
      }else if(pressed2==true){
        pressed2=false;
      }
      break;
    }
    case 2:{
      setupDone=false;
      lcd.setCursor(0, 0);
      lcd.print("Set time mode:");
      lcd.setCursor(0, 1);
      if(clockMode==0){
        lcd.print("Clock time");
      }else{
        lcd.print("Chrono");
        long val = millis() / 1000;
        int hours = numberOfHours(val);
        int minutes = numberOfMinutes(val);
        chronoHour=hours;
        chronoMin=minutes;
      }
      if(digitalRead(buttonPin2)==LOW && pressed2==false ){
        if(clockMode==0){
          clockMode++;
        }else{clockMode=0;}
        pressed2=true;
        lcd.clear();
        delay(100);
      }else if(pressed2==true){
        pressed2=false;
      }
      break;
    }
    case 3:{
      switch(timeMode){
        case 0:hour=SetHours();break;
        case 1:min = SetMinutes();break;
        case 2:break;
      }
      if(digitalRead(buttonPin2)==LOW && pressed2==false ){
        if(timeMode<2){
          timeMode++;
        }else{timeMode=0;}
        
        pressed2=true;
        lcd.clear();
        delay(100);
      }else if(pressed2==true){
        pressed2=false;
      }
      lcd.setCursor(0, 0);
      lcd.print("Set time:");
      lcd.setCursor(0, 1);
      printDigits(hour);
      lcd.print(":");
      printDigits(min);
      break;
    }
  }
  if(digitalRead(buttonPin1)==LOW && pressed==false ){
    if(mode<3){
      mode++;
    }else{
      mode=0;
      if(!setupDone){setupDone=true;}
    }
    pressed=true;
    lcd.clear();
    delay(100);
  }else if(pressed==true){
    pressed=false;
  }
  if(setupDone){
    if(Tempsensor.getTempCByIndex(0)>=temp){
      digitalWrite(relayPin,LOW);
    }else{
      BoilerTurnOn(clockMode);
    }
  }
}
