#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* 
Bloque de código para hacer marcha paro e inversión de giro a un motor DC de inducción con dos led como indicadores.
Como es para motores chicos de inducción no hay un relé térmico.

Elementos:

-Motor DC inducción.
-Led Verde: Motor activo
-Led rojo: Motor en parada.
-S1: Marcha, S2: Paro, S3: Inversión de giro, S0: Parada de emergencia.
-2 Relés controlan marcha, inversión de giro y parada. Deben tener terminal NC, NO y COM cada uno de ellos
*/

//------------------------------------------------------------------------------------------------------------------------

//Constantes

const int LED_MARCHA = 14;
const int LED_PARO = 27;
const int S1_MARCHA = 25;
const int S2_PARO = 33;
const int S3_INVERSION = 32;
const int S0_Parada_EM = 26;
const int KM1 = 4;
const int KM2 = 2;

// Constantes para medir velociadad

const int B1_SENSOR = 13; //Sensor infrarrojos B1 digital
const int RANURAS_ENCODER = 10; // numero de ranura del encoder

// Constantes para OLED 

const int COLUMNAS = 128;
const int FILAS = 64;
const int OLED_RESET = -1;

Adafruit_SSD1306 oled(COLUMNAS, FILAS, &Wire, OLED_RESET);

//--------------------------------------------------------------------------------------------------------------------------

//Variables Globales

  double lastRpmValue = 0;

//---------------------------------------------------------------------------------------------------------------------------

//Subprogramas

//Subprograma setup

void setup(){

  Serial.begin(9600);

  //Setup control motor

  pinMode(LED_MARCHA, OUTPUT);
  pinMode(LED_PARO, OUTPUT);
  pinMode(S1_MARCHA, INPUT_PULLUP);
  pinMode(S2_PARO, INPUT_PULLUP);
  pinMode(S3_INVERSION, INPUT_PULLUP);
  pinMode(S0_Parada_EM, INPUT_PULLUP); //No tengo seta así que lo configuro normal
  pinMode(KM1, OUTPUT);
  pinMode(KM2, OUTPUT);

  //Setup sensor velocidad

  pinMode(B1_SENSOR, INPUT);

  //Setup OLED

  Wire.begin();
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();
  oled.display();

}

//Subprogramas inversion marcha paro ------------------------------------------------

//Subprogramas que indican si hay errores

bool hayErrores (bool error1, bool error2){

  bool result = false;

  if (error1 || error2){

    result = true;

  }

  return result;

}

bool activacionYdesactvacionSimult (bool petActiv, bool petInver, bool petparada){ // error 1

  bool result = false;

  if ((petActiv && petparada) || (petInver && petparada)){

    result = true;

    Serial.println("\nError1");

  }

  return result;

}

bool marchaEinversionSimult (bool petMarch, bool petInver){ // error 2

  bool result = false;

  if (petMarch && petInver){

    result = true;

    Serial.println("\nError2");

  }
  
  return result;

}

//Subprogramas de peticiones

bool hayPeticionInicio ( bool swArranqueValue, bool swInversionValue){

  bool result = false;

  if (swArranqueValue || swInversionValue){

    result = true;

  }

  return result;

}

bool peticionMarcha (bool swArranqueValue){

  bool result = false;

  if (swArranqueValue){

    result = true;

  }

  return result;

}

bool peticionInversion (bool swInversionValue){

  bool result = false;

  if (swInversionValue){

    result = true;

  }

  return result;

}

bool peticionParada (bool swParadaValue, bool swParadaEmergenciaValue){

  bool result = false;

  if (swParadaValue || swParadaEmergenciaValue){

    result = true;

  }

  return result;

}

bool peticionParadaEmergencia (bool swParadaEmergenciaValue){

  bool result = false;

  if (swParadaEmergenciaValue){

    result = true;

  }

  return result;

}

//Posibilidad de inicio del sistema runSys

bool esPosible (bool errorExist, bool peticionInicioExiste, bool petParoExiste, bool enMarcha, bool enInversion){

  bool result = false;

  if (!errorExist && (peticionInicioExiste || enMarcha || enInversion) && !petParoExiste){

    result = true;

  }

  return result;

}

