/*Spinnerspeedmesser von Andreas Reheis 03.06.2017
 * Drehzahlmesser für die fidgetspinner mit Menüfunktion.
 * Menü 1 = Anzahl der Arme einstellen,
 * Menü 2 = Sensibilität einstellen,
 * Menü 3 = Testfunktion, langsam drehen und Anzeige beobachten
 * Menü 4 = Geschwindigkeitstest
 * Bedienung: 
 * 1. In Menü 1 die Anzahl der Arme durch drücken der Set Taste anpassen
 * 2. Sensibilität einstellen (Anfang ist 5, möglich ist ein Wert zwischen 1 und 10)
 * 3. Test durchführen, Menü 3 anwählen und Set Taste drücken,Spinner über den Senosr halten und langsam drehen. 
 *    Zähler zählt mit den Verdunklungen durch einen Arm hoch.
 *    Notfalls die Sensibilität einstellen.
 * 4. Geschwindigkeitstest: Man dreht den Spinner an und hält ihn über den Photoresistor. Dann drückt man 
 *    den Set Botton und wartet 3 Sekunden bis die Rpm angezeigt werden.
 */
#include <TM1637Display.h>
int ledPin = 13;  //Indikator Led, zeigt im Menü3 eine Verdunklung an
int sensorPin = 10; // 
int menuePin = 11;  //Menüauswahlbotton
int setPin = 12;   //Aktionspin im Menü
int menue = 1;     // zeigt an welcher Menüpunkt gewählt wurde
int menuestate = HIGH;   //
int setstate = LOW;
int setpinstate = HIGH;
int arm = 2;  // zählt die Anzahl der Spinnerarme
int sens = 5;   // Sensibilität, wird im Menü 2 zwischen 1 und 10 eingestellt
int sensstate = LOW;
int testdrehungen = 0;  //zählt die Sensorabdunklungen in der Testphase, nicht die Umdrehungen
int drehungen = 0; // zählt die Sensorabdekungen während des Geschwindigkeittest
long zeit = 0;  // zählt den Zeitverlauf während des Test
int dauer = 3000; //Dauer des Test in millisekunden, 1000 = 1 sek
int rpm = 0;
const int CLK = 9; //Set the CLK pin connection to the display
const int DIO = 8; //Set the DIO pin connection to the display
TM1637Display display(CLK, DIO);  //set up the 4-Digit Display.
//  | -A- |     Die 7 Segmente
//  F     B
//  | -G- |
//  E     C
//  | -D- |
  
const uint8_t SEG_AR[] = {
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G, //A                                                  
  };
 const uint8_t SEG_SP[] = {
  SEG_A | SEG_B | SEG_D | SEG_E | SEG_G, //S
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G, //P
  }; 
 const uint8_t SEG_SE[] = {
  SEG_A | SEG_B | SEG_D | SEG_E | SEG_G, //S
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, //E
  };  
 const uint8_t SEG_X[] = {
  SEG_A | SEG_G , //=
  };  
  
// ------------------------ setup -----------------------------------

void setup() {
   display.setBrightness(0x0a);  //set the diplay to maximum brightness
   pinMode(ledPin, OUTPUT);
   digitalWrite(ledPin, HIGH);
   pinMode(sensorPin, INPUT);
   pinMode(menuePin, INPUT);
   digitalWrite(menuePin, HIGH);
   pinMode(setPin, INPUT);
   digitalWrite(setPin, HIGH); 
}
// -------------------------- setup ende -------------------------------
// -------------------------- loop -------------------------------------
void loop() {
   menuewahl(); //menuebotton kontrollle
   set();       //setbotton kontrolle
   anzeigen();  // display ausgabe
    if(setstate = HIGH){   //Aktion ausführen nachdem der setbotton losgelassen wurde
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
       delay(30); //entprellen
       !menuestate;
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
       delay(30); //entprellen
       setpinstate = HIGH;
       setstate = HIGH;
       }
}
// ------------------------ set ende --------------------------
// ------------------------- anzeigen ------------------------
void anzeigen(){
     switch (menue) {
      case 1: // Anzahl der Arme anzeigen
           display.showNumberDec(arm,false,2,0);   //Frequenz ausgeben, ab Position 0 anzeigen
           display.setSegments(SEG_AR,2,2);    //A ab Position 2 anzeigen (0, 1 , 2, 3)
        break;
      case 2://Sensibilität anzeigen
           display.showNumberDec(sens,false,2,0);   //Frequenz ausgeben, ab Position 0 anzeigen
           display.setSegments(SEG_SE,2,2);    //A ab Position 2 anzeigen (0, 1 , 2, 3)
        break;
      case 3://Testdrehungen anzeigen
           display.showNumberDec(testdrehungen,false,2,0);   //Frequenz ausgeben, ab Position 0 anzeigen
           display.setSegments(SEG_X,2,2);    //A ab Position 2 anzeigen (0, 1 , 2, 3)
        break;
      case 4://RPM anzeigen
           display.showNumberDec(rpm,false,2,0);   //Frequenz ausgeben, ab Position 0 anzeigen
           display.setSegments(SEG_SP,2,2);    //A ab Position 2 anzeigen (0, 1 , 2, 3)
        break;
      }
}
// ------------------------ anzeige ende ----------------------------
// -------------------------- aktion ----------------------------------
void aktion(){
  switch (menue) {
      case 1: // Anzahl der Arme verändern
           arm++;
           if(arm > 7){
            arm = 2;
           }
        break;
      case 2://Sensibilität verändern
           sens++;
           if (sens > 10){
            sens = 1;
           }
        break;
      case 3://Testdrehungen anzeigen   
           testdrehungen = 0;
           while(digitalRead(menuePin) == LOW){
            zaehlen();
            anzeigen();
           }
        break;
      case 4://RPM anzeigen
          drehungen = 0;
          zeit = millis();
          while (millis() - zeit < dauer){
             zaehlen();
             }
          rpm = drehungen * 20 / arm;
          anzeigen();
           
        break;
      }
  
}
// --------------------------- aktion ende -----------------------------------
// --------------------------- zaehlen --------------------------------------
void zaehlen(){
    if(analogRead(sensorPin) < (sens * 100 - 100) && sensstate == LOW){  //-100 als Hysterese
       sensstate = HIGH;
       digitalWrite(ledPin,HIGH);
       }
    if(analogRead(sensorPin) > sens *100 + 100 && sensstate == HIGH){ //+ 100 als Hysterese
      testdrehungen++; 
      drehungen ++;
      !sensstate; 
      digitalWrite(ledPin,LOW); 
    }
  }
 // --------------------------------- zaehlen ende -------------------------------   

