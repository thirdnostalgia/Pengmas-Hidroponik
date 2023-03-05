#include <LiquidCrystal_I2C.h>
#include<EEPROM.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
int i;

uint8_t arrow[8] = {0x00, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00};
String input;

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
  awal :
  lcd.clear();
	lcd.setCursor(0, 0);
    lcd.print("HIDROPONIK");
    lcd.setCursor(14, 0);
    lcd.print(EEPROM.read(0));
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.setCursor(1, 1);
  lcd.print("SET      STATUS");
  while(Serial.available() == 0){}
  
  input = Serial.readString();
  input.trim();
  
  if(input == "kanan" || input == "kiri"){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HIDROPONIK");
    lcd.setCursor(14, 0);
    lcd.print(EEPROM.read(0));
    lcd.setCursor(1, 1);
    lcd.print("SET      STATUS");
    lcd.setCursor(9, 1);
    lcd.write(0);
    while(Serial.available() == 0){}
    
    input = Serial.readString();
    input.trim();

    if(input == "ok"){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("STAUTUS");
      delay(2000);
      goto awal;
      
    }
    if(input == "kanan" || input == "kiri"){
      goto awal;
    }

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
    goto awal;
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
          goto awal;
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