//Subprogramas estados del sistema

bool estaEnParo (bool km1Value, bool km2Value){ // 0 0 = 0

//tambien es valido 1 1 = 0 pero usaremos 0 0 = 0

  bool result = true;

  if ((km1Value && !km2Value) || (!km1Value && km2Value)){

    result = false;

  }

  return result;

}

bool estaEnMarcha (bool km1Value, bool km2Value){ // 0 1 = 1 

  bool result = false;

  if (!km1Value && km2Value){

    result = true;

  }

  return result;

}

bool estaEnInversion (bool km1Value, bool km2Value){ // 1 0 = 1

  bool result = false;

  if (km1Value && !km2Value){

    result = true;

  }

  return result;

}

//Subprogramas marcha paro inversion reles

void marcha (int pinKm1, int pinKm2){ // Pin del rele para activarlo

  // marcha = 0|1 = giro hacia un lado

  Serial.println ("\nSe ejecuta Marcha");

  digitalWrite(pinKm1,LOW);
  digitalWrite(pinKm2, HIGH);

}

void inversion (int pinKm1, int pinKm2){

  // marcha = 1|0 = giro hacia el otro lado

  Serial.println ("\nSe ejecuta Inversion");

  digitalWrite(pinKm1, HIGH);
  digitalWrite(pinKm2, LOW);

}

void paro (int pinKm1, int pinKM2){

  // paro = 0|0 = parada

  //Serial.println ("\nSe ejecuta paro");

  digitalWrite (pinKm1,LOW);
  digitalWrite (pinKM2,LOW);

}

//subprograma de acción del sistema

void sistema (bool sWarranque, bool sWparo, bool sWinversion, bool sWparadaEmergencia, bool km1Value, bool km2Value, int ledMarcha, int ledParo){

  bool existePeticionInicio = hayPeticionInicio (sWarranque, sWinversion);
  bool existePeticionMarcha = peticionMarcha (sWarranque);
  bool existePeticionInversion = peticionInversion (sWinversion);
  bool existePeticionParo = peticionParada(sWparo, sWparadaEmergencia);

  bool error1 = activacionYdesactvacionSimult(existePeticionMarcha, existePeticionInversion, existePeticionParo);
  bool error2 = marchaEinversionSimult(existePeticionMarcha, existePeticionInversion);
  bool existenErrores = hayErrores (error1, error2);

  bool estaEnMarchaMotor = estaEnMarcha(km1Value,km2Value);
  bool estaEnInversionMotor = estaEnInversion(km1Value, km2Value);
  
  bool runSys = esPosible (existenErrores, existePeticionInicio, existePeticionParo, estaEnMarchaMotor, estaEnInversionMotor);  //Devuelve un valor booleano haciendo posible correr sistema o no
  // Hubo peticion de inicio o el sistema está en marcha y no se encuentran errores
 
  if (runSys){ //El sistema arranca si se pide y no hay errores o bien sigue en marcha o inversion

    //Serial.println("\nRunning");

    if (!existePeticionInversion && (existePeticionMarcha || estaEnMarchaMotor)){

      //Serial.println("\nPeticion marcha o sigue en marcha aceptada");

      /*if (estaEnMarchaMotor){ //retraso en cambio de giro

        delay(2000);

      }*/
      
      marcha(KM1, KM2);
    
      digitalWrite (ledParo, LOW);
      digitalWrite (ledMarcha, HIGH);

    } else if (!existePeticionMarcha && (existePeticionInversion || estaEnInversionMotor)){

      //Serial.println("\nPeticion inversion o sigue en inversion aceptada");

     /*if (estaEnInversionMotor){ //retraso en cambio de giro

        delay(2000);

      }*/

      inversion(KM1, KM2);
    
      digitalWrite (ledParo, LOW);
      digitalWrite (ledMarcha, HIGH);

    }

  } else {

    //Serial.print("\nEn paro o existe error");

    paro(KM1, KM2);

    digitalWrite (ledParo, HIGH);
    digitalWrite (ledMarcha, LOW);

  }

}

