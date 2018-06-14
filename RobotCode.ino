#include <SharpIR.h>


void Read_sensors(void);
int priority(void);
void Next_Movement(void);
void setdirection(void);
void Move_Maze(void);
float ReadSensor(int trigPin, int echoPin);
void Read_sensors(void);
void getPIDvalues(void);
void kill(void);
void straightBreakoout(void);
void TurnLeft(void);
void TurnRight(void);
void TurnAround(int side);
void Fill_Array(void);
void Check_Array(void);
void Breakout(void);
void straightPID(void);
void Print_Array(void);
void Check(void);
void getLEFT_PIDvalues(void);




//=========Variable declaration ==========
/*Star wars theme*/


const int buzzerPin = 7;
const int ledPin1 = 8;
const int ledPin2 = 4;
int counter = 0;

  const int c = 261;
  const int d = 294;
  const int e = 329;
  const int f = 349;
  const int g = 391;
  const int gS = 415;
  const int a = 440;
  const int aS = 455;
  const int b = 466;
  const int cH = 523;
  const int cSH = 554;
  const int dH = 587;
  const int dSH = 622;
  const int eH = 659;
  const int fH = 698;
  const int fSH = 740;
  const int gH = 784;
  const int gSH = 830;
  const int aH = 880;

/* Motor Pins */
const int ML_fwdPin = 11;
const int ML_revPin = 10;
const int MR_fwdPin = 5;
const int MR_revPin = 6;


/*Random Variables */
int R_power, L_power;
float setPoint = 6.5;
int Movement;
int TEST_LED_PIN = 4;
int left_turn = 10;
int count = 0;

/* set distances*/
int Front_setDistance = 10;
int Side_setDistance = 15;

/*PID control*/
int rightBaseSpeed = 136;
int leftBaseSpeed = 130;
//int rightBaseSpeed = 165;
//int leftBaseSpeed = 160;
int error = 0, lastError = 0;
float motorSpeed;
float Kd = 1, Kp = 4.5;

int L_error = 0, L_lastError = 0;
float L_Kd = 1, L_Kp = 1.5;
float L_setPoint = 6.5;

/*Array Maze Storage*/
int Maze[10][10];
int Row_Pos = 1, Col_Pos = 1;

/* stores distance when wall disappears */
float Distance_NoWall;

/*Cheese detection*/
const int Cheese_IR_SENSOR_PIN = A0;
int Cheese_sensorValue;

/* direction */
char direction;
float Current_Distance;
int Initial_Distance = 0;

/*wheel encoders*/
const int R_IR_SENSOR_PIN = A1;
int Num_PizzaSlices;
int Last_Shade;
int sensorValue;

/*front IR sensor*/
const int FPin = A3;
int F_distance;

/*right IR sensor*/
const int RPin = A4;
int R_distance;

/*left IR sensor*/
const int LPin = A2;
int L_distance;



//=========================Initialization =================================================================

void setup() {
        pinMode(buzzerPin, OUTPUT);
        pinMode(ledPin1, OUTPUT);
        pinMode(ledPin2, OUTPUT);
        pinMode(TEST_LED_PIN, OUTPUT);
        pinMode(MR_fwdPin, OUTPUT);
        pinMode(MR_revPin, OUTPUT);
        pinMode(ML_fwdPin, OUTPUT);
        pinMode(ML_revPin, OUTPUT);
        pinMode(FPin, OUTPUT);
        pinMode(RPin, OUTPUT);
        pinMode(LPin, OUTPUT);
        Serial.begin(9600);
        Maze[Row_Pos][Col_Pos] = 1; // set initial location to visited
        direction = 'W';
        Fill_Array();
        Check();

}




