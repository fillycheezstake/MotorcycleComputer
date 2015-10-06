#include <Bounce2.h>
#include <MenuSystem.h>
#include <Encoder.h>
#include <LiquidCrystal.h>

#define ENC_L_BUT 19 //encoder buttons
#define ENC_R_BUT 18

#define ENC_R_WIRE1 16 //right encoder wires
#define ENC_R_WIRE2 17

#define ENC_L_WIRE1 14 //left encoder wires
#define ENC_L_WIRE2 15

#define RS 0 //lcd
#define EN 1
#define DB4 2
#define DB5 3
#define DB6 4
#define DB7 5
#define BLRed 9
#define BLGrn 10
#define BLBlu 11

#define GripsScreenDelay 1000 //time the heated grips screen appears for


//use debouncer library
Bounce debouncerLeft = Bounce();
Bounce debouncerRight = Bounce();

//menu selected? (for different screens)
bool heatselect = false;
bool backlightselect = false;

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
MenuItem mu1_mi5("Battery Voltage");
Menu mu2("Stats");
MenuItem mu2_mi1("Max Speed");
MenuItem mu2_mi2("Max RPM");
MenuItem mu2_mi3("Max G");
Menu mu3("Backlight Ctrl");
MenuItem mu3_mi1("Red");
MenuItem mu3_mi2("Blue");
MenuItem mu3_mi3("Green");


//global variables
long positionRight = 0;
long positionLeft = 0;
int jacketsetting = 0;
int glovesetting = 0;
int gripsetting = 0;
unsigned long lastgripscontrol = 0;

void on_heatcontrol_selected(MenuItem* p_menu_item)
{
  heatselect = true;
}

void on_backlightcontrol_selected(MenuItem* p_menu_item)
{
  backlightselect = true;
}

void on_item3_selected(MenuItem* p_menu_item)
{
  
}

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


void heatcontroldisplay() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(127);
  lcd.print("Gloves  Jacket");
  lcd.write(126);
  lcd.setCursor(0, 1);
  int a;
  for (a = 0; a < glovesetting / 14.29; a++) {
    lcd.write(255);
  }
  lcd.setCursor(9, 1);

  for (a = 0; a < jacketsetting / 14.29; a++) {
    lcd.write(255);
  }
}

void gripscontroldisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(127);
  lcd.print("Heated Grips");
  lcd.setCursor(0, 1);
  int a;
  for (a = 0; a < gripsetting / 6.25; a++) {
    lcd.write(255);
  }
  lcd.setCursor(6,1);
  lcd.print(gripsetting);
  lcd.write("%");
}

void backlightcontroldisplay() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Backlight Ctrl");
  lcd.setCursor(5,1);

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
  if (newLeft > positionLeft) {
    if (heatselect) {
      if (glovesetting < 100) {
        glovesetting = glovesetting + 2;
        heatcontroldisplay();
      }
    }
    else {
      gripscontroldisplay();
      lastgripscontrol = millis();
      if (gripsetting < 100)
        gripsetting = gripsetting + 2;
    }
    positionLeft = newLeft;
  }
  //left encoder counterclockwise
  if (newLeft < positionLeft) {
    if (heatselect) {
      if (glovesetting > 0) {
        glovesetting = glovesetting - 2;
        heatcontroldisplay();
      }
    }
    else {
      gripscontroldisplay();
      lastgripscontrol = millis();
      if (gripsetting > 0)
        gripsetting = gripsetting - 2;
    }
    positionLeft = newLeft;
  }


  //right encoder clockwise
  if (newRight > positionRight) {
    if (heatselect) {
      if (jacketsetting < 100) {
        jacketsetting = jacketsetting + 2;
        heatcontroldisplay();
      }
    }  
    else {
      ms.next();
      displayMenu();
    }
    positionRight = newRight;
  }
  //right encoder counter clockwise
  if (newRight < positionRight) {
    if (heatselect) { 
      if (jacketsetting > 0) {
        jacketsetting = jacketsetting - 2;
        heatcontroldisplay();
      }
    }
    else {
      ms.prev();
      displayMenu();
    }
    positionRight = newRight;
  }


  if (leftbut == LOW) {
    if (heatselect) {
      heatselect = false;
    }
    if (backlightselect){
      backlightselect = false;
    }
      ms.back();
      displayMenu();
  }


  if (rightbut == LOW)   {
    if (heatselect) {
    }
    if (backlightselect) {

    }
    else {
      ms.select();
      displayMenu();
    }
  }

  if (GripsScreenDelay + lastgripscontrol == millis()) {
    displayMenu();
  }

}







//=============================================================================================
//this code gets run once
void setup() {
  lcd.begin(16, 2);

  //giving buttons the debounce library - i think this library helps to clean up the main code a bit
  pinMode(ENC_L_BUT, INPUT_PULLUP);
  debouncerLeft.attach(ENC_L_BUT);
  debouncerLeft.interval(10);
  pinMode(ENC_R_BUT, INPUT_PULLUP);
  debouncerRight.attach(ENC_R_BUT);
  debouncerRight.interval(10);

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
   -Backlight Ctrl
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
  mm.add_menu(&mu3);
  ms.set_root_menu(&mm);
  displayMenu();
}
//=========================================================================================





//main loop ==============================================================================
void loop() {
  buttonHandler();
}
//=========================================================================================



