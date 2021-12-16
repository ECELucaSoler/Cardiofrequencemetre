#include <EEPROM.h>
#include <Arduino.h>
#include <FreqCount.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <TimeLib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define heure 12
#define minu 30
#define sec 12
#define UpperThreshold 900
#define LowerThreshold 750
#define B7 7




Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int temps = 69;
int etat= 0;
long potentiometre = 2;
float valeurpotentiometre;
int x = 0;
int y = 0;
int lastx = 0;
int lasty = 0;
int reading = 0;
float BPM = 0.0;
bool IgnoreReading = false;
bool FirstPulseDetected = false;
unsigned long FirstPulseTime = 0;
unsigned long SecondPulseTime = 0;
unsigned long PulseInterval = 0;
const int capteur = 0;
const int pinPotentiometre = 2;
const int ledRouge = 9;
const int ledJaune = 11;
const int ledVerte = 12;
const int pinBouton = 7;
const int beeper = 8;
int valeurPotentiometre = 0;
int signal;
unsigned long myTime;
unsigned int LastTime = 0;
const unsigned long eventInterval = 3000;
unsigned long previousTime = 0;







void affichageHeure() {
    display.writeFillRect(1,1,51,12, BLACK);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5,5);
    display.print(hour());
    display.print(":");
    display.print(minute());
    display.print(":");
    display.print(second());
    display.println("");
    display.display();
}


void reset (){
    setTime ( heure , minu , sec + 20 , 14 , 11 , 2021 );
}

void temporisteur(){
    time_t t = now ()+1;
    while (t != now()){

    }
}




void setup() {
    Serial.begin(1200);
    //FreqCount.begin(1000);
    pinMode(pinBouton, INPUT);
    pinMode(pinPotentiometre, INPUT);
    pinMode(ledVerte, OUTPUT);
    pinMode(ledJaune, OUTPUT);
    pinMode(ledRouge, OUTPUT);
    pinMode(beeper, OUTPUT);
    pinMode(B7,INPUT);
    digitalWrite(B7,HIGH);


    reset ();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    delay(2000);
    display.clearDisplay();



}


