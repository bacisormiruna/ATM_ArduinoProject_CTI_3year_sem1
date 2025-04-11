#include <Servo.h>  
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  


int trigPin = 8;    
int echoPin = 6;   
int servoPin = 7;
long duration, dist;   
Servo servo;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Adresa LCD-ului (de obicei 0x27)

// Definirea pini pentru keypad
#define key1 2  //Pin 2 pentru tasta 1
#define key2 3  
#define key3 5  
#define key4 4  

String correctPIN = "1234";  
String enteredPIN = "";  
String enteredSuma = "";
int suma = 0;    

bool prevStateKey1 = true; // Starea anterioara a tastei 1
bool prevStateKey2 = true; 
bool prevStateKey3 = true; 
bool prevStateKey4 = true; 

long average;   
long aver[3];   


void setup() {
    Serial.begin(9600);
    
    pinMode(key1, INPUT_PULLUP); 
    pinMode(key2, INPUT_PULLUP);
    pinMode(key3, INPUT_PULLUP);
    pinMode(key4, INPUT_PULLUP);
    
    servo.attach(servoPin); // Atasează servo-ul la pinul corespunzător

    pinMode(trigPin, OUTPUT);  //setam pinii pentru senzor ultrasonic
    pinMode(echoPin, INPUT);

    lcd.init();         
    lcd.backlight();  // activează iluminarea de fundal pentru lcd
    lcd.setCursor(0,0);
    lcd.print("Bine ai venit!");
    delay(2000);
    lcd.clear();
}

void loop() {
    lcd.clear();
    lcd.print("Introduceti card: ");
    
    //asteptam introducerea cardului

    dist = 100; 
    while (dist > 5) {
      measure();
    }

    //facem o medie aritmetica intre 3 masurari
    //conditia de siguranta ca a fost introdus cardul
    for(int i = 0; i <= 2; i++) {   
          measure();               
          aver[i] = dist;            
          delay(10);               
    }
    dist = (aver[0] + aver[1] + aver[2]) / 3;   

    if (dist<5){
      //cardul a fost introdus
      lcd.clear();
      lcd.print("Introduceti PIN:");
        
      // citire PIN
      enteredPIN = "";
      while (enteredPIN.length() < 4){
          if (readKey(key1, prevStateKey1)) {
            enteredPIN += "1";
            lcd.setCursor(0, 1);
            lcd.print("*");
          }
          if (readKey(key2, prevStateKey2)) {
            enteredPIN += "2";
            lcd.setCursor(1, 1);
            lcd.print("*");
          }
          if (readKey(key3, prevStateKey3)) {
            enteredPIN += "3";
            lcd.setCursor(2, 1);
            lcd.print("*");
          }
          if (readKey(key4, prevStateKey4)) {
            enteredPIN += "4";
            lcd.setCursor(3, 1);
            lcd.print("*");
          }
        }

      // verificare PIN
      if(enteredPIN == correctPIN) {
        lcd.clear();
        lcd.print("PIN corect!");
        lcd.setCursor(0, 1);
        lcd.print("Retragere...");
        delay(2000);

        //citire suma
        enteredSuma = "";

        lcd.clear();
        lcd.print("Suma dorita: ");

        while (enteredSuma.length() < 1) {
          if (readKey(key1, prevStateKey1)) {
            enteredSuma += "1";
            lcd.setCursor(0, 1);
            lcd.print(enteredSuma);
          }
          if (readKey(key2, prevStateKey2)) {
            enteredSuma += "2";
            lcd.setCursor(0, 1);
            lcd.print(enteredSuma);
          }
          if (readKey(key3, prevStateKey3)) {
            enteredSuma += "3";
            lcd.setCursor(0, 1);
            lcd.print(enteredSuma);
          }
          if (readKey(key4, prevStateKey4)) {
            enteredSuma += "4";
            lcd.setCursor(0, 1);
            lcd.print(enteredSuma);
          }
        }

        suma = enteredSuma.toInt();

        //numar miscari servo = suma introdusa
        for (int i = 0; i < suma; i++) { 
          servo.write(110);  
          delay(1000);  
          servo.write(0);  
          delay(1000);   
        }

        servo.write(110);   
        lcd.clear();
        lcd.print("Retragere");
        lcd.setCursor(0, 1);
        lcd.print("cu succes!");
        delay(4000); 

        //asteapta scoaterea cardului
        lcd.clear();
        lcd.print("Scoateti card...");
        while(dist < 5){
            measure();
        }
        
        dist = 100; 
        delay(2000);

      }else {
          lcd.clear();
          lcd.print("PIN gresit!");
          lcd.setCursor(0, 1);
          lcd.print("Incercati iar: ");
          delay(3000);
      }


    }else{
      //daca card nu a fost introdus, adica distanta > 5
      lcd.clear();
      lcd.print("Introduceti card: ");
    }

  
    delay(2000);
}


//functia de masurare a distantei
void measure() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  dist = (duration / 2) / 29.1;    //obtinere distanta
}

//citire a starii unui buton cu debouncing și eliminarea citirilor false
bool readKey(int keyPin, bool &prevState) {
  bool currentState = digitalRead(keyPin); 
  
  // detecteaza doar trecerea de la HIGH la LOW (buton apasat)
  if (!currentState && prevState) {        
    delay(100); //debouncing
    currentState = digitalRead(keyPin); 
    if (!currentState) { // verifica dacă butonul ramane apasat
      prevState = false; 
      return true;       
    }
  }

  // actualizeaza starea pentru trecerea de la LOW la HIGH (buton eliberat)
  if (currentState && !prevState) {
    prevState = true;
  }
  
  return false; //nicio apasare
}