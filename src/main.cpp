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

int hum_sen = 0;
unsigned int hum_ctrl = 0;

int temp_sen = 0;
unsigned int temp_ctrl = 0;
bool temp_estado_ctrl = false;
char temp_estado_ctrl_text[4]; // {'O', 'F', 'F', '\0'}

char luz_modo_text[4]; // {'M', 'A', 'N', '\0'}
bool luz_modo = false;
char luz_manual_text[4]; // {'O', 'F', 'F', '\0'}
bool luz_manual = false;
int luz_hora_enc[2] = {0,0}; //{hora,min}
int luz_hora_apag[2] = {0,0}; //{hora,min}

char grabar_temp_modo_text[4]; // {'M', 'A', 'N', '\0'}
bool grabar_temp_modo = false;
char grabar_temp_manual_text[4]; // {'O', 'F', 'F', '\0'}
bool grabar_temp_manual = false;
int grabar_temp_hora_enc[2] = {0,0}; //{hora,min}
int grabar_temp_hora_apag[2] = {0,0}; //{hora,min}

char grabar_hum_modo_text[4]; // {'M', 'A', 'N', '\0'}
bool grabar_hum_modo = false;
char grabar_hum_manual_text[4]; // {'O', 'F', 'F', '\0'}
bool grabar_hum_manual = false;
int grabar_hum_hora_enc[2] = {0,0}; //{hora,min}
int grabar_hum_hora_apag[2] = {0,0}; //{hora,min}

char grabar_luz_modo_text[4]; // {'M', 'A', 'N', '\0'}
bool grabar_luz_modo = false;
char grabar_luz_manual_text[4]; // {'O', 'F', 'F', '\0'}
bool grabar_luz_manual = false;
int grabar_luz_hora_enc[2] = {0,0}; //{hora,min}
int grabar_luz_hora_apag[2] = {0,0}; //{hora,min}


char string_on[] = "ON";
char string_off[] = "OFF";
char string_man[] = "MAN";
char string_auto[] = "AUTO";
// ------ Configuración de Menus/submenus
//Menus
LiquidScreen pantallaMenuPrincipal;
LiquidScreen pantallaMenuMonitorizar;
LiquidScreen pantallaCtrlTemperatura;
LiquidScreen pantallaCtrlTemperatura_ctrlStatus;
LiquidScreen pantallaCtrlTemperatura_ctrlTemp;
LiquidScreen pantallaCtrlLuz;
LiquidScreen pantallaCtrlLuz_modo;
LiquidScreen pantallaCtrlLuz_manual;
LiquidScreen pantallaCtrlLuz_horaLuz_enc;
LiquidScreen pantallaCtrlLuz_horaLuz_apag;
LiquidScreen pantallaGrabar;
LiquidScreen pantallaGrabar_temp;
LiquidScreen pantallaGrabar_temp_modo;
LiquidScreen pantallaGrabar_temp_manual;
LiquidScreen pantallaGrabar_temp_horaenc;
LiquidScreen pantallaGrabar_temp_horaapg;
LiquidScreen pantallaGrabar_hum;
LiquidScreen pantallaGrabar_hum_modo;
LiquidScreen pantallaGrabar_hum_manual;
LiquidScreen pantallaGrabar_hum_horaenc;
LiquidScreen pantallaGrabar_hum_horaapg;
LiquidScreen pantallaGrabar_luz;
LiquidScreen pantallaGrabar_luz_modo;
LiquidScreen pantallaGrabar_luz_manual;
LiquidScreen pantallaGrabar_luz_horaenc;
LiquidScreen pantallaGrabar_luz_horaapg;

// enumerar el orden de las pantallas, indicandoles un numero
enum pantalla_num {
  P_void,
  P_MenuPrincipal,
  P_MenuMonitorizar,
  P_CtrlTemperatura,
  P_CtrlTemperatura_ctrlStatus,
  P_CtrlTemperatura_ctrlTemp,
  P_CtrlLuz,
  P_CtrlLuz_modo,
  P_CtrlLuz_manual,
  P_CtrlLuz_horaLuz_enc,
  P_CtrlLuz_horaLuz_apag,
  P_Grabar,
  P_Grabar_temp,
  P_Grabar_temp_modo,
  P_Grabar_temp_manual,
  P_Grabar_temp_horaenc,
  P_Grabar_temp_horaapg,
  P_Grabar_hum,
  P_Grabar_hum_modo,
  P_Grabar_hum_manual,
  P_Grabar_hum_horaenc,
  P_Grabar_hum_horaapg,
  P_Grabar_luz,
  P_Grabar_luz_modo,
  P_Grabar_luz_manual,
  P_Grabar_luz_horaenc,
  P_Grabar_luz_horaapg,
};