//subprogramas medir velocidad---------------------------------------------

double calculaRpm (int pinInfraredSensor, int numeroRanurasEncoder){

  // Time debe ser un parámetro que esté en minutos no en milisegundos

  static unsigned long rpm = 0; // Variable que guarda el valor de las revoluciones por minuto
  unsigned long endTime;
  unsigned long elapsedTime;
  double elapsedTimeMinutes;
  bool infraredSensorValue = !digitalRead(pinInfraredSensor); // Lee el valor del sensor infrarrojo
  static bool lastInfraredSensorValue = LOW;
  static int cnt = 0;
  static unsigned long startTime = 0;

  // Monitorizar el estado del sensor infrarrojo
  // Serial.print("Estado del sensor infrarrojo: ");
  // Serial.println(infraredSensorValue);

  // Detecta un flanco ascendente
  if (infraredSensorValue == HIGH && lastInfraredSensorValue == LOW) {
      
    cnt++;
    // Serial.print("Pulsos detectados: ");
    // Serial.println(cnt);

    if (cnt == 1) {
      startTime = millis(); // Captura el tiempo inicial cuando se detecta el primer pulso
    }
    // Serial.print("startTime: ");
    // Serial.println(startTime);

  }

  if (cnt >= numeroRanurasEncoder){

    // Se considera una revolucion del disco cuando cnt = 10
    //Dividir 1 revolución por el tiempo se obtiene revolución por segundos
    //El tiempo se debe terminar dando en minutos para obtener revoluciones por cada minuto

    // Serial.println ("Rotación completa");

    endTime = millis(); // Captura tiempo final cuando se detecta que se ha dado una revolución
    elapsedTime = (endTime - startTime); // Calcula el tiempo transcurrido en milisegundos
    elapsedTimeMinutes = (float)elapsedTime / 60000.0; // Convierte el tiempo a minutos

    // Monitorizar tiempos y cálculo de RPM
    // Serial.print("startTime: ");
    // Serial.println(startTime);
    // Serial.print("endTime: ");
    // Serial.println(endTime);
    // Serial.print("elapsedTime: ");
    // Serial.println(elapsedTime);
    // Serial.print("elapsedTimeMinutes: ");
    // Serial.println(elapsedTimeMinutes);

   if (elapsedTime > 0) {
      rpm = 1.0 / elapsedTimeMinutes; // Calcula las RPM
      Serial.print("RPM calculadas: ");
      Serial.println(rpm);
    } else {
      rpm = 0; // Evita divisiones por 0
      // Serial.println("Advertencia: Tiempo transcurrido es 0, RPM no calculadas.");
    }

    cnt = 0; // Reinicia el contador
    startTime = millis(); // Reinicia el tiempo inicial

  }

  lastInfraredSensorValue = infraredSensorValue; // Reinicia el valor del sensor infrarrojo para la siguiente iteración

  return rpm;

}

//subprogramas mostrar por pantalla

void muestraElError (bool activYDesactSimultanea, bool marchaEInverSimultanea){

  Serial.print("MUESTRA ERROR");

  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.setTextSize(1);
  oled.print ("Error del sistema:");

  if (activYDesactSimultanea) {
    oled.setCursor(0, 30);
    oled.print ("Activacion/Paro simultaneo");
  }

  if (marchaEInverSimultanea) {
    oled.setCursor(0, 45);
    oled.print ("Marcha/Inversion simultanea");
  }

  oled.display();

}

void muestraInfoPeticion (bool swMarchaValue, bool swParoValue, bool swInversionValue){

  Serial.print("MUESTRA PETICION");

  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.setTextSize(1);
  oled.print("System State: Running");

  if (swParoValue) {
    oled.setCursor(10, 30);
    oled.print("Parada solicitada");
  } else if (swMarchaValue) {
    oled.setCursor(10, 30);
    oled.print("Marcha solicitada");
  } else if (swInversionValue) {
    oled.setCursor(10, 30);
    oled.print("Inversion solicitada");
  }

  oled.display();

}

