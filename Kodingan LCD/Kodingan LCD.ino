
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
	// initialize the LCD
	lcd.begin();
  Serial.begin(9600);

	
}

void loop()
{
	lcd.setCursor(0, 0);
  lcd.print("HIDROPONIK--TRKB");
  lcd.setCursor(0, 1);
  lcd.print("SET       STATUS");
  while(Serial.available() == 0){}
  
  String input = Serial.readString();
  input.trim();
  lcd.clear();

  if(input == "set"){
   lcd.setCursor(0, 0);
    lcd.print("jeda nutrisi (s)");
    lcd.setCursor(0, 1);
    lcd.print("5   10   15   20");
    while(Serial.available() == 0){}
    String input = Serial.readString();
    input.trim();
    lcd.clear();
    
    if(input == "5"){
      lcd.setCursor(0, 0);
      lcd.print("On Progres...");
       while(Serial.available() == 0){}
    }
  }
}
