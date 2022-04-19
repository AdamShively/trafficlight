#define maj_vert_light 2  //Compiler replaces mentioned with 2 at compile time
#define min_horiz_light 3  //Compiler replaces mentioned with 3 at compile time

#define min_car_sensor 4  //Compiler replaces mentioned with 4 at compile time
const int maj_vert_cross = 5;  //Variable intialization
const int min_horiz_cross = 6;  //Variable intialization
#define speaker 7  //Compiler replaces mentioned with 7 at compile time

#define maj_green 8  //Compiler replaces mentioned with 8 at compile time
#define maj_yellow 9  //Compiler replaces mentioned with 9 at compile time
#define maj_red 10  //Compiler replaces mentioned with 10 at compile time

#define min_green 11  //Compiler replaces mentioned with 11 at compile time
#define min_yellow 12  //Compiler replaces mentioned with 12 at compile time
#define min_red 13  //Compiler replaces mentioned with 13 at compile time

const int maj_green_length = 30;  //Seconds green light active for major road
const int min_green_length = 20;  //Seconds green light active for minor road
const int yellow_length = 3;  //Seconds yellow light active
const int dead_time_length = 2; //Seconds red light active on both roads at end of cycle

boolean horizFlag = false;  //Pedestian waiting to cross major road signal
boolean vertFlag = false; //Pedestian waiting to cross major road signal
boolean carFlag = false;  //Car on minor road waiting signal

enum SM_States { SM_Start, SM_s0, SM_s1, SM_s2, SM_s3, SM_s4, SM_s5, SM_s6, SM_s7 } SM_State;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);     //opens serial port
  randomSeed(analogRead(0));      //seed function
}

//majGreenMinRed() function changes LEDs to on/off
void majGreenMinRed(){
  //major road traffic lights
    digitalWrite(maj_green, HIGH);
    digitalWrite(maj_yellow, LOW);
    digitalWrite(maj_red, LOW);
           
    //minor road traffic lights
    digitalWrite(min_green, LOW);
    digitalWrite(min_yellow, LOW);
    digitalWrite(min_red, HIGH);
}

//minGreenMajRed() function changes LEDs to on/off
void minGreenMajRed(){
  //major road traffic lights
    digitalWrite(maj_green, LOW);
    digitalWrite(maj_yellow, LOW);
    digitalWrite(maj_red, HIGH);

    //minor road traffic lights  
    digitalWrite(min_green, HIGH);
    digitalWrite(min_yellow, LOW);
    digitalWrite(min_red, LOW);
}

//deadPeriod() function changes LEDs to red on, others off
void deadPeriod(){
   //major road traffic lights
    digitalWrite(maj_green, LOW);
    digitalWrite(maj_yellow, LOW);
    digitalWrite(maj_red, HIGH);

    //minor road traffic lights  
    digitalWrite(min_green, LOW);
    digitalWrite(min_yellow, LOW);
    digitalWrite(min_red, HIGH);
}

//crossWalkSignals() function turns pedestrian light on and beeps
//After 13 seconds less than green light time has passed, beeping speeds up/ light goes off
//light param is the pin number of the light to turn on then off
//greenLightTime param is the length of that road's green light

void crossWalkSignals(int light, int greenLightTime){
  digitalWrite(light, HIGH);      //light on
  for(int i = 0; i < greenLightTime; i++){
    if(i <= greenLightTime - 13){
      Serial.println("Pedstrians WALK!");      //for debugging and presentaion purposes
      tone(speaker, 3000);        //tone on
      delay(100);
      noTone(speaker);            //tone off
      delay(900);
    }
    else{
      Serial.println("Pedstrians DO NOT WALK unless already in crosswalk!");      //for debugging and presentaion purposes
      digitalWrite(light, LOW);   //light off

      for(int j = 0; j < 2; j++){
        tone(speaker, 5000);      //tone on
        delay(100);
        noTone(speaker);          //tone off
        delay(400);
      }
    }
    crossWalkFlags();
  }
}

//crossWalkFlags() function checks if pedestrian buttons are being pushed
void crossWalkFlags(){
  if(digitalRead(maj_vert_cross) == 0){
    Serial.println("PEDESTRIAN button pushed to cross north/south crosswalk is ACTIVE!");      //for debugging and presentaion purposes
    vertFlag = true;
  }
  else if(digitalRead(min_horiz_cross) == 0){ 
    Serial.println("PEDESTRIAN button pushed to cross west/east crosswalk is ACTIVE!");      //for debugging and presentaion purposes
    horizFlag = true;
  }
  
}

//majNoTraffic() creates a 7% chance of no traffic for major road
boolean majNoTraffic(){
  if(random(15) == 0){
    Serial.println("NO traffic is coming on Major Road!"); 
    return true;
  }
  else{
    Serial.println("Traffic is coming on Major Road!");      //for debugging and presentaion purposes
    return false;
  }
}

//minNoTraffic() creates a 15% chance of no traffic for minor road
boolean minNoTraffic(){
  if(random(8) == 0){
    Serial.println("NO traffic is coming on Minor Road!"); 
    return true;
  }
  else{
    Serial.println("Traffic is coming on Minor Road!");      //for debugging and presentaion purposes
    return false;
  }
}

