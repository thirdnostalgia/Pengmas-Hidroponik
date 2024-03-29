#define VREF 3.3              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample point
#include <LiquidCrystal_I2C.h>
//#include<EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TdsSensorPin 27 //pin sensor tds
#define kiri 15 //tombol kiri
#define kanan  2 //tombol kanan
#define ok  4 //tombol ok
#define back 18 //tombol back
#define relay 12 //relay untuk motor nutrisi
#define oneWireBus 26 //sensor suhu

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

uint8_t arrow[8] = {0x00, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00};
String input; //logo panah

int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;
int target_tds;
int jeda;

float averageVoltage = 0;
float tdsValue = 0;

// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

int read_tds(){
  static unsigned long analogSampleTimepoint = millis();
  temperature = suhu(); //ambil data suhu dari sensor
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
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 4096.0;
      
      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0)); 
      float compensationCoefficient = 1.0+0.02*(temperature-25.0);
      //temperature compensation
      float compensationVoltage=averageVoltage/compensationCoefficient;
      
      //convert voltage value to tds value
      tdsValue=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;
      tdsValue = tdsValue*1.19;
      return tdsValue;
    }
  }
}

void menu(){ //Menu utama
  lcd.clear();
	lcd.setCursor(0, 0);
  lcd.print("HIDROPONIK  TRKB");
  lcd.setCursor(4, 3);
  lcd.print("PPM");
  lcd.setCursor(17, 3);
  lcd.print("PPM");
  lcd.setCursor(19, 2);
  lcd.print("S");

  if (jeda == 0 || target_tds == 0){
    lcd.setCursor(11, 3);
    lcd.print("      OFF");
    lcd.setCursor(19, 2);
    lcd.print(" ");
  }else if(jeda/10 < 1){ //1 digit
    lcd.setCursor(17, 2);
    lcd.print(jeda);
  }else if(jeda/10 < 10){ // 2 digit
    lcd.setCursor(16, 2);
    lcd.print(jeda);
  }else{ // 3 digit
    lcd.setCursor(15, 2);
    lcd.print(jeda);
  }

  if (target_tds == 1000 && jeda != 0){
    lcd.setCursor(12, 3);
    lcd.print(target_tds);
  }else if(target_tds > 0 && jeda != 0){
    lcd.setCursor(13, 3);
    lcd.print(target_tds);
  }

  lcd.setCursor(1, 1);
  lcd.print("SET"); 
  lcd.setCursor(12, 1);
  lcd.print("RESET");
  lcd.setCursor(0, 3);
  lcd.print(read_tds()); //Menampilkan tds saat ini

}

int i = 0; //tombol arah
int j = 0; //tombol ok
int b = 0; //tombol back

void layar(){
  int temp_target = target_tds;
  int temp_jeda = jeda;
  
  //menu utama
  if(j%2 == 0 && i%2 == 0 ){ //i = tombol arah , j = tombol ok
    awal :
    j = 0;
    menu();
    lcd.setCursor(11,1);
    lcd.print(" ");
    lcd.setCursor(0,1);
    lcd.write(0);
  }
  if(j%2 == 0 && i%2 != 0 ){
    menu();
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(11,1);
    lcd.write(0);
  }

  if(j%2 != 0 && i%2 == 0 ){  //submenu SET
   set_target :
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set Target: ");
    i = 0; //arah
    b = 0; //back
    j = 1; //ok
    
    while(j%2 != 0){ 
      if(b%2 != 0){ //jika tombol back ditekan
        target_tds = temp_target;
        i = 0;
        goto awal;
      }
      if(i >= 10){
        lcd.setCursor(12,0);
        lcd.print("1000");
        lcd.setCursor(16,0);
        lcd.print(" PPM"); 
        target_tds = 1000;
      }else if(i <= 0){
        lcd.setCursor(12,0);
        lcd.print("0");
        lcd.setCursor(13,0);
        lcd.print(" PPM   ");
        target_tds = 0;
      }else{
        lcd.setCursor(12,0);
        lcd.print(i*100);
        lcd.setCursor(15,0);
        lcd.print(" PPM "); 
        target_tds = i*100;
      }
    }

    lcd.setCursor(0,2);
    lcd.print("Set Jeda = "); 
    i = 0;
    j = 0;

    while(j%2 == 0){
      if(b%2 != 0){ //jika tombol back ditekan
        target_tds = temp_target;
        jeda = temp_jeda;
        goto set_target;
      }
      if(i <= 0){
        lcd.setCursor(11,2);
        lcd.print("00"); 
        jeda = 0;
      }else if(i <= 9){
        lcd.setCursor(11,2);
        lcd.print("0"); 
        lcd.setCursor(12,2);
        lcd.print(i); 
        jeda = i;
      }else{
        lcd.setCursor(11,2);
        lcd.print(i); 
        jeda = i;
      }

      lcd.setCursor(13,2);
      lcd.print(" S   ");
    }
  
    delay(50);
    i = 0;
    j = 0;
    nutrisi(target_tds,jeda); //jalankan motor nutrisi
  }

  if(j%2 != 0 && i%2 != 0){  //submenu RESET
    target_tds = 0;
    jeda = 0;
    i = 0;
    j = 0;
  }

}

void nutrisi(int target_tds, int jeda){
  lcd.clear();
  lcd.lcd.setCursor(3,2);
  lcd.print("On Progress...");
  if (target_tds != 0 && jeda != 0){
    int curent_tds = read_tds();
    while(curent_tds < target_tds){
      digitalWrite(relay,HIGH); 
      delay(1000); //nyemprot 1 detik
      digitalWrite(relay,LOW);
      delay(jeda*1000);
      curent_tds = read_tds();
    }  
  }
  
}

float suhu(){ //Baca Temperature
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  return temperatureC;
}

void setup() {
  pinMode(relay,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(kanan), Kanan, RISING);
  attachInterrupt(digitalPinToInterrupt(ok), enter, RISING);
  attachInterrupt(digitalPinToInterrupt(kiri), Kiri, RISING);
  attachInterrupt(digitalPinToInterrupt(back), Back, RISING);
  lcd.begin();
  Serial.begin(9600);
  lcd.createChar(0, arrow); //logo panah
  //EEPROM.begin(255);
  sensors.begin(); // sensor suhu
}

void loop() {
  layar();
  Serial.println(tdsValue);
  delay(50);
}

//Fungsi - fungsi Interrupt
void Kanan(){
   
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    i++; 
  }
  last_interrupt_time = interrupt_time;
}

void Kiri(){
   
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    i--; 
  }
  last_interrupt_time = interrupt_time;
}



void enter(){
   static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    j++; 
  }
  last_interrupt_time = interrupt_time;
}

void Back(){
 static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    b++; 
  }
  last_interrupt_time = interrupt_time;
}