//========================= Loop ====================================================================
void loop() {
/* READ IR SENSOR ON GROUND HERE IF ACTIVE THEN KILL PROGRAM AND EXECUTE INFINITE LOOP LED FUNCTION */
        Next_Movement();
        Check_Array();
        if(left_turn == 3) Movement = 2;
        while(true){
                switch (Movement)
                {
                      case 1:          //right
                              TurnRight();        //turns right
                              setdirection();     //sets new direction after turn
                              left_turn++;
                              break;
                      case 2:         //left
                              TurnLeft();
                              setdirection();
                              left_turn++;
                              break;
                      case 3:        //180 left
                              TurnAround(1);
                              setdirection();
                              left_turn++;
                              break;
                      case 4:        //180 right
                              TurnAround(2);
                              setdirection();
                              left_turn++;
                              break;

                      case 6:
                      /*for testing*/
                              //Read_sensors();
                              //Cheese_sensorValue = analogRead(Cheese_IR_SENSOR_PIN);
                              //Serial.println(Cheese_sensorValue);
                              break;

                      default:       //forward
                              if( priority() == 1){ if(count == 2)left_turn = 0; count++; continue;}      //checks squares on sides and in front where there isnt a wall if the square has been visited. always go straight unless that block has been visited but to the side has not.
                              straightPID();       //moves forward 30 cm using PID control while storing a value of no wall exists to compare in Next_Movement function
                              Move_Maze();      // moves array one over in direction robot moved in straight() by using direction, then stores that you have been there with a 1.
                              Print_Array();
                              left_turn++;
                              break;
                }
                break;
          }

}
/*====================== Checks if array has been fully searched =================================================================*/
void Check_Array(void){
   int sum = 0;
   for(int j = 0; j < 10; j++){
       for(int i = 0; i < 10; i++){
           sum = sum + Maze[i][j];
      }
    }
   if(sum == 100){
    Celebration();
   }
}
/* ============================= Checks for cheese ========================================================================*/

void Check(void){

     long int timenow = millis();
     long int check = 0;
     while( (millis() - timenow) < 200){
         Cheese_sensorValue = analogRead(Cheese_IR_SENSOR_PIN);
         //Serial.println(Cheese_sensorValue);
         if( Cheese_sensorValue > 150)
            check++;
     }
     if( check > 100){
      while(true){
        if(counter % 2 == 0)
        {
          digitalWrite(ledPin1, HIGH);
          delay(250);
          digitalWrite(ledPin1, LOW);
        }else
        {
          digitalWrite(ledPin2, HIGH);
          delay(250);
          digitalWrite(ledPin2, LOW);
         }
        counter++;
        }
      }


}

//====================================================================================================== Maze Movement Functions =====================================================================================

int priority(void){
              Read_sensors();
              switch (direction)
              {
              case 'W':   //West is positive x axis(rows)
                      if(Maze[Row_Pos + 1][Col_Pos] == 0) { return(0); }
                      else if(L_distance > Side_setDistance && Maze[Row_Pos][Col_Pos - 1] == 0){Movement = 2; return(1); }
                      else if(R_distance > Side_setDistance && Maze[Row_Pos][Col_Pos + 1] == 0){Movement = 1; return(1); }
                      break;

              case 'N':   //North is positive y axis(cols)
                      if(Maze[Row_Pos][Col_Pos + 1] == 0) { return(0); }
                      else if(L_distance > Side_setDistance && Maze[Row_Pos + 1][Col_Pos] == 0){Movement = 2; return(1); }
                      else if(R_distance > Side_setDistance && Maze[Row_Pos - 1][Col_Pos] == 0){Movement = 1; return(1); }
                      break;

              case 'E':  //East is negative x axis(rows)
                      if(Maze[Row_Pos - 1][Col_Pos] == 0) { return(0); }
                      else if(L_distance > Side_setDistance && Maze[Row_Pos][Col_Pos + 1] == 0){ Movement = 2; return(1); }
                      else if(R_distance > Side_setDistance && Maze[Row_Pos][Col_Pos - 1] == 0){ Movement = 1; return(1); }
                      break;

              case 'S':  //South is negative y axis(cols)
                      if(Maze[Row_Pos][Col_Pos - 1] == 0) { return(0); }
                      else if(L_distance > Side_setDistance && Maze[Row_Pos - 1][Col_Pos] == 0){ Movement = 2; return(1); }
                      else if(R_distance > Side_setDistance && Maze[Row_Pos + 1][Col_Pos] == 0){ Movement = 1; return(1); }
                      break;
              }
              return(0);

}

void Next_Movement(void){
        /* check if mouse has to turn */
        Read_sensors(); /*reads all sensors and returns distances from front right and left*/
        if( Current_Distance - Distance_NoWall > 10 )                                                              {  Movement = 1; Distance_NoWall = 1000; /* 1 = RIGHT */ }
        else if( F_distance < Front_setDistance && R_distance > Side_setDistance && L_distance < Side_setDistance) {  Movement = 1; /* 1 = RIGHT */ }
        else if( F_distance < Front_setDistance && R_distance > Side_setDistance && L_distance > Side_setDistance) {  Movement = 1; /* 1 = RIGHT */ }
        else if( F_distance < Front_setDistance && R_distance < Side_setDistance && L_distance > Side_setDistance) {  Movement = 2; /* 2 = LEFT*/   }
        else if( F_distance < Front_setDistance && R_distance < Side_setDistance && L_distance < Side_setDistance) {
                if (R_distance < L_distance) { Movement = 4; /* 3 = 180 LEFT */ }
                else                       { Movement = 3; /* 4 = 180 RIGHT */}}
        else{Movement = 0;}}