void muestraEstadoMotor(bool motorEnMarcha, bool motorEnInversion, bool motorEnparo, bool paradaEmergencia, bool actDesacSimult){

  if (paradaEmergencia){

    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
    oled.setTextSize(1);
    oled.print ("System state: Error");
  
    oled.setCursor(0, 30);
    oled.print ("PARADA EMERGENCIA");

    oled.display();  

    delay (2000);
    
  }else if (actDesacSimult){

    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
    oled.setTextSize(1);
    oled.print ("System state: Error");
  
    oled.setCursor(0, 30);
    oled.print ("Activacion/Paro simultaneo");
  
    oled.display();

    delay(2000);


  }else if (motorEnMarcha){

    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
    oled.setTextSize(1);
    oled.print("System state: running");

    oled.setCursor(10, 30);
    oled.print("Motor en marcha");

    oled.display();

  }else if (motorEnInversion){

    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
    oled.setTextSize(1);
    oled.print("System state: running");

    oled.setCursor(10, 30);
    oled.print("Motor en inversion");

    oled.display();

  }else if (motorEnparo){

    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
    oled.setTextSize(1);
    oled.print("System state: stopped");

    oled.setCursor(10, 30);
    oled.print("Motor en parado");

    oled.display();

  }

}

void muestraRpm (double rpmValue){
  
  // Serial.print("MUESTRA RMP");

  // oled.clearDisplay();
  // oled.setTextColor(WHITE);
  // oled.setCursor(0,0);
  // oled.setTextSize(1);
  // oled.print("System state: running");

  oled.setCursor(10, 30);
  oled.print("RPM:");
  oled.setCursor(50, 40);
  oled.print(rpmValue);

  oled.display();

}

bool rpmValueChanged (double rpmValue, double &lastRpmValueCalculated){

  bool result = false;

  if (rpmValue != lastRpmValueCalculated){

    result = true;

    lastRpmValueCalculated = rpmValue;

  }

  return result;

}

//------------------------------------------------------------------------------------------------------------------------------------

//Principal

void loop(){

  //Asignación de variables de estado

  bool valorS1 = !digitalRead (S1_MARCHA);
  bool valorS2 = !digitalRead (S2_PARO);
  bool valorS3 = !digitalRead (S3_INVERSION);
  bool valorS0 = !digitalRead (S0_Parada_EM);
  bool estadoKm1 = digitalRead (KM1);
  bool estadoKm2 = digitalRead (KM2);

  //Hace funcionar el marcha paro e inversión -------------------------------------------

  sistema(valorS1, valorS2, valorS3, valorS0, estadoKm1, estadoKm2, LED_MARCHA, LED_PARO);

  //Calculo de las revoluciones por minuto ----------------------------------------------

  double rpm = calculaRpm(B1_SENSOR, RANURAS_ENCODER);

  //Mostrar información por pantalla ----------------------------------------------------

  //Variables necesarias

  bool petParada =  peticionParada(valorS2, valorS0);
  bool petMarcha = peticionMarcha(valorS1);
  bool petInversion = peticionInversion(valorS3);
  bool peticionEmergencia = peticionParadaEmergencia(valorS0);

  bool actDesactSimlt = activacionYdesactvacionSimult(petMarcha, petInversion, petParada);
  bool marchEinvSimlt = marchaEinversionSimult(petMarcha, petInversion);

  bool motorEnMarcha = estaEnMarcha(estadoKm1, estadoKm2);
  bool motorEnInversion = estaEnInversion(estadoKm1, estadoKm2);
  bool motorEnParo = estaEnParo(estadoKm1, estadoKm2);

  /*muestraEstadoMotor verifica el estado del motor e imprime dicho estado
  Si se pulsa pulsador de emergencia se mostrará y habrá un delay de 2 segundos en pantalla
  También para actDesactSimlt
  */
  muestraEstadoMotor(motorEnMarcha, motorEnInversion, motorEnParo, peticionEmergencia, actDesactSimlt); 
  //muestraRpm(rpm); //Muestra el valor de las revoluciones por minuto

}