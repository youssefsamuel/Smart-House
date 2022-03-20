#include <Keypad.h>
#include<Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int pir;
int ldr;
int incorrectPassword = 0;
bool smoke;
bool flame;
float t;
float f;
char data = 0;
int angle = 90;
const unsigned long eventInterval= 2;
unsigned long previous_temp_time=0; 
unsigned long smoke_temp_time=0; 
unsigned long smoke_seconds=0; 
unsigned long previous_smoke_time=0; 
unsigned long temp_seconds=0;

int motorGardenEnable = 22;
int buzzer_pin = 8;
int doorGaragePWM = 11;
int doorHousePWM = 12;
int flameSensorButton = 44;
int garden_door_sensor = 45;
int motor_garden_door =  23;
int doorGardenDirection1 = 33; 
int doorGardenDirection2 = 34; 
int input_of_PIR_sensor = 38;
int output_of_PIR_sensor = 39;
int ledGarage = 40;
int house_door = 41;
int dog_buzzer = 42;
int dog_door = 43;
int ldr_output_for_led = 46;
int ldr_output_for_curtains = 47;
int output_of_infrared_detection = 48;
int switchGarage = 49;
int bluetoothLed = 50;
int input_of_ultrasonic_sensor = 51;
int output_of_ultrasonic_sensor = 52;
int fan=53;

Servo door; //House Door
Servo garageDoor;
const int ROWS_KEYPAD = 4; //keypad used consists of four rows
const int COLUMNS_KEYPAD = 3; //keypad used consists of three columns
char keys[ROWS_KEYPAD][COLUMNS_KEYPAD] = {{'1','2','3'},{'4','5','6'},{'7','8','9'},{'*','0','#'}};
byte row_pins[ROWS_KEYPAD] = {27, 26, 25, 24}; //connect to the row pinouts of the keypad
byte column_pins[COLUMNS_KEYPAD] = {30, 29, 28}; //connect to the column pinouts of the keypad
int seconds = 0;
Keypad keypad = Keypad(makeKeymap(keys), row_pins, column_pins, ROWS_KEYPAD, COLUMNS_KEYPAD);
int saved_password[4]= {1,2,3,4}; 
int current_password[10] ;
int new_password[10] ;
int trials = 0;
char key ;
int order = 0 ;   //variable to change the screen on LCD (Enter Password or Change Password)
bool flag = 0 ;
int i = 0 ; 
bool check = 0 ; 
int garageDoorOpen = 0;
int houseDoorOpen = 0;
LiquidCrystal_I2C LCD(0x27, 20, 4);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  LCD.init();
  LCD.backlight(); 
  LCD.begin(20 , 4);      // initiaize lcd 
  pinMode(garden_door_sensor, INPUT); //IR sensor for garden door.
  pinMode(output_of_PIR_sensor, OUTPUT);
  pinMode(motor_garden_door, OUTPUT); //Motor to open and close garden door.
  pinMode(output_of_ultrasonic_sensor, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(ldr_output_for_led , OUTPUT);
  pinMode(ldr_output_for_curtains , OUTPUT);
  pinMode(35,OUTPUT);
  pinMode(53, OUTPUT);
  pinMode(A8, INPUT);
  pinMode(13, OUTPUT); //Sets digital pin 13 as output pin  
  pinMode(14, OUTPUT);
  pinMode(A7, INPUT);
  pinMode(ledGarage, OUTPUT); 
  pinMode(doorGardenDirection1, OUTPUT);
  pinMode(doorGardenDirection2, OUTPUT);
  pinMode(motorGardenEnable, OUTPUT);
  //pinMode(flameSensorButton, INPUT_PULLUP);
  pinMode(50, OUTPUT); //Sets digital pin 50 as output pin  
  //pinMode(garden_door_sensor, INPUT_PULLUP);
   //attachInterrupt(digitalPinToInterrupt(flameSensorButton), flame_sensor, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(garden_door_sensor), gardenDoor, CHANGE);
  door.attach(doorHousePWM);
  door.write(60); //pin pwm 12
  digitalWrite(fan,LOW); //pin 53
  digitalWrite(motorGardenEnable, LOW);
  garageDoor.attach(doorGaragePWM);
                  garageDoor.write(90); //PMW pin 11
}
//2 3 18 19 20 21 for interrupts
void gardenDoor() //33, 34, 22
{
    int m;
    m = digitalRead(garden_door_sensor);  //pin 45
    if (m == 0) { 
      Serial.println("hi");
      digitalWrite(doorGardenDirection1, HIGH); //PIN 33
      digitalWrite(doorGardenDirection2, LOW); //PIN 34
      digitalWrite(motorGardenEnable, HIGH); //PIN 22
      delay(1000);
      digitalWrite(motorGardenEnable, LOW);
      delay(1000);
      digitalWrite(doorGardenDirection1, LOW);
      digitalWrite(doorGardenDirection2, HIGH);
      digitalWrite(motorGardenEnable, HIGH);
      delay(1000);
      digitalWrite(motorGardenEnable, LOW);
  }
 }

