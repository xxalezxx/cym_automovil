
#include <Servo.h>

  //---* Motor A *---
#define enA 6
#define inA1 2
#define inA2 12

  //---* Motor B *---
#define enB 9
#define inB1 7
#define inB2 4

  //---* Servo-motor A *---
Servo servoA; //Crear el objeto del servo A
#define pinServoA 8

//---* Sensor Ultrasonico *---
#define trig 10
#define echo 11

//---* Variables generales *---

//Limites de deteccion en CENTIMETROS
#define limiteDeteccionLargo 90.0
#define limiteDeteccionCorto 50.0
#define limiteDeteccionPeligro 20.0
//Velocidades del vehiculo
#define velocidadRapida 255
#define velocidadLenta 250
#define velocidadPeligro 240
#define velocidadDeGiroParcial 255
#define velocidadDeGiroTotal 255
//Delay de tiempo para distintos cambios de radio
#define delayGiro 1000
#define delayGiroTotal 3000
//Delay de deteccion lateral
#define delayDeteccionLateral 1200
//Valores de angulo de servoA
#define gradosIzquierda 135
#define gradosDerecha 45
#define gradosRecto 90
//Variables adicionales del programa main
char giroAnteriorLibre = 'i';


void setup() {

  Serial.begin(9600);

  //*---- Configuracion de puertos para salida ----*
  
  //Puertos: OUTPUT Motor A
  pinMode(enA, OUTPUT);
  pinMode(inA1, OUTPUT);
  pinMode(inA2, OUTPUT);

  //Puertos: OUTPUT Motor B
  pinMode(enB, OUTPUT);
  pinMode(inB1, OUTPUT);
  pinMode(inB2, OUTPUT);

  //Puertos: OUTPUT SensorUltrasonico
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(13, OUTPUT);

  //Configuracion del servo-motor
  servoA.attach(pinServoA);

  //Inicializar robot, unica vez
  inicializarRobot();
  
}

void loop() {

  //Modo funcionamiento 1 
  modoRobot_1();

}

//Param: --
//Output: --
//Inicializa el robot y sus respectivos motores por unica vez
void inicializarRobot(){
 
  for(int i=0; i <= 180; i++){
    servoA.write(i);
    delay(14);
  }
  delay(50);
  servoA.write(90);
  
  Serial.println("Robot start");
}

//Param: --
//Output: --
void modoRobot_1(){

  int situacionObstaculo = obstaculos();
  if( situacionObstaculo == 0 ){
    Serial.println("--> Adelante LENTO");
    irAdelante('l');
  }
  else if( situacionObstaculo == 3 ) {
    Serial.println("--> Cambio de SENTIDO");
    cambiarSentido();
  }
  else if( situacionObstaculo == 2 ){
    Serial.println("--> Adelante RAPIDO");
    irAdelante('r');
  }
  else if( situacionObstaculo == 1 ){
    Serial.println("--> Adelante LENTO PELIGRO");
    irAdelante('p');
  }
}

//Param: --
//Output: --
//
void cambiarSentido(){
  
  pararMotores();
  char direccion = nuevaOrientacion();
  if( direccion == 'i'){
    Serial.println("==> Girando a la izquierda");
    girarMotorIzquierda();
    if( existeObstaculo() ){
      Serial.println("--> Deteccion a pesar del giro Izquierdo");
      cambiarSentido();
    }
    irAdelante('l');
  }
  else if( direccion == 'd'){
    Serial.println("==> Girando a la derecha");
    girarMotorDerecha();
    if( existeObstaculo() ){
      Serial.println("--> Deteccion a pesar del giro Derecho");
      cambiarSentido();
    }
    irAdelante('l');
  }
  else if( direccion == 'a'){
    Serial.println("==> Girando 180");
    darVuelta();
    irAdelante('l');
  }
}

void darVuelta(){
  
  analogWrite(enA, velocidadDeGiroTotal);
  digitalWrite(inA1, HIGH);
  digitalWrite(inA2, LOW);

  analogWrite(enB, velocidadDeGiroTotal);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, HIGH);
  
  delay(delayGiroTotal);
  pararMotores();
}

