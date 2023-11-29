#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_SHT31.h>
#include <SD.h>
#include <LiquidMenu.h>
#include <RotaryEncoder.h>
#include "RTClib.h"
// #include <Arduino.h>

// ------ Configuración del LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

#if defined(ESP8266)
// Example for ESP8266 NodeMCU with input signals on pin D5 and D6
#define PIN_IN1 D6
#define PIN_IN2 D7
#endif


// ------ Variables
int temp_sen = 0;
int hum_sen = 0;
unsigned int temp_ctrl = 0;
unsigned int hum_ctrl = 0;
bool temp_estado_ctrl = false;
char temp_estado_ctrl_text[4]; // {'O', 'F', 'F', '\0'}
char luz_estado_ctrl_text[4]; // {'O', 'F', 'F', '\0'}
char string_on[] = "Act";
char string_off[] = "Des";
bool luz_estado_ctrl = false;

// ------ Configuración de Menus/submenus
//Menus
LiquidScreen pantallaMenuPrincipal;
LiquidScreen pantallaMenuMonitorizar;
LiquidScreen pantallaCtrlTemperatura;
LiquidScreen pantallaCtrlTemperatura_ctrlStatus;
LiquidScreen pantallaCtrlTemperatura_ctrlTemp;
LiquidScreen pantallaCtrlLuz;
LiquidScreen pantallaCtrlLuz_ctrlStatus;
LiquidScreen pantallaCtrlLuz_ctrlLuz;
LiquidScreen pantallaGrabar;

//menuPrincipal
LiquidLine Prin_L1(1, 0, "Monitorizar");
LiquidLine Prin_L2(1, 1, "Control Temp");
LiquidLine Prin_L3(1, 0, "Control Luz");
LiquidLine Prin_L4(1, 1, "Grabar Datos");

//menuMonitorizar
LiquidLine Mon_L1_1(0, 0, "T:");
LiquidLine Mon_L1_2(2, 0, temp_sen, "\337", temp_ctrl, "\337");
LiquidLine Mon_L1_3(8, 0, "H:");
LiquidLine Mon_L1_4(10, 0, hum_sen, "%", hum_ctrl, "%");
LiquidLine Mon_L2_1(0, 1, "CtrlTemp:  ", temp_estado_ctrl_text);

//menuCtrlTemperatura
LiquidLine CtrlTemp_L1(1, 0, "Menu Principal");
LiquidLine CtrlTemp_L2(1, 1, "Ctrl Temp: ", temp_estado_ctrl_text);
LiquidLine CtrlTemp_L3(1, 1, "Temperatura:", temp_ctrl, "\337");

//menuCtrlLuz
LiquidLine CtrlLuz_L1(1, 0, "Menu Principal");
LiquidLine CtrlLuz_L2(1, 1, "Ctrl Luz: ", "Des");

//menuGrabar
LiquidLine Grabar_L1(1, 0, "Menu Principal");
// LiquidLine Grabar_L2(1, 1, "Estado Rec:", luz_estado_ctrl_text);
LiquidLine Grabar_L3(1, 1, "Temp Rec: ", "Des", "\337");
LiquidLine Grabar_L4(1, 1, "Hum Rec:  ", "Des", "%");

LiquidMenu menuInvernadero (lcd, pantallaMenuPrincipal);

uint8_t fcline_menuAnterior = 1;

// ------ Configuración sensor temp/humedad
// SHT2x sht;
// ArtronShop_SHT3x sht3x(0x44, &Wire); // ADDR: 0 => 0x44, ADDR: 1 => 0x45
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// ------ Configuración Encoder
// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::FOUR3);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
// RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
#define encoderBotonPin D5
long oldPosition = -999;
#define ROTARYSTEPS 2
#define ROTARYMIN 0
#define ROTARYMAX 16


// ------ Configuración Reloj
RTC_DS1307 rtc;
// static DS1307 RTC;

// ------ Configuración SD
const int chipSelect = D8;


// the follow variables is a long because the time, measured in miliseconds
// will quickly become a bigger number than can be stored in an int.

long interval = 100;  // interval at which to blink (milliseconds)

void blankFunction()
{
    return;
}

void pantalla_anterior()
{
    menuInvernadero.previous_screen2();
    menuInvernadero.set_focusedLine(fcline_menuAnterior);
    delay(100);
}

bool Sht31_update(){
  float temp_sen_curr, hum_sen_curr;
  sht31.readBoth(&temp_sen_curr, &hum_sen_curr);
  
  if((temp_sen != (int)temp_sen_curr) || (hum_sen != (int)hum_sen_curr)){
    temp_sen = (int)temp_sen_curr;
    hum_sen = (int)hum_sen_curr;
    return true;
  }else
    return false;
}