void Get_Order () {
  if ( key == '*' && flag == 0 ){       //each time user press * the order number increases to change screen 
    order = order + 1 ;                                       
    flag = 1 ;
    LCD.clear();
    i=0;
  }
  else if ( key != '#' ){
    flag = 0 ; 
  }

  if (order >= 2) {
    order = 0 ;
  }
}



void Enter_Password(){
    if(i==0) {
     LCD.setCursor(0,0);
     LCD.print("Enter Password" );
     LCD.setCursor(0,1);
     LCD.print("and then press #" );
     LCD.setCursor(0,3);
     LCD.print("- - - -" );
    }
    
    if(key=='0'||key=='1'||key=='2'||key=='3' 
    ||key=='4'||key=='5'||key=='6' 
    ||key=='7'||key=='8'||key=='9') {
    LCD.setCursor(2*i,3);
    LCD.print(key);
    delay(300);
    LCD.setCursor(2*i,3);
    LCD.print("*");
    current_password[i]= (int)key - 48 ;
    i = i + 1 ;        
    }

    if(key=='#'){
      if(i==4) {
        if((current_password[0]==saved_password[0]) && (current_password[1]==saved_password[1]) &&
        (current_password[2]==saved_password[2]) && (current_password[3]==saved_password[3]) ){
            LCD.clear();
             LCD.setCursor(4,1);
            LCD.print("Welcome Home");
            houseDoorOpen = 1;
            int angle = 0;
            for (angle = 60; angle < 150; angle+=1)
            {
              door.write(angle); //pin pwm 12
              delay(30);
            }
            delay(1000);
            for (angle = 150; angle >= 60; angle-=1)
            {
              door.write(angle);
              delay(30);
            }
            LCD.clear();
            i=0 ;       
         }
         else if (trials == 2)
         {
                trials = 0;
                LCD.clear();
                  LCD.setCursor(7,1);
                 LCD.print("THIEF!!!");
                 delay(1000);
                 digitalWrite(output_of_PIR_sensor,HIGH); //PIN 39
                 delay(2000);
                 digitalWrite(output_of_PIR_sensor,LOW);
                 LCD.clear();
                 i=0 ;  
         }
        else {
            LCD.clear();
            LCD.setCursor(0,0);
            LCD.print("Incorrect Password, Please try again");
            trials = trials + 1;
            delay(500);
            LCD.clear();
            i=0 ;
             }
       }

     else if ( i!=4 ) {
     LCD.clear();
     LCD.setCursor(0,0);
     LCD.print("Incorrect Password, Please try again");
     trials = trials + 1;
     delay(500);
     LCD.clear();
     i=0 ;
     

        
    }
   }

 }
 void Change_Password(){

if(check==0){
    if(i==0) {
     LCD.setCursor(0,0);
     LCD.print("*Change Password*" );
     LCD.setCursor(0,1);
     LCD.print("Enter OLD Password :" );
     LCD.setCursor(0,2);
     LCD.print("and then press #" );
     LCD.setCursor(0,3);
     LCD.print("- - - -" );
    }
    
    if(key=='0'||key=='1'||key=='2'||key=='3' 
    ||key=='4'||key=='5'||key=='6' 
    ||key=='7'||key=='8'||key=='9') {
    LCD.setCursor(2*i,3);
    LCD.print(key);
    delay(300);
    LCD.setCursor(2*i,3);
    LCD.print("*");
    current_password[i]= (int)key - 48 ;
    i = i + 1 ; 
       
    }

    if(key=='#'){
      if(i==4) {
        if((current_password[0]==saved_password[0]) && (current_password[1]==saved_password[1]) &&
        (current_password[2]==saved_password[2]) && (current_password[3]==saved_password[3]) ){
          
            LCD.clear();
            LCD.setCursor(0,0);
            LCD.print("*Change Password*" );
            LCD.setCursor(0,1);
            LCD.print("Enter New Password");
            LCD.setCursor(0,2);
            LCD.print("and then press #" );
            LCD.setCursor(0,3);
            LCD.print("- - - -" );
            check = 1; 
            i=0;
            while(key=='#'){key = keypad.getKey();} 
               }
         else{
             LCD.clear();
             LCD.setCursor(0,0);
             LCD.print("Incorrect Password");
             delay(1000);
             LCD.clear();
             i=0 ;
             }
          }        
       else{
       LCD.clear();
       LCD.setCursor(0,0);
       LCD.print("Incorrect Password");
       delay(1000);
       LCD.clear();
       i=0 ;
       }
    }
 }


else if (check==1) {

            if(key=='0'||key=='1'||key=='2'||key=='3' 
               ||key=='4'||key=='5'||key=='6' 
               ||key=='7'||key=='8'||key=='9') {
                    LCD.setCursor(2*i,3);
                    LCD.print(key);
                    delay(300);
                    LCD.setCursor(2*i,3);
                    LCD.print("*");
                    new_password[i]= (int)key - 48 ;
                    i = i + 1 ; 
               }
            if(key=='#') { 
                      if(i==4){
                         LCD.clear();
                         LCD.setCursor(0,0);
                         LCD.print("Password Changed");
                         saved_password[0]=new_password[0];
                         saved_password[1]=new_password[1];
                         saved_password[2]=new_password[2];
                         saved_password[3]=new_password[3];
                         delay(1000);
                         LCD.clear();
                         i=0 ; 
                         check = 0 ;
                         order = 0 ;
                      }
                       
                     else {  
                       LCD.clear();
                       LCD.setCursor(0,0);
                       LCD.print("Please Enter");
                       LCD.setCursor(0,1);
                       LCD.print("4 digit password");
                       delay(1000);
                       LCD.clear();              
                       LCD.setCursor(0,0);
                       LCD.print("*Change Password*" );
                       LCD.setCursor(0,1);
                       LCD.print("Enter New Password");
                       LCD.setCursor(0,2);
                       LCD.print("and then press #" );
                       LCD.setCursor(0,3);
                       LCD.print("- - - -" );
                       i=0 ;   
                     }
                   }
       }
 }
 void enterTheHouse()
{
  key = keypad.getKey();  // function to get a key from the keypad 
  Get_Order () ;
  if ( order ==0 ) {            
    Enter_Password();                 
  }
     
  else if ( order == 1 ) {      
    Change_Password();         
  }
}
void LDR_Sensor() {
  int ldr = analogRead(A7);
  Serial.println(ldr);
  if (ldr <= 80)   {//its dark, turn on the light 
    digitalWrite(ldr_output_for_led, HIGH);
    } //pin 46
    else { //night
      digitalWrite(ldr_output_for_led, LOW);
  }
  

}