//menuPrincipal
LiquidLine Prin_L1(1, 0, "Monitorizar");
LiquidLine Prin_L2(1, 1, "Control Temp");
LiquidLine Prin_L3(1, 0, "Control Luz");
LiquidLine Prin_L4(1, 1, "Grabar Datos");

//menuMonitorizar
LiquidLine Mon_L1_1(0, 0, "T:");
LiquidLine Mon_L1_2(2, 0, temp_sen, "/", temp_ctrl, "\337");
LiquidLine Mon_L1_3(8, 0, "H:");
LiquidLine Mon_L1_4(10, 0, hum_sen, "/", hum_ctrl, "%");
LiquidLine Mon_L2_1(0, 1, "CtrlTemp:  ", temp_estado_ctrl_text);

//menuCtrlTemperatura
LiquidLine CtrlTemp_L1(1, 0, "Menu Principal");
LiquidLine CtrlTemp_L2(1, 1, "Ctrl Temp: ", temp_estado_ctrl_text);
LiquidLine CtrlTemp_L3(1, 1, "Temperatura:", temp_ctrl, "\337");

//pantallaCtrlTemperatura_ctrlStatus
LiquidLine CtrlTemp_status_L1(1, 0, "Ctrl Temperatura:");
LiquidLine CtrlTemp_status_L2(1, 1, temp_estado_ctrl_text);

//pantallaCtrlTemperatura_ctrlTemp
LiquidLine CtrlTemp_temp_L1(1, 0, "Temperatura:");
LiquidLine CtrlTemp_temp_L2(1, 1, temp_ctrl);

//menuCtrlLuz
LiquidLine CtrlLuz_L1(1, 0, "Menu Principal");
LiquidLine CtrlLuz_L2(1, 1, "Modo: ", luz_modo_text);
LiquidLine CtrlLuz_L3(1, 1, "Manual: ", luz_manual_text);
LiquidLine CtrlLuz_L4(1, 1, "Hora enc: ", "DEF");
LiquidLine CtrlLuz_L5(1, 1, "Hora apag: ", "DEF");

//pantallaCtrlLuz_Modo
LiquidLine CtrlLuz_modo_L1(1, 0, "Modo:");
LiquidLine CtrlLuz_modo_L2(1, 1, luz_modo_text);

//pantallaCtrlLuz_Manual
LiquidLine CtrlLuz_manual_L1(1, 0, "Encender Luz:");
LiquidLine CtrlLuz_manual_L2(1, 1, luz_manual_text);

//pantallaCtrlLuz_horaenc
LiquidLine CtrlLuz_horaenc_L1(1, 0, "hora encender:");
LiquidLine CtrlLuz_horaenc_L2(1, 1, "00:00");

//pantallaCtrlLuz_horaapg
LiquidLine CtrlLuz_horaapg_L1(1, 0, "hora apagar:");
LiquidLine CtrlLuz_horaapg_L2(1, 1, "00:00");

//menuGrabar
LiquidLine Grabar_L1(1, 0, "Menu Principal");
LiquidLine Grabar_L2(1, 1, "Temperatura");
LiquidLine Grabar_L3(1, 1, "Humedad");
LiquidLine Grabar_L4(1, 1, "Luz");

//menuGrabar_temp
LiquidLine Grabar_temp_L1(1, 0, "Menu Grabar");
LiquidLine Grabar_temp_L2(1, 1, "Modo: ", grabar_temp_modo_text);
LiquidLine Grabar_temp_L3(1, 1, "Manual: ", grabar_temp_manual_text);
LiquidLine Grabar_temp_L4(1, 1, "Hora enc: ", "DEF");
LiquidLine Grabar_temp_L5(1, 1, "Hora apag: ", "DEF");

