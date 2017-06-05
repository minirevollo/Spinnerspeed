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
#include <TM1637Display.h>
int ledPin = 13;  //Indikator Led, zeigt im Menü3 eine Verdunklung an
int sensorPin = 19; // 
int menuePin = 11;  //Menüauswahlbotton
int setPin = 12;   //Aktionspin im Menü
int menue = 1;     // zeigt an welcher Menüpunkt gewählt wurde
int menuestate = HIGH;   //
int setstate = LOW;
int setpinstate = HIGH;
int arm = 2;  // zählt die Anzahl der Spinnerarme
int sens = 5;   // Sensibilität, wird im Menü 2 zwischen 1 und 10 eingestellt
int sensstate = LOW;
int hysterese = 200;
int testdrehungen = 0;  //zählt die Sensorabdunklungen in der Testphase, nicht die Umdrehungen
int drehungen = 0; // zählt die Sensorabdekungen während des Geschwindigkeittest
long zeit = 0;  // zählt den Zeitverlauf während des Test
int dauer = 4; //Dauer des Test in sekunden
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
  SEG_D,                                          //_
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G, //A                                                  
  
  };
 const uint8_t SEG_SP[] = {
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G, //S
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G, //P
  }; 
 const uint8_t SEG_SE[] = {
  SEG_D,                                  //_
  SEG_A | SEG_C | SEG_D | SEG_F | SEG_G, //S
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G, //E
  };  
 const uint8_t SEG_X[] = {
  SEG_D , //=
  SEG_A | SEG_G | SEG_D,
  };  
  const uint8_t SEG_XX[] = {
  SEG_G , //-
  SEG_G , //-
  }; 
  
// ------------------------ setup -----------------------------------

void setup() {
   display.setBrightness(0x0a);  //set the diplay to maximum brightness
   pinMode(ledPin, OUTPUT);
   digitalWrite(ledPin, LOW);
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
         if(menue > 5){
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
// ------------------------- anzeigen ------------------------
void anzeigen(){
     switch (menue) {
      case 1: // Anzahl der Arme anzeigen
           display.showNumberDec(arm,false,2,0);   //Anzahl der Arme ausgeben ab Position 0 anzeigen (Anzahl der Stellen, Position)
           display.setSegments(SEG_AR,2,2);    //_A ab Position 2 anzeigen (0, 1 , 2, 3)
        break;
      case 2://Sensibilität anzeigen
           display.showNumberDec(sens,false,1,0);   //Sensibilität ausgeben, ab Position 0 anzeigen
           display.setSegments(SEG_SE,3,1);    //SE ab Position 2 anzeigen (0, 1 , 2, 3)
        break;
      case 3://Testdrehungen anzeigen
           display.showNumberDec(testdrehungen,false,2,0);   //Verdunklungen ausgeben, ab Position 0 anzeigen
           display.setSegments(SEG_X,2,2);    //_= ab Position 2 anzeigen (0, 1 , 2, 3)
        break;
      case 4://RPM anzeigen
           display.showNumberDec(rpm,false,2,0);   //rpm ausgeben, ab Position 0 anzeigen
           display.setSegments(SEG_SP,2,2);    //SP ab Position 2 anzeigen (0, 1 , 2, 3)
        break;
      case 5://Dauer anzeigen
           display.showNumberDec(dauer,false,2,0);   //Dauer ausgeben, ab Position 0 anzeigen
           display.setSegments(SEG_XX,2,2);    //-- ab Position 2 anzeigen (0, 1 , 2, 3)
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
      case 2:            //Sensibilität verändern
           sens++;
           if (sens > 9){
            sens = 1;
           }
        break;
      case 3:          //Testdrehungen anzeigen   
           testdrehungen = 0;
           while(digitalRead(menuePin) == HIGH){
           zaehlen();
           anzeigen();          
           }
        break;
      case 4:          //RPM anzeigen
          drehungen = 0;
          zeit = millis();
          while (millis() - zeit < dauer*1000){
             zaehlen();
             }
          rpm = (drehungen * 20) / arm;
          anzeigen();
          digitalWrite(ledPin,LOW); 
        break;
      case 5:          // Dauer verändern
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
  
    if(digitalRead(sensorPin)== LOW && sensstate == LOW){ //Digitalmodus mit Linefollower
 // if(analogRead(sensorPin) < sens *100 - hysterese && sensstate == LOW){ //Analogmodus mit Photosensor und Spannungsteilung
       sensstate = HIGH;
       digitalWrite(ledPin,LOW);
       }
       
    if(digitalRead(sensorPin) == HIGH && sensstate == HIGH){ 
 // if(analogRead(sensorPin) > sens * 100  && sensstate == HIGH){         //Analogmodus mit Photosensor und Spannungsteilung
      testdrehungen++; 
      drehungen ++;
      sensstate = LOW; 
      digitalWrite(ledPin,HIGH);
      Serial.print(analogRead(sensorPin));
      Serial.print("__"); 
      Serial.println(digitalRead(sensorPin));
    }
    
  }
 // --------------------------------- zaehlen ende -------------------------------   