void setdirection(void){
        switch (Movement)

        {
        case 1:    //right
                if     (direction == 'N') { direction = 'E'; }
                else if(direction == 'E') { direction = 'S'; }
                else if(direction == 'S') { direction = 'W'; }
                else if(direction == 'W') { direction = 'N'; }
                break;

        case 2:   //left
                if     (direction == 'E') { direction = 'N'; }
                else if(direction == 'S') { direction = 'E'; }
                else if(direction == 'W') { direction = 'S'; }
                else if(direction == 'N') { direction = 'W'; }
                break;

        case 3:  //180 left
                if     (direction == 'N') { direction = 'S'; }
                else if(direction == 'E') { direction = 'W'; }
                else if(direction == 'S') { direction = 'N'; }
                else if(direction == 'W') { direction = 'E'; }
                break;

        case 4:  //180 right
                if     (direction == 'N') { direction = 'S'; }
                else if(direction == 'E') { direction = 'W'; }
                else if(direction == 'S') { direction = 'N'; }
                else if(direction == 'W') { direction = 'E'; }
                break;
        }
}

void Move_Maze(void){
        switch (direction)
        {
        case 'W':   //north is positive x axis(rows)
                Row_Pos++;
                Maze[Row_Pos][Col_Pos] = 1;
                break;

        case 'N':   //east is positive y axis(cols)
                Col_Pos++;
                Maze[Row_Pos][Col_Pos] = 1;
                break;

        case 'E':  //south is negative x axis(rows)
                Row_Pos--;
                Maze[Row_Pos][Col_Pos] = 1;
                break;

        case 'S':  //west is negative y axis(cols)
                Col_Pos--;
                Maze[Row_Pos][Col_Pos] = 1;
                break;
        }

}

/*==========Read_sensors and pid control===============*/

float ReadSensor(int Pin){
         SharpIR sharp(Pin, 430);
         int dist = sharp.distance();

         if(dist < 4)
                dist = 3;
         else if(dist > 30)
                dist = 31;

         return(dist);
}
void Read_sensors(void){
        L_distance = ReadSensor(LPin);
        R_distance = ReadSensor(RPin);
        F_distance = ReadSensor(FPin);
}
void getPIDvalues(void){
        R_distance = ReadSensor(RPin);
        error = R_distance - setPoint;
        motorSpeed = Kp * error + Kd * (error - lastError);
        lastError = error;
        R_power = rightBaseSpeed - motorSpeed;
        L_power = leftBaseSpeed + motorSpeed;

}
void getLEFT_PIDvalues(void){
        L_distance = ReadSensor(LPin);
        L_error = L_distance - L_setPoint;
        motorSpeed = L_Kp * L_error + L_Kd * (L_error - L_lastError);
        L_lastError = L_error;
        R_power = rightBaseSpeed + motorSpeed;
        L_power = leftBaseSpeed - motorSpeed;

}
/*===========turning and motor control functions====================*/

void kill(void){
        /* Motors first break then kills power*/
        analogWrite(MR_fwdPin, 205);
        analogWrite(ML_fwdPin, 200);
        analogWrite(MR_revPin, 225);
        analogWrite(ML_revPin, 230);
        delay(300);
        analogWrite(MR_fwdPin, 0);
        analogWrite(ML_fwdPin, 0);
        analogWrite(MR_revPin, 0);
        analogWrite(ML_revPin, 0);
        Check();
}
void straightPID(void){

        /* make initial distance and counter values equal to zero*/
        Current_Distance = Initial_Distance;
        Distance_NoWall = 1000;
        Num_PizzaSlices = 0;

        /*read wheel encoder analog sensor value*/
        Last_Shade = analogRead(R_IR_SENSOR_PIN);


        while(Num_PizzaSlices < 48){

               sensorValue = analogRead(R_IR_SENSOR_PIN);
               Read_sensors();
               getPIDvalues();
               if(R_distance > 15){
                   R_power = rightBaseSpeed;
                   L_power = leftBaseSpeed;
               }
               if(R_distance > 30 && L_distance < 20){
                   getLEFT_PIDvalues();
                   }

               Current_Distance = (Num_PizzaSlices) * 1.20411;

               if(R_distance < Side_setDistance) {
                   Distance_NoWall = Current_Distance;
                        /* stores distance value continuously until wall on right side disappears then the value does not change this is to
                        compare value when done moving to this value in order to see if robot has to turn right */
                }

               if(Last_Shade > 500 && sensorValue < 500 )
               {
                   Num_PizzaSlices++;
               }
                else if(Last_Shade < 500 && sensorValue > 500)
               {
                   Num_PizzaSlices++;
               }
               Last_Shade = sensorValue;
               analogWrite(MR_fwdPin, R_power);
               analogWrite(ML_fwdPin, L_power);


               if(F_distance < 5)
                      {break;}

    }
        kill(); // kills motors to stop robot moving from here out

}

