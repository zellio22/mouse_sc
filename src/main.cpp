#include <Arduino.h>
#include <HID-Project.h>
#include <HID-Settings.h>


unsigned int largeur_ecran=2560/2;
unsigned int hauteur_ecran=1080/2;


//def de parametres IO
int inputX = A1; // entrée Ana X
int inputY = A0; // entrée Ana X
int click = 15; // entrée poussoir

// def des variable global

bool read_bp =false;
bool etat_debouce  = false;         // etat Bp apres anti rebond
bool old_etat_debouce  = false;          // Last etat Bp

bool basc_status = true;                // Etat bascule
bool last_basc_status = true;         // Last etat bascule


unsigned long lastrebondTime  = 0;
unsigned long rebondDelay  = 250;
int readX;           
int readY;
int dt = 18; // bande morte
unsigned long last_send = 0; // Derniere valeur de milis lors du send
int time_loop = 10; //time loops 10 ms For Run 250 ms For Debug
int coef = 20; // sencibilité

// debug
long debug = 0;
unsigned long lastdebug = 0;

int calc (int read){
    if(read > dt + 512){
        return round(-(read - (512 + dt))/coef);
    }

    if (512 - read > dt){
        return round((512 - dt - read)/coef);
    }

    if (read < 512 + dt && read > 512 - dt ){ // point mort
        return 0;
    }
    return 0;
}

void anti_rebond()
{
    read_bp = digitalRead(click);

    if ((millis() - lastrebondTime)>= rebondDelay && read_bp==LOW)
    {
        lastrebondTime=millis();

        basc_status= 1- basc_status;

    }
    
}

void absolut_mouse(){
    /* je taff dessu besoin de modifier la fonction calc
        pour ajouter un argumenent du coef absolu ou relatif 
        en abs je doit etre entre 0...32767
    
    */
    if (millis() >= last_send + time_loop){
        last_send = millis(); // rst de Millis()
        readX = analogRead(inputX);
        readY = analogRead(inputY);
        AbsoluteMouse.moveTo(256,256);

    }

}

void relative_mouse(){
  if (millis() >= last_send + time_loop)
  {
      last_send = millis(); // rst de Millis()
      readX = analogRead(inputX);
      readY = analogRead(inputY);

      Mouse.move(calc(readX), calc(readY)*-1);


      if (calc(readX)>24 || calc(readY)*-1>24) // debug
      {
          Serial.println(" ");
          Serial.print("Lecture Ana X: ");
          Serial.print(readX);
          Serial.print(" Y: ");
          Serial.println(readY);

          Serial.print("Resultat X: ");
          Serial.print(calc(readX));
          Serial.print(" Y: ");
          Serial.println(calc(readY));
          Serial.print("Etat BP : ");
          Serial.print(read_bp);
          Serial.print(" Etat Bascule : ");
          Serial.println(basc_status);
      }
  }
}


void setup ()
{
    Serial.begin(9600);
    Serial.println(F("System Start"));
    pinMode(click,INPUT_PULLUP);
    Mouse.begin();
    AbsoluteMouse.begin();
}

void loop ()
{
/*
  if (millis() >= lastdebug + 100) // debug qui fait bug la sourie 
  {
      lastdebug = millis();
      Serial.print("Etat BP : ");
      Serial.print(read);
      Serial.print(" Etat Bascule : ");
      Serial.println(basc_status);

  }*/

    if (basc_status==true){
        relative_mouse();
    }
    else{
        absolut_mouse();
    }
  //relative_mouse();
  anti_rebond();


}