//pantallaGrabar_temp_modo
LiquidLine Grabar_temp_modo_L1(1, 0, "Modo:");
LiquidLine Grabar_temp_modo_L2(1, 1, grabar_temp_modo_text);

//pantallaGrabar_temp_manual
LiquidLine Grabar_temp_manual_L1(1, 0, "Manual:");
LiquidLine Grabar_temp_manual_L2(1, 1, grabar_temp_manual_text);

//pantallaGrabar_temp_horaenc
LiquidLine Grabar_temp_horaenc_L1(1, 0, "hora encender:");
LiquidLine Grabar_temp_horaenc_L2(1, 1, "00:00");

//pantallaGrabar_temp_horaapg
LiquidLine Grabar_temp_horaapg_L1(1, 0, "hora apagar:");
LiquidLine Grabar_temp_horaapg_L2(1, 1, "00:00");

//menuGrabar_hum
LiquidLine Grabar_hum_L1(1, 0, "Menu Grabar");
LiquidLine Grabar_hum_L2(1, 1, "Modo: ", grabar_hum_modo_text);
LiquidLine Grabar_hum_L3(1, 1, "Manual: ", grabar_hum_manual_text);
LiquidLine Grabar_hum_L4(1, 1, "Hora enc: ", "DEF");
LiquidLine Grabar_hum_L5(1, 1, "Hora apag: ", "DEF");

//pantallaGrabar_hum_modo
LiquidLine Grabar_hum_modo_L1(1, 0, "Modo:");
LiquidLine Grabar_hum_modo_L2(1, 1, grabar_hum_modo_text);

//pantallaGrabar_hum_manual
LiquidLine Grabar_hum_manual_L1(1, 0, "Manual:");
LiquidLine Grabar_hum_manual_L2(1, 1, grabar_hum_manual_text);

//pantallaGrabar_hum_horaenc
LiquidLine Grabar_hum_horaenc_L1(1, 0, "hora encender:");
LiquidLine Grabar_hum_horaenc_L2(1, 1, "00:00");

//pantallaGrabar_hum_horaapg
LiquidLine Grabar_hum_horaapg_L1(1, 0, "hora apagar:");
LiquidLine Grabar_hum_horaapg_L2(1, 1, "00:00");

//menuGrabar_hum
LiquidLine Grabar_luz_L1(1, 0, "Menu Grabar");
LiquidLine Grabar_luz_L2(1, 1, "Modo: ", grabar_luz_modo_text);
LiquidLine Grabar_luz_L3(1, 1, "Manual: ", grabar_luz_manual_text);
LiquidLine Grabar_luz_L4(1, 1, "Hora enc: ", "DEF");
LiquidLine Grabar_luz_L5(1, 1, "Hora apag: ", "DEF");

//pantallaGrabar_luz_modo
LiquidLine Grabar_luz_modo_L1(1, 0, "Modo:");
LiquidLine Grabar_luz_modo_L2(1, 1, grabar_luz_modo_text);

//pantallaGrabar_luz_manual
LiquidLine Grabar_luz_manual_L1(1, 0, "Manual:");
LiquidLine Grabar_luz_manual_L2(1, 1, grabar_luz_manual_text);

//pantallaGrabar_luz_horaenc
LiquidLine Grabar_luz_horaenc_L1(1, 0, "hora encender:");
LiquidLine Grabar_luz_horaenc_L2(1, 1, "00:00");

//pantallaGrabar_luz_horaapg
LiquidLine Grabar_luz_horaapg_L1(1, 0, "hora apagar:");
LiquidLine Grabar_luz_horaapg_L2(1, 1, "00:00");

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

void fn_principal()
{
  menuInvernadero.change_screen2(P_MenuPrincipal);
  menuInvernadero.set_focusedLine(fcline_menuAnterior);
  // delay(100);
}

