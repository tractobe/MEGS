
/*T.G.Kinsey Febuary 2018
 Software to control the Electronic Gearhob 1
 Sieg C1 lathe with hobbing spindle
 3 stepper motors - 1) hob spindle, 2) lathe headstock, 3) lathe feedscrew
 Outputs high/low/high pulse to stepper motor controllers
*/
#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(4, 3, 9, 8, 7, 6);    // set pins for lcd

const byte stepMotor = 10;    // pin connected to stepper motor controller. Controls motor drivng headstock spindle
const byte stepMotorTwo = 11; // pin connected to stepper motor controller. Controls milling spindle
const byte stepMotorThree = 12;   // pin connected stepper motor controller. Controls power feed
const byte back = A0;         // pin connected to back button
const byte enter = A1;        // pin connected to enter button
const byte down = A2;         // pin connected to down button
const byte up = A3;           // pin connected to up button
const byte feedStart = A5;    // pin connected to feed start button
const byte feedStop = A4;     // pin connected to feed stop button
const byte feedSpeed = A6;    // pin connected to pot to control the feed speed
const byte feedLimitStop = 2; // pin connected to microswitch that stops feed when reaches end of travel
bool displaySpur = true;      // used to display spur gear option(true) or helical gear option(false)
bool displayRun = true;       // used to display run option(true) or set teeth option(false)
bool displayJogTeeth = true;
bool temp;                    // used to hold temp hold the state of display options
byte teeth;               // variable to store how many teeth are on the gear we are cutting
int stepDelay = 150;    // variable use to delay between steps. If too short stepper motor stalls
int speedOfFeed;    // var used to hold value of pot. Uesd to detirmine feed speed
int feedCount;      // var used to count up to trigger feed step
bool feed = false;    // if false feed is disabled, if true feed enabled
byte teethFeedValue;    //

//*************************
void setup() {
// turn lines to steppers high, steppers move one step when these lines go low for at least 50us
  digitalWrite(stepMotor, HIGH);
  digitalWrite(stepMotorTwo, HIGH);
  digitalWrite(stepMotorThree, HIGH);
// displays software version
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("IGear_18_V_1.2");
  lcd.setCursor(0,1);
  lcd.print("02/2018");
  
  // set pins to correct direction
  pinMode(stepMotor, OUTPUT);
  pinMode(stepMotorTwo, OUTPUT);
  pinMode(stepMotorThree, OUTPUT);
  pinMode (back, INPUT_PULLUP);
  pinMode (enter, INPUT_PULLUP);
  pinMode (down, INPUT_PULLUP);
  pinMode (up, INPUT_PULLUP);
  pinMode (feedStart, INPUT_PULLUP);
  pinMode (feedStop, INPUT_PULLUP);
  pinMode (feedLimitStop, INPUT_PULLUP);
   teeth = EEPROM.read(0);      // read the eeprom to get saved value for the number of teeth to cut
  delay(3000);                  // wait so you can read info
  //Serial.begin(9600);
  
}
//*************************
void loop() {

  
   pickGearType();          // go to function to pick type of gear to cut


}

//****************************

// gives you the choice of gear to cut. Only spur gears can be cut at the moment
void pickGearType(){
  spurDisplay();              // display function showing spur gear choice
 temp = displaySpur;          // store the current state of display Spur (true, display spur option or false display helical option)
  while(1){                   // keep looping until enter button pushed
    if((digitalRead(down) == 0) | (digitalRead(up) ==0)){     // if either up or down button pushed toggle state
      displaySpur = !displaySpur;
    }

  if(displaySpur != temp){        // if the state is different to the stored state up or down button has been pushed
    
    if(displaySpur == true){      // if displaySpur is true, call spur display function...
      spurDisplay();
    }
    else{
      helicalDisplay();           // ...of if false call helical display function
    }
  }
  if((digitalRead(enter) == 0) & (displaySpur == true)){  // if enter button and we are in spur state, go to run or set function
    runOrSet();
    spurDisplay();    // on return from run or set, display spur
    delay(50);        // debounce back button
    while(digitalRead(back)== 0){}    // wait until back button released
    delay(50);        // debounce back button
  }
  if((digitalRead(enter) == 0) & (displaySpur == false)){  // if enter button and we are in helical state, go to helical function
    // there is not helical gear option yet
    //runMainHelical();
    helicalDisplay();    // on return from run or set, display helical
    delay(50);        // debounce back button
    while(digitalRead(back)== 0){}    // wait until back button released
    delay(50);        // debounce back button
  }
  }
}
//***************************

