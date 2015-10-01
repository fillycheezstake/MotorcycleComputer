#include <Bounce2.h>
#include <MenuSystem.h>
#include <Encoder.h>
#include <LiquidCrystal.h>

#define ENC_L_BUT 34
#define ENC_R_BUT 36

#define ENC_R_WIRE1 2 //first pair of interupts
#define ENC_R_WIRE2 3

#define ENC_L_WIRE1 18 //second pair of interupts
#define ENC_L_WIRE2 19

#define RS 22
#define EN 24
#define DB4 26
#define DB5 28
#define DB6 30
#define DB7 32


//use debouncer library
Bounce debouncerLeft = Bounce(); 
Bounce debouncerRight = Bounce(); 

//menu selected? (for different screens)
bool heatselect = false;

//create an lcd object - use display wires: (RS,EN,DB4,DB5,DB6,DB7)
LiquidCrystal lcd(RS, EN, DB4, DB5, DB6, DB7);

//create an encoder object (wire1, wire2) - you can reverse the wires if the direction is wrong.
Encoder knobRight(ENC_R_WIRE1, ENC_R_WIRE2);
Encoder knobLeft(ENC_L_WIRE1, ENC_L_WIRE2);

// menu variables
MenuSystem ms;
Menu mm("Main Menu");
MenuItem mm_mi1("Heat Control");
Menu mu1("Live Data");
MenuItem mu1_mi1("Tach");
MenuItem mu1_mi2("GPS Speed");
MenuItem mu1_mi3("Speed");
MenuItem mu1_mi4("G Forces");
Menu mu2("Stats");
MenuItem mu2_mi1("Max Speed");
MenuItem mu2_mi2("Max RPM");
MenuItem mu2_mi3("Max G");

void on_heatcontrol_selected(MenuItem* p_menu_item)
{
  heatselect = true;
}

void on_item2_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0, 2);
  lcd.print("Item2 Selected  ");
  delay(1500); // so we can look the result on the LCD
}

void on_item3_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0, 2);
  lcd.print("Item3 Selected  ");
  delay(1500); // so we can look the result on the LCD
}

//====================================
//this code gets run once
void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);
  
  //giving buttons the debounce library - i think this library helps to clean up the main code a bit
  pinMode(ENC_L_BUT,INPUT_PULLUP);
  debouncerLeft.attach(ENC_L_BUT);
  debouncerLeft.interval(5);
  pinMode(ENC_R_BUT,INPUT_PULLUP);
  debouncerRight.attach(ENC_R_BUT);
  debouncerRight.interval(5);

  /*
  Menu Structure:
   -Heat Control
   -Live Data
   --Tach
   --GPS Speed
   --Speed
   --G Forces
   -Stats
   --Max Speed
   --Max RPM
   --Max G
   */
  
  //add in the menus, items, and their select actions
  mm.add_item(&mm_mi1, &on_heatcontrol_selected);
  mm.add_menu(&mu1);
  mu1.add_item(&mu1_mi1, &on_item3_selected);
  mu1.add_item(&mu1_mi2, &on_item3_selected);
  mu1.add_item(&mu1_mi3, &on_item3_selected);
  mu1.add_item(&mu1_mi4, &on_item3_selected);
  mm.add_menu(&mu2);
  mu2.add_item(&mu2_mi1, &on_item3_selected);
  mu2.add_item(&mu2_mi2, &on_item3_selected);
  mu2.add_item(&mu2_mi3, &on_item3_selected);
  ms.set_root_menu(&mm);
  displayMenu();
}
//====================================

//prints the menu to the lcd
void displayMenu() {
  lcd.clear();  //clear the screen
  lcd.setCursor(0, 0); //make sure the cursor is home
  
  // Display the menu
  Menu const* cp_menu = ms.get_current_menu();
  lcd.print(cp_menu->get_name());  //current menu name
  lcd.setCursor(0, 1); //advance to next line
  lcd.print(cp_menu->get_selected()->get_name()); //the current menu item
}



//global variables
long positionRight = 0;
long positionLeft = 0;
int jacketsetting = 0;
int glovesetting = 0;

//main loop =================================================
void loop() {
  buttonHandler();
}
//===========================================================


void heatcontroldisplay() {
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(127);
  lcd.print("Gloves  Jacket");
  lcd.write(126);
  lcd.setCursor(0,1);
  int a;
  for (a=0; a < glovesetting/14.29; a++){
    lcd.write(255); 
  }
  lcd.setCursor(9,1);

  for (a=0; a < jacketsetting/14.29; a++){
    lcd.write(255); 
  }
}


void buttonHandler() {
  long  newRight;
  newRight = knobRight.read() / 4;
  long newLeft;
  newLeft = knobLeft.read() / 4;

  debouncerLeft.update();
  debouncerRight.update();

  int leftbut = debouncerLeft.read();
  int rightbut = debouncerRight.read();

  //left encoder clockwise
  if (heatselect){
      if (newLeft > positionLeft) { 
        if (glovesetting < 100) {
          glovesetting=glovesetting+2;
          heatcontroldisplay();
        }
       positionLeft = newLeft;
      }
      //left encoder counterclockwise
      if (newLeft < positionLeft) { 
        if (glovesetting > 0) {
          glovesetting=glovesetting-2;
          heatcontroldisplay();
        }
       positionLeft = newLeft;
      }
  }


  //right encoder clockwise
  if (newRight > positionRight) { 
    if (heatselect && jacketsetting < 100){
      jacketsetting=jacketsetting+2;
      heatcontroldisplay();
    }
    else {
      ms.next();
      displayMenu();   
    }
    positionRight = newRight;
  }
  //right encoder counter clockwise
  if (newRight < positionRight) {
    if (heatselect && jacketsetting > 0) {
      jacketsetting=jacketsetting-2;
      heatcontroldisplay();
    }
    else{
      ms.prev();
      displayMenu(); 
    }
    positionRight = newRight;
  }
    
  
  if (leftbut == LOW) {
    if (heatselect){    
      heatselect = false;
    }
    else {
      ms.back();
      displayMenu();      
    }
  }

  
  if (rightbut == LOW)   {
    if (heatselect){
      heatcontroldisplay();
    }
    else {
      ms.select();
      displayMenu();      
    }
  }
}