void TurnLeft(void){

                      analogWrite(ML_revPin, 118);
                      analogWrite(MR_fwdPin, 125);
                      delay(820);
                      kill();
}

void TurnRight(void){

                      analogWrite(MR_revPin, 125);
                      analogWrite(ML_fwdPin, 118);
                      delay(780);
                      kill();

}


void TurnAround(int side){

        if(side == 1) {
                      analogWrite(MR_revPin, 125);
                      analogWrite(ML_fwdPin, 118);
                      delay(1670);
                      kill();
        }
        else if(side == 2) {
                      analogWrite(ML_revPin, 118);
                      analogWrite(MR_fwdPin, 125);
                      delay(1670);
                      kill();
        }

}
/*===============================================================================Celebration=============================================================================*/

void Celebration(void){


  counter = 0;
  while(true){
    analogWrite(MR_fwdPin, 200);
    analogWrite(ML_revPin, 200);

    //Play first section
    firstSection();

    //Play second section
    secondSection();

    //Variant 1
    beep(f, 250);
    beep(gS, 500);
    beep(f, 350);
    beep(a, 125);
    beep(cH, 500);
    beep(a, 375);
    beep(cH, 125);
    beep(eH, 650);

    delay(500);

    //Repeat second section
    secondSection();

    //Variant 2
    beep(f, 250);
    beep(gS, 500);
    beep(f, 375);
    beep(cH, 125);
    beep(a, 500);
    beep(f, 375);
    beep(cH, 125);
    beep(a, 650);

    delay(650);
  }
}

void beep(int note, int duration)
{
  //Play tone on buzzerPin
  tone(buzzerPin, note, duration);

  //Play different LED depending on value of 'counter'
  if(counter % 2 == 0)
  {
    digitalWrite(ledPin1, HIGH);
    delay(duration);
    digitalWrite(ledPin1, LOW);
  }else
  {
    digitalWrite(ledPin2, HIGH);
    delay(duration);
    digitalWrite(ledPin2, LOW);
  }

  //Stop tone on buzzerPin
  noTone(buzzerPin);

  delay(50);

  //Increment counter
  counter++;
}

void firstSection()
{

  beep(a, 500);
  beep(a, 500);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);

  delay(500);

  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);

  delay(500);
}

void secondSection()
{


  beep(aH, 500);
  beep(a, 300);
  beep(a, 150);
  beep(aH, 500);
  beep(gSH, 325);
  beep(gH, 175);
  beep(fSH, 125);
  beep(fH, 125);
  beep(fSH, 250);

  delay(325);

  beep(aS, 250);
  beep(dSH, 500);
  beep(dH, 325);
  beep(cSH, 175);
  beep(cH, 125);
  beep(b, 125);
  beep(cH, 250);

  delay(350);
}
/* ======================================================================Set outside wall of array = 1====================================================================*/
void Fill_Array(void){
      for(int i = 0; i < 10; i++){
             Maze[i][0] = 1;

      }
      for(int i = 0; i < 10; i++){
             Maze[i][9] = 1;

      }
      for(int i = 0; i < 10; i++){
             Maze[0][i] = 1;

      }
      for(int i = 0; i < 10; i++){
             Maze[9][i] = 1;

      }
}

/*================================================================== Prints array including location for testing =============================================================*/
void Print_Array(void){
   for(int j = 0; j < 10; j++){
       for(int i = 0; i < 10; i++){
             Serial.print(Maze[i][j]);
             Serial.print("   ");
      }
      Serial.println("");
  }
  Serial.println("--------------------------------------------------------------------------------------------------------------------------------------------");
  Serial.println();
}