// displays spur gear choice
void spurDisplay(){
  lcd.clear();   // clear the LCD
  lcd.setCursor(0, 0);
  lcd.print(">SPUR");
  lcd.setCursor(0,1);
  lcd.print("HELICAL");
  while((digitalRead(down) == 0) | (digitalRead(up) ==0)){}     // wait for up or down button release
  delay(50);    // debounce button
  temp = displaySpur;   // reset temp to current state so will wait for up or down button push
  
}
//**************************
// displays helical gear choice (not a choice as not able to cut helical gears yet)
void helicalDisplay(){
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("SPUR");
  lcd.setCursor(0,1);
  lcd.print(">HELICAL");
  while((digitalRead(down) == 0) | (digitalRead(up) ==0)){}    // wait for up or down button release
  delay(50);    // debounce button
  temp = displaySpur;     // reset temp to current state so will wait for up or down button push
}
//***************************
// displays the choice set number of teeth or start cutting
void runOrSet(){
  displayRun = true;
  runDisplay();
  temp = displayRun;              // store the current state of display run (true, display run option or false display set option)
  // debounce button and wait for release
  delay(50);
  while(digitalRead(enter)){}
  delay(500);
  while(1){
    if((digitalRead(down) == 0) | (digitalRead(up) ==0)){     // if either up or down button pushed toggle state
      displayRun = !displayRun;
    }

  if(displayRun != temp){       // if the state is different to the stored state up or down button has been pushed
    
    if(displayRun == true){      // if displayRun is true, call Run display function...
     runDisplay();
    }
    else{
      setDisplay();               // ...of if false call set display function 
    }
  }
  if((digitalRead(enter) == 0) & (displayRun == true)){     // if enter button pushed and display run true, go to run function
    runMain();
    delay(50);      // debounce button
    runDisplay();   // on return from run function display run option
    displayRun = true;
    while(digitalRead(back)== 0){}    // wait until back button released
    delay(50);       // debounce button 
  }
  if(digitalRead(back) == 0){     // if back button pushed back to spur or helical display
      return;
    }
    if((digitalRead(enter) == 0) & (displayRun == false)){      // if enter button pushed and display run is false, go to set function.
    setUp();
    delay(50);      // debounce button
    runDisplay();   // display run option 
    displayRun = true;  
    while(digitalRead(back)== 0){}    //wait until back button released
    delay(50);    // debounce button
  }
  }
  
}
//*****************************
// used by runOrSet to display that pressing enter button will go to run func
void runDisplay(){
  lcd.clear();        // clear lcd display
  lcd.setCursor(0, 0);    // set cursor to top left
  lcd.print(">RUN");
  lcd.setCursor(0,1);
  lcd.print("SET UP");
  while((digitalRead(down) == 0) | (digitalRead(up) ==0)){}   // wait until up or down button released
  delay(50);    // debounce switch
  temp = displayRun;
}
//****************************
// used by runOrSet to display that pressing enter button will go to set up func
void setDisplay(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RUN");
  lcd.setCursor(0,1);
  lcd.print(">SET UP");
  while((digitalRead(down) == 0) | (digitalRead(up) ==0)){}   // wait until up or down button released
  delay(50);
  temp = displayRun;
}
//********************************************************************************************************^^^^^^^^^^^^^^^^^^^^
// runs stepper motors to do the hobbing
void runMain(){
// display on lcd Running and cutting and number of teeth
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RUNNING");
  lcd.setCursor(0,1);
  lcd.print("CUTTING");
  lcd.setCursor(9, 1);
  lcd.print(teeth);
  feedCount = 0;
// keep looping until back button pushed
  while(1){
    if(digitalRead(feedStop)== 0){    // if stop feed button closed...
      feed = false;                   // ...set feed to false, to stop feed
    }
    if(digitalRead(feedStart) == 0){    // if feed start button closed...
      feed = true;                      // ...set feed to true, start feed
    }
    if(digitalRead(feedLimitStop) == 0  & feed == true){    // if feed is on and limit switch closed, return to run/setup function
      delayMicroseconds(100);   // stop spikes triggering the return
      if(digitalRead(feedLimitStop) ==0){
      feed = false;
      return;
      }
    }
// for loop steps hob round as many times as teeth then steps the headstock     
      for(byte i = 0;i<teeth;i ++){
        stepMillspindle();    // step hob round one step
        delayMicroseconds(stepDelay);
        if(digitalRead(back) == 0){   // if back button pushed back to run or set func
        return;
        }
      }
      stepHeadstock();    // step headstock round one step
      if(feed == true){     // if button has been pushed to start feed...
        feedCount++;        // ...add on to feedcount...
        speedOfFeed =analogRead(feedSpeed);   // ... get value from pot (0 to 1023)...
        speedOfFeed = map(speedOfFeed, 0, 1023, 0 ,10);     // map value 
        if(feedCount >= (speedOfFeed + teethFeedValue)){   //... if the value of the feedCount is greater than the pot value...
          stepFeed();     // ...step the feed stepper one step
          feedCount = 0;    // reset feedcount to zero ready to start counting up again
        }
      }
    }
  
}
//*******************************************$$$$$$$$$$$$$$$$$$
// this function won't run a only here as not fully worked out
void runMainHelical(){
  // display on lcd Running and cutting and number of teeth
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RUNNING HELIX");
  lcd.setCursor(0,1);
  lcd.print("CUTTING");
  lcd.setCursor(9, 1);
  lcd.print(teeth);
  int hobSpindleCount = 0;
  int feedCountH = 0;
  int indexCount = 0;
// keep looping until back button pushed
while((digitalRead(back) == 1) | (digitalRead(feedLimitStop) == 1)){
      stepMillspindle();
      delayMicroseconds(stepDelay);
      hobSpindleCount ++;
      indexCount ++;
      feedCountH ++;
      if(indexCount == 1599){
        stepHeadstock();
        indexCount = 0;
        hobSpindleCount = 0;
      }
      if(hobSpindleCount == 20 & indexCount <1599){
        stepHeadstock();
        hobSpindleCount = 0;
      }
      if(feedCountH == 15){
        stepFeed();
        feedCountH = 0;
      }
      
  }
}

