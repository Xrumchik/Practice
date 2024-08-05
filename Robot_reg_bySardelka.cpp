#include "Robot_reg_bySardelka.h"
#include <Servo.h>

Servo big_servo,small_servo;


#define ar(x) analogRead(x)
#define dr(x) digitalRead(x)
#define aw(x,y) analogWrite(x,y)
#define dw(x,y) digitalWrite(x,y)

/*Robot_reg::Robot_reg(){ //заготовка под конструктор с параметрами
  this->pin_led1 = 
  this->pin_led2 = 
  this->pin_inter_enA = 
  this->pin_inter_enB = 
  this->dirA = 
  this->dirB = 
  this->spdA = 
  this->spdB = 
  this->trig = 
  this->echo = 
  this->pin_btn1 = 
  this->pin_led3 = 
  this->pin_enA = 
  this->pin_enB = 
  this->pin_lineL = 
  this->pin_lineR = 
  this->pin_distL = 
  this->pin_distR = 
}*/


Robot_reg::Robot_reg(){
  this->pin_led1 = 0;
  this->pin_led2 = 1;
  this->pin_inter_enA = 2;
  this->pin_inter_enB = 3;
  this->dirA = 4;
  this->dirB = 5;
  this->spdA = 6;
  this->spdB = 7;
  this->trig = 8;
  this->echo = 9;
  this->pin_btn1 = 10;
  this->pin_led3 = 11;
  this->pin_enA = 12;
  this->pin_enB = 13;
  this->pin_lineL = 14;
  this->pin_lineR = 15;
  this->pin_distL = 16;
  this->pin_distR = 17;
  this->pin_servo_big = 18;
  this->pin_servo_small = 19;
  this->tickL = 0;
  this->tickR = 0;
  this->dspd = 100;
  this->cm2tick = 400 / 11;
  this->deg2tk = 1200/180;
  this->grey = 600;
  this->errold = 0;
  this->servo_so = 90;
  this->servo_sc = 180;
  this->servo_bd = 130;
  this->servo_bu = 80;
  
  for (int i = 4;i<10;i++) pinMode(i,OUTPUT);
  pinMode(trig,INPUT_PULLUP);
  //кнопку инициализировать!!!!!!!!!!!!!!
  big_servo.attach(pin_servo_big);
  small_servo.attach(pin_servo_small);
  //attachInterrupt(digitalPinToInterrupt(pin_inter_enA), encoderL, RISING);
//  attachInterrupt(digitalPinToInterrupt(pin_inter_enB), encoderR, RISING);
}

void Robot_reg::set_cm2tick(float number){
  this->cm2tick=number;
}

void Robot_reg::set_deg2tk(float number){
  this->deg2tk=number;
}

void Robot_reg::set_dspd(float number){
  this->dspd=number;
}


void Robot_reg::encoderL() {
  if (digitalRead(pin_inter_enA) == digitalRead(pin_enA)) {
    tickL--;
  } else {
    tickL++;
  }
}

void Robot_reg::encoderR() {
  if (digitalRead(pin_inter_enB) == digitalRead(pin_enB)) {
    tickR++;
  } else {
    tickR--;
  }
}

void Robot_reg::zero_tick(){
  tickL=0;
  tickR=0;
}

void Robot_reg::check_tick(){
  zero_tick();
  while (tickL<500 and tickR<500){
    move(100,100);
  }
  stop();
}

void Robot_reg::turn(float degr) { //больше 0 - через лево, меньше 0 - через право
  float tk = degr*deg2tk;
  zero_tick();
  if(degr>0) {
    move(-dspd, dspd);
    while(tickR < tk);
  }
  else {
    move(dspd, -dspd);
    while(tickR > tk);
  }
  stop();
}

void Robot_reg::goStraight(float cm, bool stp = true) {
  float tk = cm * cm2tick;
  zero_tick();
  if (cm > 0) {
    while (tickR < tk) {
      float err = 10.0 * (tickR - tickL);
      move(dspd + err, dspd - err);
      delay(5);
    }
  }
  else {
    while (tickR > tk) {
      float err = 10.0 * (tickR - tickL);
      move(-100 + dspd, -dspd - err);
      delay(5);
    }
  }
  move(0, 0);
  if (stp) stop();
}

void Robot_reg::move(int left,int right){
  dw(dirA,left>0);
  dw(dirB,right>0);

  left=map(constrain(abs(left),0,100),0,100,0,255);
  right=map(constrain(abs(right),0,100),0,100,0,255);

  aw(spdA,left);
  aw(spdB,right);
}

void Robot_reg::test_reversed_motor(){
  move(80,80);
  delay(500);
  move(80,-80);
  delay(500);
}

void Robot_reg::stop() {
  zero_tick();
  uint32_t ts = millis() + 400;
  while (millis() < ts) {
    move(tickL * -30, tickR * -30);
  }
  move(0, 0);
}

void Robot_reg::show_all_system(){//!!!!!!!!!!!!!!!!!!!!!переделать
  //сделать
}

void Robot_reg::line(int spd,int p,int d){
  int erp=(ar(pin_lineL)-ar(pin_lineR))*p/100;
  int erd=(ar(pin_lineL)-ar(pin_lineR)-errold)*d/100;
  erp=erp+erd;
  aw(spdA,map(constrain(spd-erp,0,100),0,100,0,255));
  aw(spdB,map(constrain(spd+erp,0,100),0,100,0,255));
  errold=erp+erd;
}

void Robot_reg::ncrossroad(int x){
  int c=0;
  while (c!=x){
    while (not(ar(pin_lineL)>grey and ar(pin_lineR)>grey)){
      line(40,10,2);
    }
    c++;
    if (c!=x){
      while (ar(pin_lineL)>grey and ar(pin_lineR)>grey){
        line(40,10,2);
      }
    }
  }
  move(0,0);
}

void Robot_reg::to_open(){ //!!!!!!!!!!!!!!сделать переменными градус открытия и закрытия
  for (int i=80;i<130;i++){
    big_servo.write(i);
    delay(20);
  }
  for (int i=90;i<180;i++){
    small_servo.write(i);
    delay(20);
  }
}

void Robot_reg::to_close(){
  for (int i=130;i>80;i--){
    big_servo.write(i);
    delay(20);
  }
  for (int i=180;i>90;i--){
    small_servo.write(i);
    delay(20);
  }
}


int Robot_reg::distance_hcsr04(){
  dw(trig, LOW);
  delayMicroseconds(5);
  dw(trig, HIGH);
  delayMicroseconds(10);
  dw(trig, LOW);
  duration = pulseIn(echo, HIGH);
  cmm = (duration / 2) / 29.1;
  return cmm;
}

int Robot_reg::distance_sharpL(){
  return ar(pin_distL);
}

int Robot_reg::distance_sharpR(){
  return ar(pin_distR);
}

int Robot_reg::distance_sharp_tocm(int number){
  float volts = number*0.0048828125;
  int distance=32*pow(volts,-1.10);
  return distance;
}