//LightFct_SM() simulates a four way intersection traffic light using a state machine
void LightFct_SM(){
   switch(SM_State) {   // Transitions
     case SM_Start:  // Initial transition
        SM_State = SM_s0;
        break;
     case SM_s0:                            //major road green 
          SM_State = SM_s1;  
          break;
     case SM_s1:                           //yellow light for major
          SM_State = SM_s2;  
           break;
     case SM_s2:                            //dead period
          if(horizFlag){                  
            SM_State = SM_s7;               //if pedestrian cross active
          }
          else{
            SM_State = SM_s3;  
          }
           break;
     case SM_s3:                            //min road green
          SM_State = SM_s4;                           
          break;
     case SM_s4:                            //yellow min road
          SM_State = SM_s5;  
           break;
     case SM_s5:                            //dead period
          if(vertFlag){
            SM_State = SM_s6;               //if pedestrian cross active
          }
          else{
            SM_State = SM_s0;
          }
           break;
     case SM_s6:                            //major highway green pedestrian cross
          SM_State = SM_s1;
          break;
          
     case SM_s7:                          //minor highway green pedestrian cross
          SM_State = SM_s4;  
          break;
     default:
        break;
    } // Transitions
    switch(SM_State) {   // State actions
     case SM_s0:                            //major road green, 30 secs if loop not conditionally broken
          Serial.println("STATE 0: Major road is GREEN, Minor road is RED");      //for debugging and presentaion purposes
          majGreenMinRed();
          for(int i = 0; i < maj_green_length; i++){  
            delay(1000);
            crossWalkFlags();
            if(digitalRead(min_car_sensor) == 0){
              Serial.print("Car is waiting on the minor road, ACTIVE!\n");      //for debugging and presentaion purposes
              carFlag = true;
             }
            if(majNoTraffic() && (horizFlag || carFlag)){
              if(horizFlag){
                Serial.print("NO TRAFFIC AND PEDESTRIAN SIGNAL ACTIVE, ENDING CURRENT STATE\n");      //for debugging and presentaion purposes
              }
              else if(carFlag){
                Serial.print("NO TRAFFIC AND CAR WAITING SIGNAL ACTIVE, ENDING CURRENT STATE\n");      //for debugging and presentaion purposes
              }
              break;
            }
          }
          break;
     case SM_s1:                            //yellow light for major, 3 secs
          carFlag = false;
          Serial.print("STATE 1: Major road is YELLOW\n");      //for debugging and presentaion purposes
          //major road traffic lights        
          digitalWrite(maj_green, LOW);
          digitalWrite(maj_yellow, HIGH);
          digitalWrite(maj_red, LOW);

          //minor road traffic lights  
          digitalWrite(min_green, LOW);
          digitalWrite(min_yellow, LOW);
          digitalWrite(min_red, HIGH);
          for(int i = 0; i < yellow_length; i++){  
            delay(1000);
            crossWalkFlags();
          }
          break;
     case SM_s2:                           //dead period, 2 secs
          Serial.print("STATE 2: dead period, both roads RED\n");      //for debugging and presentaion purposes
          deadPeriod();
          for(int i = 0; i < dead_time_length; i++){  
            delay(1000);
            crossWalkFlags();
          }
          break;
     case SM_s3:                        //minor green, 20 secs if loop not conditionally broken
          Serial.print("STATE 3: Minor road is GREEN, Major road is RED\n");      //for debugging and presentaion purposes
          minGreenMajRed();
          for(int i = 0; i < min_green_length; i++){ 
            delay(1000);
            if(minNoTraffic() && vertFlag){
              Serial.print("NO TRAFFIC AND PEDESTRIAN SIGNAL ACTIVE, ENDING CURRENT STATE\n");      //for debugging and presentaion purposes
              break;
            }
            crossWalkFlags();
          }
          break;
     
     case SM_s4:                        //yellow light for minor, 3 secs
          Serial.print("STATE 4: Minor road is YELLOW\n");      //for debugging and presentaion purposes
          //major road traffic lights        
          digitalWrite(maj_green, LOW);
          digitalWrite(maj_yellow, LOW);
          digitalWrite(maj_red, HIGH);

          //minor road traffic lights  
          digitalWrite(min_green, LOW);
          digitalWrite(min_yellow, HIGH); 
          digitalWrite(min_red, LOW); 
          for(int i = 0; i < yellow_length; i++){ 
            delay(1000);
            crossWalkFlags();
          }
          break;

    case SM_s5:                          //dead period, 2 secs
              Serial.print("STATE 5: dead period, both roads RED\n");      //for debugging and presentaion purposes
              deadPeriod();
          for(int i = 0; i < dead_time_length; i++){       
            delay(1000);
            crossWalkFlags();
          }
          break;
    case SM_s6:                //major road green for 30 secs, pedestrian light and beeping signal active during
           Serial.print("STATE 6: Major road is GREEN, Minor road is RED, Pedestrian \n Crosswalk Sensor ACTIVE\n");      //for debugging and presentaion purposes
           majGreenMinRed();
           vertFlag = false;
           crossWalkSignals(maj_vert_light, maj_green_length);
           //crossWalkFlags();
          break;

    case SM_s7:               //minor road green for 20 secs, pedestrian light and beeping signal active during
         Serial.print("STATE 7: Minor road is GREEN, Major road is RED, Pedestrian \n Crosswalk Sensor ACTIVE\n");      //for debugging and presentaion purposes
         minGreenMajRed();
         horizFlag = false;
         crossWalkSignals(min_horiz_light, min_green_length);     
         break;
            
     default:
        break;

     } // State actions
}

void loop() {
  // put your main code here, to run repeatedly:
  LightFct_SM();   //function call
}
