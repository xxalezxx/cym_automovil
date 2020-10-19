
#include <Servo.h>

  //Motor A
#define enA 3
#define inA1 2
#define inA2 12
  //Motor B
#define enB 5
#define inB1 7
#define inB2 4
  //Servo-motor A
Servo servoA; //Crear el objeto del servo A
#define pinServoA 8

//Sensor Ultrasonico
#define trig 10
#define echo 11

//Variables generales

#define limiteDeteccionLargo 80.0
#define limiteDeteccionCorto 30.0
#define velocidadRapida 220
#define velocidadLenta 100
#define delayGiro 1000

  int pos =0;



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

  servoA.attach(pinServoA);

  inicializarRobot();

  
}

void loop() {

  modoRobot_1();


}



void inicializarRobot(){
  servoA.write(90);
  delay(500);
  servoA.write(45);
  delay(500);
  servoA.write(135);
  delay(500);
  servoA.write(90);

  Serial.println("Robot start");
}

void modoRobot_1(){

  int situacionObstaculo = obstaculos();
  if( situacionObstaculo == 0 ){
    Serial.println("--> Adelante LENTO");
    irAdelante('l');
  }
  else if( situacionObstaculo == 1 ) {
    Serial.println("--> Cambio de SENTIDO");
    cambiarSentido();
  }
  else if( situacionObstaculo == 2 ){
    Serial.println("--> Adelante RAPIDO");
    irAdelante('r');
  }
  
}

void cambiarSentido(){
  pararMotores();
  char direccion = nuevaOrientacion();
  if( direccion == 'i'){
    Serial.println("==> Girando a la izquierda");
    girarMotorIzquierda();
    irAdelante('l');
  }
  else if( direccion == 'd'){
    Serial.println("==> Girando a la derecha");
    girarMotorDerecha();
    irAdelante('l');
  }
  else if( direccion == 'a'){
    Serial.println("==> Girando 180");
    darVuelta();
    irAdelante('l');
  }
}

void darVuelta(){
  analogWrite(enA, 100);
  digitalWrite(inA1, HIGH);
  digitalWrite(inA2, LOW);

  analogWrite(enB, 100);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, HIGH);

  delay(3000);

  pararMotores();
}

void girarMotorIzquierda(){
  analogWrite(enA, 100);
  digitalWrite(inA1, HIGH);
  digitalWrite(inA2, LOW);

  analogWrite(enB, 100);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, HIGH);

  delay(delayGiro);

  pararMotores();
}

void girarMotorDerecha(){
  analogWrite(enA, 100);
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, HIGH);

  analogWrite(enB, 100);
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
  if(existeObstaculoEn('i')){
    Serial.println("--* Obstaculo IZQUIERDA");
    return 'i';
  }
  if(existeObstaculoEn('d')){
    Serial.println("--* Obstaculo DERECHA");
    return 'd';
  }
  else{
    Serial.println("--* Obstaculo LATERAL --  IR ATRAS");
    return 'a';
  }
  
}

boolean existeObstaculoEn(char sentido){
  if(sentido == 'i'){
    servoA.write(135);
    delay(1200);
    if(existeObstaculo()){
      servoA.write(90);
      delay(80);
      return true;
    }
    else{
      servoA.write(90);
      delay(80);
      return false;
    }
  }

  if(sentido == 'd'){
    servoA.write(45);
    delay(1200);
    if(existeObstaculo()){
      servoA.write(90);
      delay(80);
      return true;
    }
    else{
      servoA.write(90);
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
    girarMotorAdelante(velocidadRapida);
  }
  else if(velocidad == 'l'){
    girarMotorAdelante(velocidadLenta);
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
  Serial.print("==> Distancia: ");
  Serial.println(distancia);
  if( distancia <= limiteDeteccionLargo 
  && distancia >= limiteDeteccionCorto ){
     return 0;
  }
  else if( distancia <= limiteDeteccionCorto ) {
    return 1;
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