void fn_monitorizar()
{
  fcline_menuAnterior = menuInvernadero.get_focusedLine();
  Sht31_update();
  if (temp_estado_ctrl)
    strncpy(temp_estado_ctrl_text, string_on, sizeof(string_on));
  else
    strncpy(temp_estado_ctrl_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(2);
  // delay(100);
}

void fn_ctrlTemp()
{
  fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (temp_estado_ctrl)
    strncpy(temp_estado_ctrl_text, string_on, sizeof(string_on));
  else
    strncpy(temp_estado_ctrl_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen(3);
  menuInvernadero.set_focusedLine(0);
  // delay(100);
}

void fn_ctrlLuz()
{
  fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (luz_estado_ctrl)
    strncpy(luz_estado_ctrl_text, string_on, sizeof(string_on));
  else
    strncpy(luz_estado_ctrl_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(4);
  menuInvernadero.set_focusedLine(0);
  // delay(100);
}

void fn_grabar()
{
  fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (luz_estado_ctrl)
    strncpy(luz_estado_ctrl_text, string_on, sizeof(string_on));
  else
    strncpy(luz_estado_ctrl_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(5);
  menuInvernadero.set_focusedLine(0);
  // delay(100);
}

void fn_principal()
{
  menuInvernadero.change_screen2(1);
  menuInvernadero.set_focusedLine(fcline_menuAnterior);
  // delay(100);
}


void selectOption(){
  if(digitalRead(encoderBotonPin)==LOW){
    while (digitalRead(encoderBotonPin)==LOW);
    delay(100);

    if (menuInvernadero.is_callable(1))
      menuInvernadero.call_function(1);
    else
    {
      pantalla_anterior();
      // menuInvernadero.change_screen2(1);
      // menuInvernadero.set_focusedLine(0);
    }
    menuInvernadero.update();
    
  }
}


void setup() {

  int tim_inf=500;

  // ------ Iniciar Entradas/Salidas
  pinMode(encoderBotonPin, INPUT);

  // ------ Iniciar LCD
  lcd.init();
  lcd.init();
  lcd.setBacklight(1);
  lcd.setCursor(0, 0);
  lcd.print(" Universidad del");
  lcd.setCursor(0, 1);
  lcd.print("    Pacifico");
  delay(tim_inf);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Invernadero");
  lcd.setCursor(0, 1);
  lcd.print("     v0.1");
  delay(tim_inf);
  lcd.clear();

    // ------ Iniciar Reloj
  lcd.setCursor(0, 0);
  lcd.print("Reloj: ");
  lcd.setCursor(0, 1);
  if (!rtc.begin()) 
    lcd.print("Error. No iniciado");
  else
    lcd.print("Conectado");
  delay(tim_inf);
  lcd.clear();

   // ------ Iniciar sht
  lcd.setCursor(0, 0);
  lcd.print("Sht31: ");
  lcd.setCursor(0, 1);
  if (!sht31.begin(0x44)) 
    lcd.print("No conectado");
  else
    lcd.print("Conectado");
  delay(tim_inf);
  lcd.clear();

  // ------ Iniciar SD
  lcd.setCursor(0, 0);
  lcd.print("SD: ");
  lcd.setCursor(0, 1);
  delay(1500);
  if (!SD.begin(chipSelect)) 
    lcd.print("No conectado");
  else
    lcd.print("Conectado");
  delay(tim_inf);
  lcd.clear();

     // ------ Iniciar menus/submenus
  //menuPrincipal
  pantallaMenuPrincipal.add_line(Prin_L1);
  pantallaMenuPrincipal.add_line(Prin_L2);
  pantallaMenuPrincipal.add_line(Prin_L3);
  pantallaMenuPrincipal.add_line(Prin_L4);

  Prin_L1.attach_function(1, fn_monitorizar);
  Prin_L2.attach_function(1, fn_ctrlTemp);
  Prin_L3.attach_function(1, fn_ctrlLuz);
  Prin_L4.attach_function(1, fn_grabar);

  pantallaMenuPrincipal.set_focusPosition(Position::LEFT);
  pantallaMenuPrincipal.set_displayLineCount(2);

  //menuMonitorizar
  pantallaMenuMonitorizar.add_line(Mon_L1_1);
  pantallaMenuMonitorizar.add_line(Mon_L1_2);
  pantallaMenuMonitorizar.add_line(Mon_L1_3);
  pantallaMenuMonitorizar.add_line(Mon_L1_4);
  pantallaMenuMonitorizar.add_line(Mon_L2_1);
  menuInvernadero.add_screen(pantallaMenuMonitorizar); // pantalla 2 

  //menuCtrlTemperatura
  pantallaCtrlTemperatura.add_line(CtrlTemp_L1);
  pantallaCtrlTemperatura.add_line(CtrlTemp_L2);
  pantallaCtrlTemperatura.add_line(CtrlTemp_L3);

  CtrlTemp_L1.attach_function(1, fn_principal);
  CtrlTemp_L2.attach_function(1, blankFunction);
  CtrlTemp_L3.attach_function(1, blankFunction);

  pantallaCtrlTemperatura.set_focusPosition(Position::LEFT);
  pantallaCtrlTemperatura.set_displayLineCount(2);
  menuInvernadero.add_screen(pantallaCtrlTemperatura); // pantalla 3

  //menuCtrlLuz
  pantallaCtrlLuz.add_line(CtrlLuz_L1);
  pantallaCtrlLuz.add_line(CtrlLuz_L2);
  // pantallaCtrlLuz.add_line(CtrlLuz_L3);

  CtrlLuz_L1.attach_function(1, fn_principal);
  CtrlLuz_L2.attach_function(1, blankFunction);
  // CtrlLuz_L3.attach_function(1, blankFunction);

  pantallaCtrlLuz.set_focusPosition(Position::LEFT);
  pantallaCtrlLuz.set_displayLineCount(2);
  menuInvernadero.add_screen(pantallaCtrlLuz); // pantalla 4

  //menuGrabar
  pantallaGrabar.add_line(Grabar_L1);
  // pantallaGrabar.add_line(Grabar_L2);
  pantallaGrabar.add_line(Grabar_L3);
  pantallaGrabar.add_line(Grabar_L4);

  Grabar_L1.attach_function(1, fn_principal);
  // Grabar_L2.attach_function(1, blankFunction);
  Grabar_L3.attach_function(1, blankFunction);
  Grabar_L4.attach_function(1, blankFunction);

  pantallaGrabar.set_focusPosition(Position::LEFT);
  pantallaGrabar.set_displayLineCount(2);
  menuInvernadero.add_screen(pantallaGrabar); // pantalla 5

  //sistema de menus
  menuInvernadero.init();
  menuInvernadero.set_focusedLine(0);
  menuInvernadero.update();

  // oldPosition = myEncoder.read();
  
}

void loop() {

selectOption();

int C_screen = menuInvernadero.get_currentNumScreen();

if(C_screen == 1){ // screen Monitorizar
  if(Sht31_update()) // si cambia valores de temp o hum, actualizar pantalla
    menuInvernadero.update();
}

if (menuInvernadero.is_callable(1)){
  // encoder
  encoder.tick();
 int En_direction = (int)encoder.getDirection();

  if(En_direction!=0){
    if (En_direction==1)
          menuInvernadero.switch_focus(true);
    else if (En_direction==-1)
      menuInvernadero.switch_focus(false);
    
    menuInvernadero.update();
    delay(100);
  }

  // }

  // if(digitalRead(bt1)==LOW){
  //   menuInvernadero.switch_focus(true);
  //   menuInvernadero.update();
  //   while (digitalRead(bt1)==LOW);
  //   delay(100);
  // }else if (digitalRead(bt2)==LOW)
  // {
  //   menuInvernadero.switch_focus(false);
  //   menuInvernadero.update();
  //   while (digitalRead(bt2)==LOW);
  //   delay(100);
  // }
  
}

// if(menuInvernadero.)
  //lcd.setCursor(1, 1);
  //lcd.print(SHT2x_LIB_VERSION);
  //lcd.clear();
  /* 

  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    //Serial.println(newPosition);
    lcd.setCursor(0, 0);
    lcd.print(newPosition);
  }
*/

  /*lcd.setCursor(0, 0);
  lcd.print(RTC.getDay());
  lcd.print("-");
  lcd.print(RTC.getMonth());
  lcd.print("-");
  lcd.print(RTC.getYear());

  lcd.setCursor(0, 1);
  lcd.print(RTC.getHours());
  lcd.print(":");
  lcd.print(RTC.getMinutes());
  lcd.print(":");
  lcd.print(RTC.getSeconds());
  delay(1000);*/

  /*
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  // make a string for assembling the data to log:
  String dataString = "";
  lcd.setCursor(0, 1);
  if (dataFile) {
    dataString = String(RTC.getHours()) + ":" + String(RTC.getMinutes()) + ":" + String(RTC.getSeconds());
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    lcd.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    lcd.println("error opening datalog.txt");
  }*/

}
