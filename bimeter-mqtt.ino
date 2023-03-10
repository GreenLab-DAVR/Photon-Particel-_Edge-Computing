#include <time.h>
#include "math.h"
#include "stdio.h"
#include <MQTT.h>
SYSTEM_THREAD(ENABLED);
void callback(char* topic, byte* payload, unsigned int length);
const uint8_t server_TESIS[] = { 148, 213, 117, 98};
MQTT client(server_TESIS, 1883, callback);

void callback(char* topic, byte* payload, unsigned int length) {
   
}

int led1 = D5;  // The on-board LED RGB
int led2 = D6;  // The on-board LED RGB
int led3 = D7;  // The on-board LED RGB
int led7 = D7;
double kWh; // antes double kWh=0;
float Ph=0;
double PPROM=0;
float Pprom_b ;
float Dapar;
float DP;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
 
  
}

void loop() {
 //digitalWrite(led, HIGH);   // Turn ON the LED
float Van[399]={}; // Vector of Vphase a

float Ia[399]={}; //  Vector of Iphase a
float Pa[399]={}; //  Vector of Instanenous power
float Vrms_an=0; //rms of voltage
float Irms_a=0; //rms of current Pprom_a=0;
float Pprom_a=0;
float DP;
float PA;
float DS;
float FP;

for(int i =0; i<=399; i++){ // are taken 400 samples to compute the parameters

 // float sensorIa = (analogRead(A1));// reading the current of phase a
  float sensorIa = (analogRead(A2));// reading the current of phase a
  
  float sensorVan = (analogRead(A1));// reading the voltage of phase a

// Conversion to analog data (calibration):
  sensorIa=((sensorIa/4096)*3.3-1.65)*24.3;//27.62;
  sensorVan=((sensorVan/4096)*3.3-1.65)*817.70;//785;//827.95;
  
  /// aqui inicia el calculo de los valores rms de voltaje y corriente
  Van[i]=pow(sensorVan,2); //se almacena el cuadrado de cada muestra de voltaje
  Ia[i]=pow(sensorIa,2); // se almacena el cuadrado de cada muestra de corriente
  Pa[i]=sensorIa*sensorVan/400; // se almacena la potencia instantanea
  Vrms_an=Vrms_an+Van[i];// se acumulan los cuadrados de cada muestra de voltaje
  Irms_a=Irms_a+Ia[i];// se acumulan los cuadrado de cada muestra de corriente
  Pprom_a=Pprom_a+Pa[i];// se acumulan las muestras de potencia instanea

 if (client.isConnected())
        client.loop();

}


PPROM=Pprom_a;
float VRMS_an;
float IRMS_a;
//Pprom_a=Pprom_a*(2000/1000)*(1/3600);
//Pprom_a=((Pprom_a*(1000/1000)/3600)/1000);
Ph=((Pprom_a*(2)/3600)/1000);
//kWh=kWh+((Pprom_a*(5000/1000)/3600)/1000);
kWh=kWh+Ph;
//kWh=kWh+Pprom_a*(5/3600000);

VRMS_an=sqrt(Vrms_an/400);

IRMS_a=sqrt(Irms_a/400);
 

if (kWh>0 && kWh<=75)
{
digitalWrite(led1, HIGH);    // Turn OFF the LED
digitalWrite(led2, LOW);    // Turn OFF the LED
digitalWrite(led3, HIGH);    // Turn OFF the LED
}
else if (kWh>75 && kWh<=175)
{
digitalWrite(led1, HIGH);    // Turn OFF the LED
digitalWrite(led2, LOW);    // Turn OFF the LED
digitalWrite(led3, LOW);    // Turn OFF the LED
}
else if (kWh>175)
{
digitalWrite(led1, LOW);    // Turn OFF the LED
digitalWrite(led2, LOW);    // Turn OFF the LED
digitalWrite(led3, HIGH);    // Turn OFF the LED
}


DP =   PPROM -Pprom_b;
Pprom_b = PPROM;


PA =  VRMS_an * IRMS_a;
DS=  PA-Dapar;
Dapar = PA;
FP = PPROM/ PA;

String V = String(VRMS_an);
String I = String(IRMS_a);
String PP = String(PPROM);
String SFP = String(FP);
String DELTA_P = String(DP);
String DELTA_S = String(DS);
String SPA = String(PA);


client.connect("server_TESIS");

    // publish/subscribe
    if (client.isConnected()) {
        client.publish("VOLTAJE",V);
        client.publish("CORRIENTE",I);
        client.publish("PPROMEDIO",PP);
        client.publish("BD/FACTOR DE POTENCIA",SFP);
        client.publish("BD/DELTA_P",DELTA_P);
        client.publish("BD/DELTA_S",DELTA_S);
        client.publish("PAPARENTE",SPA);
    } 
 

 delay(300);
 


}