void Ultrasonic_For_Garage() {
  // Duration will be the input pulse width and distance will be the distance to the obstacle in centimeters
  int duration, distance;
  // Output pulse with 1ms width on trigPin
  digitalWrite(output_of_ultrasonic_sensor, HIGH); //pin 52
  delay(2);
  digitalWrite(output_of_ultrasonic_sensor, LOW);
  // Measure the pulse input in echo pin
  duration = pulseIn(input_of_ultrasonic_sensor, HIGH); //pin 51
  // Distance is half the duration devided by 29.1 (from datasheet)
  distance = duration * 0.034 / 2;
  // if distance less than 0.5 meter and more than 0 (0 or less means over range)
  if (distance <= 3 && distance >= 0) {
    digitalWrite(buzzer_pin, HIGH); //pin 8
  } else {
    digitalWrite(buzzer_pin, LOW);
  }
  // Waiting 200 ms
  delay(50);

}
void Garage() {
  int x;
  x = digitalRead(switchGarage); // taking input of switch on pin 49
  Serial.println(x);
  if (x == 1 && garageDoorOpen == 0) {
            for (angle = 90; angle < 180; angle+=1)
            {
              digitalWrite(ledGarage, HIGH); //pin 40
              garageDoor.write(angle); //PMW pin 11
              delay(30);
            }
            digitalWrite(ledGarage, LOW);
            garageDoorOpen = 1;
  }
  else if (x == 0 && garageDoorOpen == 1)
  {
    for (angle = 180; angle >= 90; angle-=1)
         {
            garageDoor.write(angle);
            delay(30);
         }
     garageDoorOpen = 0;
 }
 else
 {
        garageDoor.write(90);

 }
}
void flame_sensor(){
  f=digitalRead(flameSensorButton); //PIN 44
  if (f==0)
  {
    digitalWrite(buzzer_pin,HIGH); //pin 8
    delay(300);
    digitalWrite(buzzer_pin,LOW);
    delay(100);
    digitalWrite(buzzer_pin,HIGH);
    delay(300);
    digitalWrite(buzzer_pin,LOW);
    delay(100);
  }
}
void smoke_sensor(){
  int smoke=digitalRead(36);
  smoke_seconds=millis()/1000 ;  
  if(smoke==1&&(smoke_seconds - previous_smoke_time > eventInterval))
  {
    digitalWrite(35,LOW);
  } 
  else if(smoke==0){
    digitalWrite(35,HIGH);
    previous_smoke_time=smoke_seconds;
  }
}
void temp_sensor()
{
if (houseDoorOpen == 1) //depend on enterTheHouse();
{
  t=digitalRead(6); 
  temp_seconds=millis()/1000 ;  
   if(t==1&&(temp_seconds - previous_temp_time > eventInterval))
  {
    digitalWrite(fan,LOW); //pin 53
  } 
  else if(t==0){
  digitalWrite(fan,HIGH);
    previous_temp_time=temp_seconds;
  }
}
else
{
      digitalWrite(fan,LOW); //pin 53

}
}
void pirSensor()
{
  if (houseDoorOpen == 0) //depend on enterTheHouse();
  {
      pir = digitalRead(input_of_PIR_sensor); //pin 38
      if(pir==1) { digitalWrite(output_of_PIR_sensor, HIGH); seconds=millis()/1000 ; } //pin 39
      else if (pir==0&& ((millis()/1000)>seconds+2) ) digitalWrite(output_of_PIR_sensor, LOW); 
  }
  else
    digitalWrite(output_of_PIR_sensor, LOW); 
}

