#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <AccelStepper.h>
#include <BlynkSimpleEsp8266.h>

const int stepsPerRevolution = 200;  //number of steps per revolution

char auth[] = "Blynk_project_token";// Insert the token number of your project

//WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Network_name";
char pass[] = "Network_password";

unsigned long timer_1 = 0;
unsigned long timer_2 = 0;

#define food_pin D6
#define water_pin D7
#define pump_pin D5
#define enable_pin D0


int food_slider = 0;
int water_slider = 0;


//Food Button
int _V0 = 0;
BLYNK_WRITE(V0){
  _V0 = param.asInt();
}
int _V0M = 0;

//Food Slider
int _V4 = 0;
BLYNK_WRITE(V4){
  _V4 = param.asInt();
}
int foodAmount = 0;

//Food LED
WidgetLED ledFood(V2);

//Water button
int _V1 = 0;
BLYNK_WRITE(V1){
  _V1 = param.asInt();
}
int _V1M = 0;

//Water slider
int _V5 = 0;
BLYNK_WRITE(V5){
  _V5 = param.asInt();
}
int waterAmount = 0;

//Water LED
WidgetLED ledWater(V3);

AccelStepper stepper2(AccelStepper::FULL4WIRE, D1, D2, D3, D4);

void setup(){
  Serial.begin(9600);
  stepper2.setMaxSpeed(500.0);
  stepper2.setAcceleration(100.0);
  Blynk.begin(auth, ssid, pass);
  pinMode(pump_pin, OUTPUT);
  digitalWrite(pump_pin, LOW);
  pinMode(enable_pin, OUTPUT);
  digitalWrite(enable_pin, LOW);
  pinMode(food_pin, INPUT);
  pinMode(water_pin, INPUT);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
}

bool flag_food_M = true;
bool flag_water_M = true;

void loop(){
  Blynk.run();
  if((millis() - timer_1) >= 1000){
    bool flag_food = digitalRead(food_pin);
    if(flag_food != flag_food_M){
      if(flag_food){
          ledFood.on();
        }else{
          ledFood.off();  
          }
        flag_food_M = flag_food;
      }
    bool flag_water = digitalRead(water_pin);
    if(flag_water != flag_water_M){
      if(flag_water){
          ledWater.off();
        }else{
          ledWater.on();  
          }
        flag_water_M = flag_water;
      }
    timer_1 = millis();
    Serial.print(flag_food);
    Serial.print("|");
    Serial.print(flag_water);
    Serial.print("|");
    Serial.print(_V4);
    Serial.print("|");
    Serial.println(_V5);
    }
  if(bool(_V0)){
    if(!_V0M){
      foodAmount = map(_V4,0,100,0,20000);
      digitalWrite(enable_pin, HIGH);
      stepper2.moveTo(foodAmount);
      }else if(stepper2.distanceToGo() == 0){
          _V0 = 0;
          stepper2.setCurrentPosition(0);
          Blynk.virtualWrite(V0, _V0);
          digitalWrite(enable_pin, LOW);
          }
      _V0M = _V0;
    }
  if(bool(_V1)){
    if(!_V1M){
      waterAmount = map(_V5,0,100,0,30000);
      timer_2 = millis();
      digitalWrite(pump_pin, HIGH);
    }else if(millis() - timer_2 >= waterAmount){
      _V1 = 0;
      digitalWrite(pump_pin, LOW);
      Blynk.virtualWrite(V1, _V1);
      }
    _V1M = _V1; 
    }
    stepper2.run();
}// End Loop
