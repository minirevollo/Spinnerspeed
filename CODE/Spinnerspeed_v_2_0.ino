/*Spinnerspeedmesser von Andreas Reheis 03.06.2017
 * Drehzahlmesser für die fidgetspinner mit Menüfunktion.
 * Menü 1 (Anzeige A) = Anzahl der Arme einstellen,
 * Menü 2 (Anzeige SE) = Sensibilität einstellen,
 * Menü 3 (Anzeige =) = Testfunktion, langsam drehen, Anzeige und Indikator_LED beobachten
 * Menü 4 (Anzeige SP) = Geschwindigkeitstest
 * Menp 5 (Anzeige --) = Testdauer in Sekunden 
 * Bedienung: 
 * 1. In Menü 1 die Anzahl der Arme durch drücken der Set Taste anpassen
 * 2. Sensibilität einstellen (Anfang ist 5, möglich ist ein Wert zwischen 1 und 10)
 * 3. Test durchführen, Menü 3 anwählen und Set Taste drücken,Spinner über den Senosr halten und langsam drehen. 
 *    Zähler zählt mit den Verdunklungen durch einen Arm hoch.
 *    Notfalls die Sensibilität einstellen.
 * 4. Geschwindigkeitstest: Man dreht den Spinner an und hält ihn über den Photoresistor. Dann drückt man 
 *    den Set Botton und wartet 3 Sekunden bis die Rpm angezeigt werden.
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
 

#define LED_PIN 8 //Indikator Led, zeigt im Menü3 eine Verdunklung an
int sensorPin = 17; // 
int menuePin = 12;  //Menüauswahlbotton
int setPin = 11;   //Aktionspin im Menü
int menue = 0;     // zeigt an welcher Menüpunkt gewählt wurde
int menuestate = HIGH;   //
int setstate = LOW;
int setpinstate = HIGH;
int arm = 2;  // zählt die Anzahl der Spinnerarme
int sens = 5;   // Sensibilität, wird im Menü 2 zwischen 1 und 10 eingestellt
int sensstate = LOW;
int hysterese = 70;
int testdrehungen = 0;  //zählt die Sensorabdunklungen in der Testphase, nicht die Umdrehungen
int drehungen = 0; // zählt die Sensorabdekungen während des Geschwindigkeittest
long zeit = 0;  // zählt den Zeitverlauf während des Test
int dauer = 4; //Dauer des Test in sekunden
int rpm = 0;

 
  
// ------------------------ setup -----------------------------------

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay(); // Clear the buffer
 
  
   pinMode(LED_PIN, OUTPUT);
   digitalWrite(LED_PIN, LOW);
   pinMode(sensorPin, INPUT);
   pinMode(menuePin, INPUT);
   digitalWrite(menuePin, HIGH);
   pinMode(setPin, INPUT);
   digitalWrite(setPin, HIGH); 
   Serial.begin(9600);
}
// -------------------------- setup ende -------------------------------
// -------------------------- loop -------------------------------------
void loop() {
  menuewahl(); //menuebotton kontrollle
  set();       //setbotton kontrolle
  anzeigen();  // display ausgabe
  if(setstate == HIGH){   //Aktion ausführen nachdem der setbotton losgelassen wurde
    setstate = LOW;      // setbotton entspannen
    aktion();  // Aktion entsprechend der Menüauswahl ausführen
  }
}
// -----------------------loop ende -----------------------------

// ---------------------- menue --------------------------
void menuewahl(){
    if(digitalRead(menuePin) == LOW){
      menuestate = LOW;
    }
    if(digitalRead(menuePin) == HIGH && menuestate == LOW){
       delay(50); //entprellen
       menuestate = HIGH;
       menue++;
         if(menue > 4){
           menue = 1;
         }
     }
  }
// ------------------- menue ende --------------------------- 

// ------------------- set -----------------------------
void set(){
  if(digitalRead(setPin) == LOW){
      setpinstate = LOW;
      }
    if(digitalRead(setPin) == HIGH && setpinstate == LOW){
       delay(50); //entprellen
       setpinstate = HIGH;
       setstate = HIGH;
       }
}
// ------------------------ set ende --------------------------
// ------------------- universalanzeige -----------------------
void universalanzeige(){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Spinnerspeed");
    display.setCursor(0,8);
    display.print("Arme: ");
    display.println(arm);
    display.setCursor(50,8);
    display.print("Testdauer: ");
    display.println(dauer);
}



// ------------------ universalanzeige ende -------------------

// ------------------------- anzeigen ------------------------
void anzeigen(){
     switch (menue) {
      case 0: // Intro anzeigen
        display.setTextSize(1);
        display.setTextColor(BLACK, WHITE); // 'inverted' text
        display.setCursor(10,0);
        display.println("erfindergarden.de");
        display.setCursor(25,16);
        display.setTextColor(WHITE);
        display.println("Spinnerspeed");
        display.setCursor(48,24);
        display.println("v.2.0");
        display.display();

      
         break;
      case 1: // Anzahl der Arme anzeigen
        universalanzeige();
        display.println("Anzahl der Arme");
        display.println("waehlen");
        display.display();      
         break;
  /*    case 2://Sensibilität anzeigen
        universalanzeige();
        display.println("Sensibilität ");
        display.println("waehlen");
        display.display();   
*/
      
           break;
      case 3://Testdrehungen anzeigen
        universalanzeige();
        display.println("Testdrehungen ");
        display.println(testdrehungen);
        display.display(); 
        Serial.println(testdrehungen);
      
              break;
      case 4://RPM anzeigen
      universalanzeige();
        display.println("Drehungen");
        display.println("pro min ");

        display.setTextSize(2);
        display.setCursor(70,16);     
        display.print(rpm);
        display.display(); 
        display.setTextSize(1);
        display.display();
             
             break;
      case 2://Dauer anzeigen
        universalanzeige();
        display.println("Dauer  ");
        display.println("waehlen");
        display.display();     
        
            
            break; 
      }
}
// ------------------------ anzeige ende ----------------------------
// -------------------------- aktion ----------------------------------
void aktion(){
  switch (menue) {
      case 1:          // Anzahl der Arme verändern
           arm++;
           if(arm > 7){
            arm = 2;
           }
        break;
   /*   case 2:            //Sensibilität verändern
           sens++;
           if (sens > 9){
            sens = 1;
           }
        break;
        */
      case 3:          //Testdrehungen anzeigen   
           testdrehungen = 0;
           while(digitalRead(menuePin) == HIGH){
           zaehlen();
           anzeigen();          
           }
        break;
      case 4:          //RPM anzeigen
          drehungen = 0;
          rpm = 0;
          digitalWrite(LED_PIN,HIGH);
          anzeigen();
          zeit = millis();
          while (millis() - zeit < dauer*1000){
             zaehlen();
             }
          rpm = drehungen * 60 /dauer / arm;
          anzeigen();
          digitalWrite(LED_PIN,LOW); 
        break;
      case 2:          // Dauer verändern
           dauer++;
           if(dauer > 7){
            dauer = 2;
           }
        break;  
      }
}
// --------------------------- aktion ende -----------------------------------
// --------------------------- zaehlen --------------------------------------
void zaehlen(){
  
  // if(digitalRead(sensorPin)== LOW && sensstate == LOW){ //Digitalmodus mit Linefollower
  if(analogRead(sensorPin) < sens * 100 - hysterese && sensstate == LOW){ //Analogmodus mit Photosensor und Spannungsteilung
    sensstate = HIGH;
    digitalWrite(LED_PIN,LOW);
  }
  
  //  if(digitalRead(sensorPin) == HIGH && sensstate == HIGH){ 
  if(analogRead(sensorPin) > sens * 100  && sensstate == HIGH){         //Analogmodus mit Photosensor und Spannungsteilung
    testdrehungen++; 
    drehungen++;
    sensstate = LOW;
    digitalWrite(LED_PIN, HIGH);   
  }
  
  Serial.print(drehungen);
  Serial.print("__"); 
  Serial.println(rpm);
}
// --------------------------------- zaehlen ende -------------------------------   