//******************************%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setUp(){
  jogDisplay();
  temp = displayJogTeeth;              // store the current state of display run (true, jog run option or false display set teeth option)
  // debounce button and wait for release
  delay(50);
  while(digitalRead(enter)){}
  delay(500);
  while(1){
    if((digitalRead(down) == 0) | (digitalRead(up) ==0)){     // if either up or down button pushed toggle state
      displayJogTeeth = !displayJogTeeth;
    }

  if(displayJogTeeth != temp){       // if the state is different to the stored state up or down button has been pushed
    
    if(displayJogTeeth == true){      // if displayJogTeeth is true, call jog display function...
     jogDisplay();
    }
    else{
      setTeethDisplay();               // ...of if false call set teeth display function 
    }
  }
  if((digitalRead(enter) == 0) & (displayJogTeeth == true)){     // if enter button pushed and display run true, go to jog function
    jog();
    delay(50);      // debounce button
    displayJogTeeth == true;
    jogDisplay();   // on return from run function display run option
    while(digitalRead(back)== 0){}    // wait until back button released
    delay(50);       // debounce button 
  }
  if(digitalRead(back) == 0){     // if back button pushed back to run or set up display
      return;
    }
    if((digitalRead(enter) == 0) & (displayJogTeeth == false)){      // if enter button pushed and display run is false, go to set function.
    setTeeth();
    delay(50);      // debounce button;
    //displayJogTeeth == true;
    temp = true;
    displayJogTeeth = true;
    jogDisplay();   // display run option   
    while(digitalRead(back)== 0){}    //wait until back button released
    delay(50);    // debounce button
  }
  }
  
}
//****************************
// displays the option for the jog func
void jogDisplay(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">JOG");
  lcd.setCursor(0,1);
  lcd.print("SET TEETH");
  while((digitalRead(down) == 0) | (digitalRead(up) ==0)){}
  delay(50);
  temp = displayJogTeeth;
}
//**************************
// allows you to set number of teeth between 9 and 100
void setTeeth(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEETH =");
  whatTeeth();
  while((digitalRead(down) == 0) | (digitalRead(up) ==0)){}   // wait until up or down button released
  delay(50);        // debounce button
  while(1){
    if(digitalRead(back) == 0){     // if back button pushed back to spur or helical display
      teethFeedValue = teeth/10;
      EEPROM.write(0, teeth);       // store the current number of teeth in non volatile memory so can be recalled on reboot
      return;
        }
      if(digitalRead(up) == 0){   // up button pushed increment number of teeth
        teeth ++;
        if(teeth >100){           // stop the number of teeth from going over 100
          teeth = 100;
          }
        whatTeeth();
        delay(500);
        while(digitalRead(up)==0){   // keep looping while up button pushed (fast ups teeth)
               delay (200);      // wait 1/2 second
                teeth ++;
                if (teeth > 100)   // stops teeth going over hundred
                  {
                    teeth =100;
                  }
            whatTeeth();        // call display function to update screen
            }
          }
        
        
          
      
      if(digitalRead(down) == 0){
        teeth --;
        if(teeth <10){
          teeth = 10;
        }
         whatTeeth();
        delay(500);
        while(digitalRead(down)==0){   // keep looping while up button pushed (fast ups teeth)
               delay (200);      // wait 1/2 second
                teeth --;
                if (teeth <9)   // stops teeth going over hundred
                  {
                    teeth =9;
                  }
            whatTeeth();        // call display function to update screen
            }
        
        
      }
    
    
 
  }
}
//************************
void whatTeeth(){
  
      //lcd.begin(16, 2);
     // lcd.setCursor(0, 0);
     // lcd.print("TEETH =");
      lcd.setCursor(8,0);
      lcd.print("    ");
      lcd.setCursor(8,0);
      lcd.print(teeth);     
    
}

