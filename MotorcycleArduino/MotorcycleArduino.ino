#include <Bounce2.h>
#include <MenuSystem.h>
#include <Encoder.h>
#include <LiquidCrystal.h>


/*
Motorcycle Computer
By Philip Ahlers - www.philipahlers.com
Hardware used: Teensy 3.2 - www.pjrc.com/store/teensy32.html
Teensy code modifed to support low freqency PWM (<5Hz) - see https://forum.pjrc.com/threads/25164-2Hz-analogWriteFrequency-on-Teensy-3-1?p=80081&viewfull=1#post80081
  Also uses PJRC Encoder libraries.
For a rough description of the hardware this controls - see http://philipahlers.com/motorcycle-computer-the-screen-module/
It's essentially a couple of rotary encoders, buttons, and 16x2 LCD.


Left Knob - Grips Control (all the time, unless in a menu)
Right Knob - Navigation (unless in a menu that requires it)
Encoder button right - select
Encoder button left - back
Button Left - ?
Button Right - ?

Menu Structure:
 -Heat Control (for gloves & jacket)
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
 --Red
 --Blue
 --Green
 */


#define ENC_L_BUT 19 //encoder buttons
#define ENC_R_BUT 18

#define ENC_R_WIRE1 16 //right encoder wires
#define ENC_R_WIRE2 17

#define ENC_L_WIRE1 14 //left encoder wires
#define ENC_L_WIRE2 15

#define RS 0 //lcd pins
#define EN 1
#define DB4 2
#define DB5 3
#define DB6 4
#define DB7 5
#define BLRed 9
#define BLGrn 10
#define BLBlu 11 //end lcd pins

#define GripsScreenDelay 1000 //time the heated grips screen appears for


//use debouncer library
Bounce debouncerLeft = Bounce();
Bounce debouncerRight = Bounce();

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
boolean menuSelected = false;

enum menuTypes {
  heatControl,
  backlightControl
};

enum menuTypes currMenu;

void on_heatcontrol_selected(MenuItem* p_menu_item)
{
  menuSelected = true;
  currMenu = heatControl;
  heatcontroldisplay();
}

void on_backlightcontrol_selected(MenuItem* p_menu_item)
{
  menuSelected = true;
  currMenu = backlightControl;
}

void on_red_selected(MenuItem* p_menu_item)
{

}
void on_green_selected(MenuItem* p_menu_item)
{

}
void on_blue_selected(MenuItem* p_menu_item)
{

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
  lcd.setCursor(6, 1);
  lcd.print(gripsetting);
  lcd.write("%");
}

void backlightcontroldisplay() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Backlight Ctrl");
  lcd.setCursor(5, 1);
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
    if (menuSelected) {
      switch (currMenu) { //this switches between the different menu cases
        case heatControl :
          if (glovesetting < 100) {
            glovesetting = glovesetting + 2;
            heatcontroldisplay();
          }
          break;
        case backlightControl :
          break;
        default :
          break;
      } //end of the switch
    }
    else {
      gripscontroldisplay();
      lastgripscontrol = millis();
      if (gripsetting < 100)
        gripsetting = gripsetting + 2;
    }
    positionLeft = newLeft;
  }

  // called whenever left encoder moves counterclockwise
  if (newLeft < positionLeft) {
    if (menuSelected) {
      switch (currMenu) { //this switches between the different menu cases
        case heatControl :
          if (glovesetting > 0) {
            glovesetting = glovesetting - 2;
            heatcontroldisplay();
          }
          break;
        case backlightControl :
          break;
        default:
          break;
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
    if (menuSelected) {
      switch (currMenu) {
        case heatControl :
          if (jacketsetting < 100) {
            jacketsetting = jacketsetting + 2;
            heatcontroldisplay();
          }
          break;
        case backlightControl :
          break;
        default :
          break;
      } //end switch
    }
    else {
      ms.next();
      displayMenu();
    }
    positionRight = newRight;
  }


  //right encoder counter clockwise
  if (newRight < positionRight) {
    if (menuSelected) {
      switch (currMenu) {
        case heatControl :
          if (jacketsetting > 0) {
            jacketsetting = jacketsetting - 2;
            heatcontroldisplay();
          }
          break;
        case backlightControl :
          break;
        default :
          break;
      } //end switch
    }
    else {
      ms.prev();
      displayMenu();
    }
    positionRight = newRight;
  }


  if (leftbut == LOW) {
    if (menuSelected) {
      switch (currMenu) {
        case heatControl :
          menuSelected = false;
          break;
        case backlightControl :
          menuSelected = false;
          break;
        default :
          break;
      } //end switch
    }
    else {
      ms.back();
      displayMenu();
    }
  }


  if (rightbut == LOW)   {
    if (menuSelected) {
      switch (currMenu) {
        case heatControl :
          break;
        case backlightControl :
          break;
        default :
          break;
      } //end switch
    }
    else {
      ms.select();
      if (!menuSelected)
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
  lcd.begin(16, 2); //initialize the LCD

  //setting the pinmodes of the buttons & using the debounce library on them
  pinMode(ENC_L_BUT, INPUT_PULLUP);
  debouncerLeft.attach(ENC_L_BUT);
  debouncerLeft.interval(10);
  pinMode(ENC_R_BUT, INPUT_PULLUP);
  debouncerRight.attach(ENC_R_BUT);
  debouncerRight.interval(10);

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
  mu3.add_item(&mu3_mi1, &on_red_selected);
  mu3.add_item(&mu3_mi2, &on_blue_selected);
  mu3.add_item(&mu3_mi3, &on_green_selected);
  ms.set_root_menu(&mm);
  displayMenu();
}
//=========================================================================================



//main loop ==============================================================================
void loop() {
  buttonHandler();
}
//=========================================================================================



