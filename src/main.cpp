#include <Arduino.h>
#include <HID-Project.h>
#include <HID-Settings.h>
#include "Joystick.h"

//def de parametres IO
int inputX = A1; // entrée Ana X
int inputY = A0; // entrée Ana X
int click = 15; // entrée poussoir

// def des variable global

bool read_bp =false;
bool basc_status = true;                // Etat bascule
bool last_basc_status = true;         // Last etat bascule

unsigned long lastrebondTime  = 0;
unsigned long rebondDelay  = 250;
int readX;           
int readY;
int dt = 18; // bande morte
unsigned long last_send = 0; // Derniere valeur de milis lors du send
int time_loop = 10; //time loops 10 ms For Run 250 ms For Debug
int coef_rel = 20; // sencibilité
int coef_abs =20;

// debug
long debug = 0;
unsigned long lastdebug = 0;

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
    1, 0,                  // Button Count, Hat Switch Count
    true, true, false,     // X and Y, but no Z Axis
    false, false, false,   // No Rx, Ry, or Rz
    false, false,          // No rudder or throttle
    false, false, false);  // No accelerator, brake, or steering

int calc (int read, int coef){
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

void anti_rebond(){

    read_bp = digitalRead(click);

    if ((millis() - lastrebondTime)>= rebondDelay && read_bp==LOW){

        lastrebondTime=millis();
        basc_status= 1- basc_status;

    }
    
}

void absolut_mouse(){
    /* Fonctionne pas avec SC 
    */
    if (millis() >= last_send + time_loop){

        last_send = millis(); // rst de Millis()
        readX = analogRead(inputX);
        readY = analogRead(inputY);
        int X = calc(readX,1)*coef_abs;
        int Y =  calc(readY,1)*-1*coef_abs;
        AbsoluteMouse.moveTo(X,Y);

    }

}

void relative_mouse(){
    if (millis() >= last_send + time_loop){

        last_send = millis(); // rst de Millis()
        readX = analogRead(inputX);
        readY = analogRead(inputY);
        Mouse.move(calc(readX,coef_rel), calc(readY,coef_rel)*-1);

  }
}

void mode_joystick(){
    //rien d'autre ?
    readX = analogRead(inputX);
    readY = analogRead(inputY);
    Joystick.setXAxis(readX);
    Joystick.setYAxis(readY);
}

void setup ()
{
    Serial.begin(9600);
    Serial.println(F("System Start"));
    pinMode(click,INPUT_PULLUP);
    Mouse.begin();
    AbsoluteMouse.begin();
    Joystick.begin();
    Joystick.setXAxisRange(0, 1023);
    Joystick.setYAxisRange(0, 1023);
}

void loop ()
{

    if (basc_status==true){
        relative_mouse();
    }
    else{
        absolut_mouse();
    }

  anti_rebond();


}

