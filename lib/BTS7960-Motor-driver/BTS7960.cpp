#include "Arduino.h"
#include "BTS7960.h"



BTS7960::BTS7960(int INH_R, int IN_R, int IS_R, int INH_L, int IN_L, int IS_L, bool DEBUG)
{
    _inh_R =INH_R;
	_in_R =IN_R;
	_is_R =IS_R;
	
    _inh_L =INH_L;
	_in_L =IN_L;
	_is_L =IS_L;
	
	_debug = DEBUG;

}
BTS7960::BTS7960(int INH_R, int IN_R, int INH_L, int IN_L, bool DEBUG)
{
    _inh_R =INH_R;
	_in_R =IN_R;
	_is_R =-1;
	
    _inh_L =INH_L;
	_in_L =IN_L;
	_is_L =-1;
	
	_debug = DEBUG;

}

void BTS7960::begin()
{
	pinMode(this->_inh_R, OUTPUT);
	pinMode(this->_in_R, OUTPUT);
	if (this->_is_R >= 0) {
		pinMode(this->_is_R, INPUT);
	}		
	
	pinMode(this->_inh_L, OUTPUT);
	pinMode(this->_in_L, OUTPUT);
	if (this->_is_L >= 0) {
		pinMode(this->_is_L, INPUT);
	}		

	if(this->_debug){
		Serial.println("BTS7960 Motor Control");
		Serial.println("by P-M-X");		
	}	
}

void BTS7960::rotate(int value, int dir) {

    digitalWrite(this->_inh_R, HIGH);
	digitalWrite(this->_inh_L, HIGH);
	int pwm1Pin, pwm2Pin;
	if(dir ==1){
		pwm1Pin =this->_in_L;
		pwm2Pin =this->_in_R;
	}else{
		pwm1Pin =this->_in_R;
		pwm2Pin =this->_in_L;		
	}
	if(this->_debug){
		Serial.print("PWM1: ");
		Serial.print(pwm1Pin);
		Serial.print(" PWM2: ");
		Serial.print(pwm2Pin);
		Serial.print(" PWM value: ");
		Serial.print(value);
		Serial.println("%");		
	}     

    if(value >=0 && value <=100 ){
		
		analogWrite(pwm2Pin, toPWM(value));
	}
}

void BTS7960::stop(){
    digitalWrite(this->_in_R, LOW);
	digitalWrite(this->_in_L, LOW);
	if(this->_debug){
		Serial.println("Motor stopped");
	}	
}

int BTS7960::toPWM(int v){
  return map(v, 0,100,0,255);
}