void loop() {
    digitalWrite(ledVerte, LOW);
    digitalWrite(ledJaune, LOW);
    digitalWrite(ledRouge, LOW);
    display.drawRect(0,0,128,15,WHITE);
    //Calcul du BPM
    reading = analogRead(0);

    if(reading > UpperThreshold && IgnoreReading == false){
        if(FirstPulseDetected == false){
            FirstPulseTime = millis();
            FirstPulseDetected = true;
        }
        else{
            SecondPulseTime = millis();
            PulseInterval = SecondPulseTime - FirstPulseTime;
            FirstPulseTime = SecondPulseTime;
        }

        IgnoreReading = true;

        if(reading < LowerThreshold){
            IgnoreReading = false;
        }


        //Serial.print(reading);
        //Serial.print("\n");
        //Serial.print(PulseInterval);
        //Serial.print("periode\n");
    }
    BPM = (1.0/PulseInterval) * 16.0 * 1000.0 * 5;

    if(signal < 800){
        BPM = 0;
        display.fillRoundRect(54,3,115, 10, 1, BLACK);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(64,4);
        display.print("BPM :");
        display.print((int)BPM);
        IgnoreReading = false;
    } else{
        IgnoreReading = false;
        // Fin de calcul BPM

        display.fillRoundRect(54,3,115, 10, 1, BLACK);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(64,4);
        display.print("BPM :");
        display.print((int)BPM);
    }
    if (BPM == 0){
        unsigned long currentTime = millis();
        if (currentTime - previousTime >= 3000) {
            digitalWrite(ledRouge, HIGH);
            previousTime = currentTime;
        }
        digitalWrite(ledRouge, HIGH);

    } // On choisit par rapport a un adult au repos 100 comme une valeur seuil de frequence cardiaque trop élevée.

    if (BPM < 100 && BPM > 60){
        unsigned long currentTime = millis();
        if (currentTime - previousTime >= 3000) {
            digitalWrite(ledVerte, LOW);
            previousTime = currentTime;
        }
        digitalWrite(ledVerte, HIGH);
    }// On choisit par rapport a un adult au repos un intervalle entre 60 à 100 bpm comme un intervalle seuil de frequence cardiaque normale.

    if (BPM <= 60 && BPM > 0){
        unsigned long currentTime = millis();
        if (currentTime - previousTime >= 3000) {
            digitalWrite(ledJaune, LOW);

            previousTime = currentTime;
        }
        digitalWrite(ledJaune, HIGH);
    }// On choisit par rapport a un adult au repos 60 comme une valeur seuil de frequence cardiaque trop peu élevée.




    //afficher courbe BPM
    if(x>127){
        x = 0;
        display.writeFillRect(0,25,128,64, BLACK);
        lastx = x;
    }

    signal = analogRead(0);
    Serial.print(signal);
    display.setTextColor(WHITE);
    y = (64 - (signal)/16) +40;
    display.writeLine(lastx,lasty,x,y,WHITE);
    lasty = y;
    lastx = x;
    x++;
    valeurPotentiometre = analogRead(pinPotentiometre);
    valeurPotentiometre = map(valeurPotentiometre,1,1020,0,5);
    x = x + valeurPotentiometre;
    display.display();
    //fin afficher courbe BPM

    //afficher heure
    temporisteur();
    affichageHeure();
    //fin afficher heure



    /* This is the event */
    if(BPM > 0){
        unsigned long currentTime = millis();
        if (currentTime - previousTime >= eventInterval) {
            tone(beeper, 1000, 500);
            previousTime = currentTime;
        }
    }


    int nb = EEPROM.read(0);
    etat = digitalRead(B7);
    if (etat == LOW){
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(5,5);
        display.print("consulter les ");
        display.setCursor(5,13);
        display.print("enregistrements ");
        display.setCursor(5,30);
        display.print("effacer la memoire");
        display.setCursor(5,50);
        display.print("new enregistrement");
        display.display();
        delay(100);
        int a=1;
        while(a){
            valeurpotentiometre = analogRead(pinPotentiometre);
            valeurpotentiometre = (valeurpotentiometre/1024) *64;
            if(valeurpotentiometre < 20){
                display.fillRoundRect(0,0,128,23,2,BLACK);
                display.setCursor(5,5);
                display.print("consulter les ");
                display.setCursor(5,13);
                display.print("enregistrements ");
                display.drawRect(1,1,125,22,WHITE);
                display.display();
                delay(200);
                display.fillRoundRect(0,0,128,23,2,BLACK);
                display.setCursor(5,5);
                display.print("consulter les ");
                display.setCursor(5,13);
                display.print("enregistrements ");
                display.display();
                delay(200);
                valeurpotentiometre = analogRead(pinPotentiometre);
                valeurpotentiometre = (valeurpotentiometre/1024) *64;
                etat = digitalRead(B7);
            }


            if((valeurpotentiometre > 20) && (valeurpotentiometre < 40)){
                display.fillRoundRect(0,23,128,23,2,BLACK);
                display.setCursor(5,30);
                display.print("effacer la memoire");
                display.drawRect(1,23,125,22,WHITE);
                display.display();
                delay(200);
                display.fillRoundRect(0,23,128,23,2,BLACK);
                display.setCursor(5,30);
                display.print("effacer la memoire");
                display.display();
                delay(200);
                valeurpotentiometre = analogRead(pinPotentiometre);
                valeurpotentiometre = (valeurpotentiometre/1024) *64;
                etat = digitalRead(B7);
            }
            if(valeurpotentiometre > 40){
                display.fillRoundRect(0,42,128,22,2,BLACK);
                display.setCursor(5,50);
                display.print("new enregistrement");
                display.drawRect(0,43,125,20,WHITE);
                display.display();
                delay(200);
                display.fillRoundRect(0,42,128,22,2,BLACK);
                display.setCursor(5,50);
                display.print("new enregistrement");
                display.display();
                delay(200);
                valeurpotentiometre = analogRead(pinPotentiometre);
                valeurpotentiometre = (valeurpotentiometre/1024) *64;
                etat = digitalRead(B7);
            }
            if (etat == LOW){
                display.clearDisplay();
                display.display();
                int b =1;
                delay(100);
                while (b){
                    etat = digitalRead(B7);
                    if(valeurpotentiometre < 20){
                        int compt = 0;
                        for (int i = 0 ; i< nb ; i++){
                            display.setCursor(5,10*compt);
                            display.print("BPM:");
                            display.print(EEPROM.read(i +1));
                            display.print(" time:");
                            display.print(EEPROM.read(i + 2));
                            display.print(":");
                            display.print(EEPROM.read(i + 3));
                            display.print(":");
                            display.print(EEPROM.read(i + 4));
                            display.display();
                            delay(100);
                            i = i + 3 ;
                            compt++;
                        }
                        delay(2000);
                    }
                    if((valeurpotentiometre > 20) && (valeurpotentiometre < 40)){
                        for (int i = 0 ; i< nb ; i++){
                            EEPROM.write(i, 0);
                        }
                        display.setCursor(5,5);
                        display.print("Memoire effacee");
                        display.display();
                        delay(2000);
                    }
                    if(valeurpotentiometre > 40){
                        reading = analogRead(0);

                        if(reading > UpperThreshold && IgnoreReading == false){
                            if(FirstPulseDetected == false){
                                FirstPulseTime = millis();
                                FirstPulseDetected = true;
                            }
                            else{
                                SecondPulseTime = millis();
                                PulseInterval = SecondPulseTime - FirstPulseTime;
                                FirstPulseTime = SecondPulseTime;
                            }

                            IgnoreReading = true;

                            if(reading < LowerThreshold){
                                IgnoreReading = false;
                            }


                            //Serial.print(reading);
                            //Serial.print("\n");
                            //Serial.print(PulseInterval);
                            //Serial.print("periode\n");
                        }
                        BPM = (1.0/PulseInterval) * 16.0 * 1000.0 * 5;
                        int h = hour();
                        int m = minute();
                        int s = second();
                        EEPROM.update(nb +1, BPM);
                        EEPROM.update(nb + 2, h);
                        EEPROM.update(nb + 3, m);
                        EEPROM.update(nb + 4,s);
                        display.setCursor(5,5);
                        display.print("donnee : ");
                        display.print(EEPROM.read(nb + 1));
                        display.print(" BPM");
                        display.setCursor(5,30);
                        display.print("heure : ");
                        display.print(EEPROM.read(nb + 2));
                        display.print(":");
                        display.print(EEPROM.read(nb + 3));
                        display.print(":");
                        display.print(EEPROM.read(nb + 4));
                        display.display();
                        delay(2000);
                        nb = EEPROM.read(0) + 4;
                        EEPROM.update(0, nb);

                    }

                    if (etat == LOW){
                        display.clearDisplay();
                        display.setCursor(5,5);
                        display.print("Merci!!!");
                        display.display();
                        delay(2000);
                        a = 0;
                        b = 0;
                    }
                }
            }
        }
    }
}