void fn_monitorizar()
{
  fcline_menuAnterior = menuInvernadero.get_focusedLine();
  Sht31_update();
  if (temp_estado_ctrl)
    strncpy(temp_estado_ctrl_text, string_on, sizeof(string_on));
  else
    strncpy(temp_estado_ctrl_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(P_MenuMonitorizar);
  // delay(100);
}

void fn_ctrlTemp()
{
  fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (temp_estado_ctrl)
    strncpy(temp_estado_ctrl_text, string_on, sizeof(string_on));
  else
    strncpy(temp_estado_ctrl_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(P_CtrlTemperatura);
  menuInvernadero.set_focusedLine(0);
  // delay(100);
}

void fn_ctrlTemp_Status()
{
  fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (temp_estado_ctrl)
    strncpy(temp_estado_ctrl_text, string_on, sizeof(string_on));
  else
    strncpy(temp_estado_ctrl_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(P_CtrlTemperatura_ctrlStatus);
  menuInvernadero.set_focusedLine(0);
  // delay(100);
}

void fn_ctrlTemp_Temp()
{
  fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_CtrlTemperatura_ctrlTemp);
  menuInvernadero.set_focusedLine(0);
  // delay(100);
}

void fn_ctrlLuz()
{
  fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (luz_modo)
    strncpy(luz_modo_text, string_auto, sizeof(string_auto));
  else
    strncpy(luz_modo_text, string_man, sizeof(string_man));
  if (luz_manual)
    strncpy(luz_manual_text, string_on, sizeof(string_on));
  else
    strncpy(luz_manual_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(6);
  menuInvernadero.set_focusedLine(0);
  // delay(100);
}

void fn_grabar()
{
  fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (grabar_temp_modo)
    strncpy(grabar_temp_modo_text, string_auto, sizeof(string_auto));
  else
    strncpy(grabar_temp_modo_text, string_man, sizeof(string_man));
  menuInvernadero.change_screen2(7);
  menuInvernadero.set_focusedLine(0);
  // delay(100);
}


void selectOption(){
  if(digitalRead(encoderBotonPin)==LOW){
    delay(20);
    while (digitalRead(encoderBotonPin)==LOW);
    delay(100);

    if (menuInvernadero.is_callable(1))
      menuInvernadero.call_function(1);
    else
    {
      pantalla_anterior();
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
  menuInvernadero.add_screen(pantallaMenuMonitorizar);

  //menuCtrlTemperatura
  pantallaCtrlTemperatura.add_line(CtrlTemp_L1);
  pantallaCtrlTemperatura.add_line(CtrlTemp_L2);
  pantallaCtrlTemperatura.add_line(CtrlTemp_L3);

  CtrlTemp_L1.attach_function(1, fn_principal);
  CtrlTemp_L2.attach_function(1, fn_ctrlTemp_Status);
  CtrlTemp_L3.attach_function(1, fn_ctrlTemp_Temp);

  pantallaCtrlTemperatura.set_focusPosition(Position::LEFT);
  pantallaCtrlTemperatura.set_displayLineCount(2);
  menuInvernadero.add_screen(pantallaCtrlTemperatura);

  // menuCtrlTemperatura_ctrlStatus
  pantallaCtrlTemperatura_ctrlStatus.add_line(CtrlTemp_status_L1);
  pantallaCtrlTemperatura_ctrlStatus.add_line(CtrlTemp_status_L2);

  menuInvernadero.add_screen(pantallaCtrlTemperatura_ctrlStatus);

  //menuCtrlTemperatura_ctrlTemp
  pantallaCtrlTemperatura_ctrlTemp.add_line(CtrlTemp_temp_L1);
  pantallaCtrlTemperatura_ctrlTemp.add_line(CtrlTemp_temp_L2);

  menuInvernadero.add_screen(pantallaCtrlTemperatura_ctrlTemp);

  //menuCtrlLuz
  pantallaCtrlLuz.add_line(CtrlLuz_L1);
  pantallaCtrlLuz.add_line(CtrlLuz_L2);
  pantallaCtrlLuz.add_line(CtrlLuz_L3);
  pantallaCtrlLuz.add_line(CtrlLuz_L4);
  pantallaCtrlLuz.add_line(CtrlLuz_L5);

  CtrlLuz_L1.attach_function(1, fn_principal);
  CtrlLuz_L2.attach_function(1, blankFunction);
  CtrlLuz_L3.attach_function(1, blankFunction);
  CtrlLuz_L4.attach_function(1, blankFunction);
  CtrlLuz_L5.attach_function(1, blankFunction);

  pantallaCtrlLuz.set_focusPosition(Position::LEFT);
  pantallaCtrlLuz.set_displayLineCount(2);
  menuInvernadero.add_screen(pantallaCtrlLuz); 

  // pantallaCtrlLuz_Modo
  pantallaCtrlLuz_modo.add_line(CtrlLuz_modo_L1);
  pantallaCtrlLuz_modo.add_line(CtrlLuz_modo_L2);

  menuInvernadero.add_screen(pantallaCtrlLuz_modo);

  //pantallaCtrlLuz_Manual
  pantallaCtrlLuz_manual.add_line(CtrlLuz_manual_L1);
  pantallaCtrlLuz_manual.add_line(CtrlLuz_manual_L2);

  menuInvernadero.add_screen(pantallaCtrlLuz_manual);

  //pantallaCtrlLuz_horaenc
  pantallaCtrlLuz_horaLuz_enc.add_line(CtrlLuz_horaenc_L1);
  pantallaCtrlLuz_horaLuz_enc.add_line(CtrlLuz_horaenc_L2);

  menuInvernadero.add_screen(pantallaCtrlLuz_horaLuz_enc); 

  //pantallaCtrlLuz_horaapg
  pantallaCtrlLuz_horaLuz_apag.add_line(CtrlLuz_horaapg_L1);
  pantallaCtrlLuz_horaLuz_apag.add_line(CtrlLuz_horaapg_L2);

  menuInvernadero.add_screen(pantallaCtrlLuz_horaLuz_apag); 

  //menuGrabar
  pantallaGrabar.add_line(Grabar_L1);
  pantallaGrabar.add_line(Grabar_L2);
  pantallaGrabar.add_line(Grabar_L3);
  pantallaGrabar.add_line(Grabar_L4);

  Grabar_L1.attach_function(1, fn_principal);
  Grabar_L2.attach_function(1, blankFunction);
  Grabar_L3.attach_function(1, blankFunction);
  Grabar_L4.attach_function(1, blankFunction);

  pantallaGrabar.set_focusPosition(Position::LEFT);
  pantallaGrabar.set_displayLineCount(2);
  menuInvernadero.add_screen(pantallaGrabar); 

  //menuGrabar_temp
  pantallaGrabar_temp.add_line(Grabar_temp_L1);
  pantallaGrabar_temp.add_line(Grabar_temp_L2);
  pantallaGrabar_temp.add_line(Grabar_temp_L3);
  pantallaGrabar_temp.add_line(Grabar_temp_L4);
  pantallaGrabar_temp.add_line(Grabar_temp_L5);

  Grabar_temp_L1.attach_function(1, fn_principal);
  Grabar_temp_L2.attach_function(1, blankFunction);
  Grabar_temp_L3.attach_function(1, blankFunction);
  Grabar_temp_L4.attach_function(1, blankFunction);
  Grabar_temp_L5.attach_function(1, blankFunction);

  pantallaGrabar_temp.set_focusPosition(Position::LEFT);
  pantallaGrabar_temp.set_displayLineCount(2);
  menuInvernadero.add_screen(pantallaGrabar_temp);

  //pantallaGrabar_temp_modo
  pantallaGrabar_temp_modo.add_line(Grabar_temp_modo_L1);
  pantallaGrabar_temp_modo.add_line(Grabar_temp_modo_L2);

  menuInvernadero.add_screen(pantallaGrabar_temp_modo); 

  //pantallaGrabar_temp_manual
  pantallaGrabar_temp_manual.add_line(Grabar_temp_manual_L1);
  pantallaGrabar_temp_manual.add_line(Grabar_temp_manual_L2);

  menuInvernadero.add_screen(pantallaGrabar_temp_manual); 

  //pantallaGrabar_temp_horaenc
  pantallaGrabar_temp_horaenc.add_line(Grabar_temp_horaenc_L1);
  pantallaGrabar_temp_horaenc.add_line(Grabar_temp_horaenc_L2);

  menuInvernadero.add_screen(pantallaGrabar_temp_horaenc); 

  //pantallaGrabar_temp_horaapg
  pantallaGrabar_temp_horaapg.add_line(Grabar_temp_horaapg_L1);
  pantallaGrabar_temp_horaapg.add_line(Grabar_temp_horaapg_L2);

  menuInvernadero.add_screen(pantallaGrabar_temp_horaapg); 

  //menuGrabar_hum
  pantallaGrabar_hum.add_line(Grabar_hum_L1);
  pantallaGrabar_hum.add_line(Grabar_hum_L2);
  pantallaGrabar_hum.add_line(Grabar_hum_L3);
  pantallaGrabar_hum.add_line(Grabar_hum_L4);
  pantallaGrabar_hum.add_line(Grabar_hum_L5);

  Grabar_hum_L1.attach_function(1, fn_principal);
  Grabar_hum_L2.attach_function(1, blankFunction);
  Grabar_hum_L3.attach_function(1, blankFunction);
  Grabar_hum_L4.attach_function(1, blankFunction);
  Grabar_hum_L5.attach_function(1, blankFunction);

  pantallaGrabar_hum.set_focusPosition(Position::LEFT);
  pantallaGrabar_hum.set_displayLineCount(2);
  menuInvernadero.add_screen(pantallaGrabar_hum); 

  //pantallaGrabar_hum_modo
  pantallaGrabar_hum_modo.add_line(Grabar_hum_modo_L1);
  pantallaGrabar_hum_modo.add_line(Grabar_hum_modo_L2);

  menuInvernadero.add_screen(pantallaGrabar_hum_modo);

  //pantallaGrabar_hum_manual
  pantallaGrabar_hum_manual.add_line(Grabar_hum_manual_L1);
  pantallaGrabar_hum_manual.add_line(Grabar_hum_manual_L2);

  menuInvernadero.add_screen(pantallaGrabar_hum_manual);

  //pantallaGrabar_hum_horaenc
  pantallaGrabar_hum_horaenc.add_line(Grabar_hum_horaenc_L1);
  pantallaGrabar_hum_horaenc.add_line(Grabar_hum_horaenc_L2);

  menuInvernadero.add_screen(pantallaGrabar_hum_horaenc);

  //pantallaGrabar_hum_horaapg
  pantallaGrabar_hum_horaapg.add_line(Grabar_hum_horaapg_L1);
  pantallaGrabar_hum_horaapg.add_line(Grabar_hum_horaapg_L2);

  menuInvernadero.add_screen(pantallaGrabar_hum_horaapg);

  //menuGrabar_luz
  pantallaGrabar_luz.add_line(Grabar_luz_L1);
  pantallaGrabar_luz.add_line(Grabar_luz_L2);
  pantallaGrabar_luz.add_line(Grabar_luz_L3);
  pantallaGrabar_luz.add_line(Grabar_luz_L4);
  pantallaGrabar_luz.add_line(Grabar_luz_L5);

  Grabar_luz_L1.attach_function(1, fn_principal);
  Grabar_luz_L2.attach_function(1, blankFunction);
  Grabar_luz_L3.attach_function(1, blankFunction);
  Grabar_luz_L4.attach_function(1, blankFunction);
  Grabar_luz_L5.attach_function(1, blankFunction);

  pantallaGrabar_luz.set_focusPosition(Position::LEFT);
  pantallaGrabar_luz.set_displayLineCount(2);
  menuInvernadero.add_screen(pantallaGrabar_luz); 

  //pantallaGrabar_luz_modo
  pantallaGrabar_luz_modo.add_line(Grabar_luz_modo_L1);
  pantallaGrabar_luz_modo.add_line(Grabar_luz_modo_L2);

  menuInvernadero.add_screen(pantallaGrabar_luz_modo);

  //pantallaGrabar_luz_manual
  pantallaGrabar_luz_manual.add_line(Grabar_luz_manual_L1);
  pantallaGrabar_luz_manual.add_line(Grabar_luz_manual_L2);

  menuInvernadero.add_screen(pantallaGrabar_luz_manual);

  //pantallaGrabar_luz_horaenc
  pantallaGrabar_luz_horaenc.add_line(Grabar_luz_horaenc_L1);
  pantallaGrabar_luz_horaenc.add_line(Grabar_luz_horaenc_L2);

  menuInvernadero.add_screen(pantallaGrabar_luz_horaenc);

  //pantallaGrabar_luz_horaapg
  pantallaGrabar_luz_horaapg.add_line(Grabar_luz_horaapg_L1);
  pantallaGrabar_luz_horaapg.add_line(Grabar_luz_horaapg_L2);

  menuInvernadero.add_screen(pantallaGrabar_luz_horaapg);

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