void bluetooth()
{
    if(Serial.available() > 0)  // Send data only when you receive data:  
   {  
      data = Serial.read();        //Read the  incoming  data and store it into variable data  
      switch (data)
      {
        case 'a': digitalWrite(13, HIGH);break;
        case 'b': digitalWrite(13, LOW);break;
        case 'c': digitalWrite(14, HIGH);break;
        case 'd': digitalWrite(14, LOW);break;
        case 'e':
        {
          if (garageDoorOpen == 0)
          {
            for (angle = 90; angle < 180; angle+=1)
            {
              digitalWrite(ledGarage, HIGH); //pin 40
              garageDoor.write(angle); //PMW pin 11
              delay(30);
            }
            digitalWrite(ledGarage, LOW);
            garageDoorOpen = 1;
          }
          break;
        }
        case 'f':
        {
            if (garageDoorOpen == 1)
            {
            for (angle = 180; angle >= 90; angle-=1)
            {
              garageDoor.write(angle);
              delay(30);
            }
            garageDoorOpen = 0;
            }       
          break;
        }

      }
   }

 }


void loop() {
   // put your main code here, to run repeatedly:
  enterTheHouse(); //done
    gardenDoor();
   temp_sensor();
   flame_sensor(); 
   Ultrasonic_For_Garage();
   LDR_Sensor();
   smoke_sensor();
   pirSensor();
  bluetooth();
}