//**************************
void setTeethDisplay(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("JOG");
  lcd.setCursor(0,1);
  lcd.print(">SET TEETH");
  while((digitalRead(down) == 0) | (digitalRead(up) ==0)){}
  delay(50);
  temp = displayJogTeeth;
}
//**************************
void displayTeeth(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SET TEETH");
  lcd.setCursor(0,1);
  lcd.print(teeth);
}
//*******************************
// function to manually rotate headstock when UP or DOWN button pushed
void jog(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("JOG");
  lcd.setCursor(0,1);
  lcd.print("UP DOWN TO JOG");

  while(1){
    if((digitalRead(down) == 0) | (digitalRead(up) ==0)){   // if up or down button pushed...
      stepHeadstock();    //...rotate headstock
      delay(5);
    }
    if(digitalRead(back) == 0){     // if back button pushed back to jog or set teeth display
      return;
    }
}
}
// These fucntions when called, step their respective stepper motors one step
//**************************
void stepHeadstock(){
  digitalWrite(stepMotorTwo, LOW);
  delayMicroseconds(50);
  digitalWrite(stepMotorTwo, HIGH);
}
//***************************
void stepMillspindle(){
  digitalWrite(stepMotor, LOW);
  delayMicroseconds(50);
  digitalWrite(stepMotor, HIGH); 
}
//*****************************
void stepFeed(){
  digitalWrite(stepMotorThree, LOW);
  delayMicroseconds(50);
  digitalWrite(stepMotorThree, HIGH); 
}