void girarMotorIzquierda(){
  
  analogWrite(enA, velocidadDeGiroParcial);
  digitalWrite(inA1, HIGH);
  digitalWrite(inA2, LOW);

  analogWrite(enB, velocidadDeGiroParcial);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, HIGH);

  delay(delayGiro);
  pararMotores();
}

void girarMotorDerecha(){
  
  analogWrite(enA, velocidadDeGiroParcial);
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, HIGH);

  analogWrite(enB, velocidadDeGiroParcial);
  digitalWrite(inB1, HIGH);
  digitalWrite(inB2, LOW);

  delay(delayGiro);
  pararMotores();
}

void pararMotores(){

  analogWrite(enA, 0);
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, LOW);

  analogWrite(enB, 0);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, LOW);
  
}

char nuevaOrientacion(){
  
  boolean obstaculoIzquierdo = existeObstaculoEn('i');
  boolean obstaculoDerecha = existeObstaculoEn('d');
  if(obstaculoIzquierdo && !obstaculoDerecha){
    Serial.println("--* Obstaculo IZQUIERDA");
    return 'i';
  }
  if(!obstaculoIzquierdo && obstaculoDerecha){
    Serial.println("--* Obstaculo DERECHA");
    return 'd';
  }
  if( !obstaculoIzquierdo && !obstaculoDerecha ){
    Serial.println("--* Despejado lateralmente");
    return sentidoRandom();
    
  }
  else{
    Serial.println("--* Obstaculo LATERAL --  IR ATRAS");
    return 'a';
  }
}

char sentidoRandom(){
 
  if( giroAnteriorLibre == 'i' ){
    giroAnteriorLibre = 'd';
    return giroAnteriorLibre;
  }
  else{
    giroAnteriorLibre = 'i';
    return giroAnteriorLibre;
  }
}

boolean existeObstaculoEn(char sentido){
  
  if(sentido == 'i'){
    servoA.write(gradosIzquierda);
    delay(delayDeteccionLateral);
    if(existeObstaculo()){
      servoA.write(gradosRecto);
      delay(80);
      return true;
    }
    else{
      servoA.write(gradosRecto);
      delay(80);
      return false;
    }
  }

  if(sentido == 'd'){
    servoA.write(gradosDerecha);
    delay(delayDeteccionLateral);
    if(existeObstaculo()){
      servoA.write(gradosRecto);
      delay(80);
      return true;
    }
    else{
      servoA.write(gradosRecto);
      delay(80);
      return false;
    }
  }
  
}

boolean existeObstaculo(){
  
  if(sensorUlt() <= limiteDeteccionCorto ){
    return true;
  }
  return false;
}

void irAdelante(char velocidad){
  
  if(velocidad == 'r'){
    Serial.println("RAPIDOOOO");
    girarMotorAdelante(velocidadRapida);
  }
  else if(velocidad == 'l'){
    girarMotorAdelante(velocidadLenta);
  }
  else if( velocidad == 'p'){
    girarMotorAdelante(velocidadPeligro);
  }
}

//Gira los motores para adelante acorde a la velocidad ingresada
void girarMotorAdelante(int velocidad){
  
  analogWrite(enA, velocidad);
  digitalWrite(inA1, HIGH);
  digitalWrite(inA2, LOW);

  analogWrite(enB, velocidad);
  digitalWrite(inB1, HIGH);
  digitalWrite(inB2, LOW);
}

//return 0 cuando el obstaculo esta entre el limite largo y el limite corto
//return 1 cuando el obstaculo esta por debajo del limite corto
//return 2 cuando el obstaculo esta por ensima del limite largo
int obstaculos(){
  
  long distancia = sensorUlt();
  if( distancia <= limiteDeteccionLargo 
  && distancia >= limiteDeteccionCorto ){
     return 0;
  }
  else if( distancia <= limiteDeteccionCorto 
  && distancia >= limiteDeteccionPeligro) {
    return 1;
  }
  else if( distancia <= limiteDeteccionPeligro){
    return 3;
  }
  return 2;
}

//return long distancia en cm del sensor ultrasonico
long sensorUlt(){ 
  
  digitalWrite(trig,LOW);
  delayMicroseconds(10);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  long t = pulseIn(echo, HIGH);
  return (t/2) * 0.0343;
}
