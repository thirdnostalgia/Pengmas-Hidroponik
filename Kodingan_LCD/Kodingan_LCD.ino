#include <LiquidCrystal_I2C.h>
#include<EEPROM.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t arrow[8] = {0x00, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00};
String input;

void menu(){
  lcd.clear();
	lcd.setCursor(0, 0);
  lcd.print("HIDROPONIK  TRKB");
  if (EEPROM.read(0) == 0){
    lcd.setCursor(17, 3);
    lcd.print("OFF");
  }else{
    lcd.setCursor(12, 3);
    lcd.print(" Minutes");
    lcd.setCursor(10, 3);
    lcd.print(EEPROM.read(0));
  }
  
  lcd.setCursor(1, 1);
  lcd.print("SET");
  lcd.setCursor(0, 2);
  lcd.write(0);
  lcd.setCursor(1, 2);
  lcd.print("Status TDS ");
  lcd.setCursor(0, 3);
  lcd.write(0);
  lcd.setCursor(1, 3);
  lcd.print("RESET");
}

void read_tds)(){
  static unsigned long analogSampleTimepoint = millis();
  if(millis()-analogSampleTimepoint > 40U){     //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == SCOUNT){ 
      analogBufferIndex = 0;
    }
  }       
  static unsigned long printTimepoint = millis();
    if(millis()-printTimepoint > 800U){
      printTimepoint = millis();
      for(copyIndex=0; copyIndex<SCOUNT; copyIndex++){
        analogBufferTemp[copyIndex] = analogBuffer[copyIndex];   
        // read the analog value more stable by the median filtering algorithm, and convert to voltage value
        averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0;
        //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0)); 
        float compensationCoefficient = 1.0+0.02*(temperature-25.0);
        //temperature compensation
        float compensationVoltage=averageVoltage/compensationCoefficient;
        //convert voltage value to tds value
        tdsValue=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;

        //Serial.print("voltage:");
        //Serial.print(averageVoltage,2);
        Serial.print("TDS Value:");
        Serial.print(1.2203*tdsValue,0);
        Serial.println("ppm");
      }
    }
}

void setup()
{
	// initialize the LCD
	lcd.begin();
  Serial.begin(9600);
  lcd.createChar(0, arrow); //logo panah
  EEPROM.begin(255);
}

void loop()
{
  satu :
  menu();
  lcd.setCursor(0, 1);
  lcd.write(0);
  while(Serial.available() == 0){}
  
  input = Serial.readString();
  input.trim();
  
  if(input == "kanan"){
    dua :
    menu();
    lcd.setCursor(0, 2);
    lcd.write(0);

    while(Serial.available() == 0){}
    input = Serial.readString();
    input.trim();

    if(input == "ok"){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("STAUTUS");
      delay(2000);
    //  goto awal;
      
    }
    if(input == "kanan"){
      menu();
      lcd.setCursor(0, 3);
      lcd.write(0);

      while(Serial.available() == 0){}
      input = Serial.readString();
      input.trim();
      if (input == "kiri"){
       goto dua; 
      }else if (input == "kanan"){
        goto satu;
      }
    }

  }

  if(input == "kiri"){
    menu();
    lcd.setCursor(0, 3);
    lcd.write(0);

    while(Serial.available() == 0){}
    input = Serial.readString();
    input.trim();
  }

  if(input == "ok"){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Durasi (detik) : ");
    lcd.setCursor(0, 1);
    lcd.print("0 Menit");
    int durasi= 0;

    set :
    lcd.setCursor(0, 1);
    lcd.print(durasi);
    while(Serial.available() == 0){}
    input = Serial.readString();
    input.trim();
    
    if (input == "kanan"){
        durasi++;
        goto set; 
    }else if (input == "kiri"){
        durasi--;
        goto set;
    }else if(input == "ok"){
        EEPROM.write(0, durasi);
        EEPROM.commit();
    }


    if (input == "kanan"){
      lcd.clear();
      Serial.println("dua");
      lcd.setCursor(0, 0);
      lcd.print("Durasi (detik) ");
      lcd.setCursor(1, 1);
      lcd.print("5     10     15");
      lcd.setCursor(6, 1);
      lcd.write(0);
      while(Serial.available() == 0){}
      
      input = Serial.readString();
      input.trim();
     
      if (input == "kanan"){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Durasi (detik) ");
        lcd.setCursor(1, 1);
        lcd.print("5     10     15");
        lcd.setCursor(13, 1);
        lcd.write(0);
        while(Serial.available() == 0){}

        input = Serial.readString();
        input.trim();

        if(input == "kanan"){
          lcd.clear();
          goto set;
        }
      }
    }
  }

  if(input == "ok"){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("NUTRISI ON DUTY ");
    delay(5000);
//    goto awal;
  }
  
  if(input == "back"){
    lcd.clear();
    //goto awal;
    lcd.setCursor(0, 0);
    for(byte i=0;i<10;i++){
        lcd.print(i);
        while(Serial.available() == 0){}

        input = Serial.readString();
        input.trim();
        lcd.clear();
        if(input == "stop"){
          EEPROM.write(1,i);
          EEPROM.commit();
          Serial.println(EEPROM.read(1));
     //     goto awal;
        }        
    }    
  }

  if(input == "gas"){
    
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(EEPROM.read(0));
      delay(2000);
    
    
  }

}
