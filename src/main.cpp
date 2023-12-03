#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_SHT31.h>
#include <SD.h>
#include <LiquidMenu.h>
#include <RotaryEncoder.h>
#include "RTClib.h"
// #include <Arduino.h>

// ------ Configuración del LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

#if defined(ESP8266)
// Example for ESP8266 NodeMCU with input signals on pin D5 and D6
#define PIN_IN1 D6
#define PIN_IN2 D7
#endif


// ------ Variables
int lcd_pos_hora=0;

int time_select = 0;

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

// enum state_encoder{
//   enc_scrolling,
//   enc_switch,
//   enc_poll
// } State_encoder;

//menuPrincipal
LiquidLine Prin_L1(1, 0, "Monitorizar");
LiquidLine Prin_L2(1, 1, "Control Temp");
LiquidLine Prin_L3(1, 2, "Control Luz");
LiquidLine Prin_L4(1, 3, "Grabar Datos");

//menuMonitorizar
LiquidLine Mon_L1_1(0, 0, "T:");
LiquidLine Mon_L1_2(2, 0, temp_sen, "/", temp_ctrl, "\337");
LiquidLine Mon_L1_3(8, 0, "H:");
LiquidLine Mon_L1_4(10, 0, hum_sen, "%");
LiquidLine Mon_L2_1(0, 1, "CtrlTemp:  ", temp_estado_ctrl_text);

//menuCtrlTemperatura
LiquidLine CtrlTemp_L1(1, 0, "Menu Principal");
LiquidLine CtrlTemp_L2(1, 1, "Ctrl Temp: ", temp_estado_ctrl_text);
LiquidLine CtrlTemp_L3(1, 2, "Temperatura:", temp_ctrl, "\337");

//pantallaCtrlTemperatura_ctrlStatus
LiquidLine CtrlTemp_status_L1(1, 0, "Ctrl Temperatura:");
LiquidLine CtrlTemp_status_L2(1, 1, temp_estado_ctrl_text);

//pantallaCtrlTemperatura_ctrlTemp
LiquidLine CtrlTemp_temp_L1(1, 0, "Temperatura:");
LiquidLine CtrlTemp_temp_L2(1, 1, temp_ctrl);

//menuCtrlLuz
LiquidLine CtrlLuz_L1(1, 0, "Menu Principal");
LiquidLine CtrlLuz_L2(1, 1, "Modo: ", luz_modo_text);
LiquidLine CtrlLuz_L3(1, 2, "Manual: ", luz_manual_text);
LiquidLine CtrlLuz_L4(1, 3, "Hora enc: ", luz_hora_enc[0],":",luz_hora_enc[1]);
LiquidLine CtrlLuz_L5(1, 4, "Hora apag: ", luz_hora_apag[0],":",luz_hora_apag[1]);

//pantallaCtrlLuz_Modo
LiquidLine CtrlLuz_modo_L1(1, 0, "Modo:");
LiquidLine CtrlLuz_modo_L2(1, 1, luz_modo_text);

//pantallaCtrlLuz_Manual
LiquidLine CtrlLuz_manual_L1(1, 0, "Encender Luz:");
LiquidLine CtrlLuz_manual_L2(1, 1, luz_manual_text);

//pantallaCtrlLuz_horaenc
LiquidLine CtrlLuz_horaenc_L1(1, 0, "hora encender:");
LiquidLine CtrlLuz_horaenc_L2(1, 1, luz_hora_enc[0] ,":", luz_hora_enc[1]);

//pantallaCtrlLuz_horaapg
LiquidLine CtrlLuz_horaapg_L1(1, 0, "hora apagar:");
LiquidLine CtrlLuz_horaapg_L2(1, 1, luz_hora_apag[0] ,":", luz_hora_apag[1]);

//menuGrabar
LiquidLine Grabar_L1(1, 0, "Menu Principal");
LiquidLine Grabar_L2(1, 1, "Temperatura");
LiquidLine Grabar_L3(1, 2, "Humedad");
LiquidLine Grabar_L4(1, 3, "Luz");

//menuGrabar_temp
LiquidLine Grabar_temp_L1(1, 0, "Menu Grabar");
LiquidLine Grabar_temp_L2(1, 1, "Modo:          ", grabar_temp_modo_text);
LiquidLine Grabar_temp_L3(1, 2, "Manual:        ", grabar_temp_manual_text);
LiquidLine Grabar_temp_L4(1, 3, "Hora enc:   ", grabar_temp_hora_enc[0], ":", grabar_temp_hora_enc[1]);
LiquidLine Grabar_temp_L5(1, 4, "Hora apag:  ", grabar_temp_hora_apag[0], ":", grabar_temp_hora_apag[1]);

//pantallaGrabar_temp_modo
LiquidLine Grabar_temp_modo_L1(1, 0, "Modo:");
LiquidLine Grabar_temp_modo_L2(1, 1, grabar_temp_modo_text);

//pantallaGrabar_temp_manual
LiquidLine Grabar_temp_manual_L1(1, 0, "Manual:");
LiquidLine Grabar_temp_manual_L2(1, 1, grabar_temp_manual_text);

//pantallaGrabar_temp_horaenc
LiquidLine Grabar_temp_horaenc_L1(1, 0, "hora encender:");
LiquidLine Grabar_temp_horaenc_L2(1, 1, grabar_temp_hora_enc[0], ":", grabar_temp_hora_enc[1]);

//pantallaGrabar_temp_horaapg
LiquidLine Grabar_temp_horaapg_L1(1, 0, "hora apagar:");
LiquidLine Grabar_temp_horaapg_L2(1, 1, grabar_temp_hora_apag[0], ":", grabar_temp_hora_apag[1]);

//menuGrabar_hum
LiquidLine Grabar_hum_L1(1, 0, "Menu Grabar");
LiquidLine Grabar_hum_L2(1, 1, "Modo: ", grabar_hum_modo_text);
LiquidLine Grabar_hum_L3(1, 2, "Manual: ", grabar_hum_manual_text);
LiquidLine Grabar_hum_L4(1, 3, "Hora enc: ", grabar_hum_hora_enc[0], ":", grabar_hum_hora_enc[1]);
LiquidLine Grabar_hum_L5(1, 4, "Hora apag: ", grabar_hum_hora_apag[0], ":", grabar_hum_hora_apag[1]);

//pantallaGrabar_hum_modo
LiquidLine Grabar_hum_modo_L1(1, 0, "Modo:");
LiquidLine Grabar_hum_modo_L2(1, 1, grabar_hum_modo_text);

//pantallaGrabar_hum_manual
LiquidLine Grabar_hum_manual_L1(1, 0, "Manual:");
LiquidLine Grabar_hum_manual_L2(1, 1, grabar_hum_manual_text);

//pantallaGrabar_hum_horaenc
LiquidLine Grabar_hum_horaenc_L1(1, 0, "hora encender:");
LiquidLine Grabar_hum_horaenc_L2(1, 1, grabar_hum_hora_enc[0], ":", grabar_hum_hora_enc[1]);

//pantallaGrabar_hum_horaapg
LiquidLine Grabar_hum_horaapg_L1(1, 0, "hora apagar:");
LiquidLine Grabar_hum_horaapg_L2(1, 1, grabar_hum_hora_apag[0], ":", grabar_hum_hora_apag[1]);

//menuGrabar_hum
LiquidLine Grabar_luz_L1(1, 0, "Menu Grabar");
LiquidLine Grabar_luz_L2(1, 1, "Modo: ", grabar_luz_modo_text);
LiquidLine Grabar_luz_L3(1, 2, "Manual: ", grabar_luz_manual_text);
LiquidLine Grabar_luz_L4(1, 3, "Hora enc: ", grabar_luz_hora_enc[0], ":", grabar_luz_hora_enc[1]);
LiquidLine Grabar_luz_L5(1, 4, "Hora apag: ", grabar_luz_hora_apag[0], ":", grabar_luz_hora_apag[1]);

//pantallaGrabar_luz_modo
LiquidLine Grabar_luz_modo_L1(1, 0, "Modo:");
LiquidLine Grabar_luz_modo_L2(1, 1, grabar_luz_modo_text);

//pantallaGrabar_luz_manual
LiquidLine Grabar_luz_manual_L1(1, 0, "Manual:");
LiquidLine Grabar_luz_manual_L2(1, 1, grabar_luz_manual_text);

//pantallaGrabar_luz_horaenc
LiquidLine Grabar_luz_horaenc_L1(1, 0, "hora encender:");
LiquidLine Grabar_luz_horaenc_L2(1, 1, grabar_luz_hora_enc[0], ":", grabar_luz_hora_enc[1]);

//pantallaGrabar_luz_horaapg
LiquidLine Grabar_luz_horaapg_L1(1, 0, "hora apagar:");
LiquidLine Grabar_luz_horaapg_L2(1, 1, grabar_luz_hora_apag[0], ":", grabar_luz_hora_apag[1]);

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
#define ROTARYSTEPS 4
#define ROTARYMIN 0
#define ROTARYMAX 50


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
    lcd.clear();
    menuInvernadero.previous_screen2();
    // menuInvernadero.set_focusedLine(fcline_menuAnterior);
    delay(50);
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
  lcd.clear();
  // State_encoder = enc_scrolling;
  menuInvernadero.change_screen2(P_MenuPrincipal);
  //menuInvernadero.set_focusedLine(fcline_menuAnterior);
}

void fn_monitorizar()
{
  lcd.clear();
  // State_encoder = enc_scrolling;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  Sht31_update();
  if (temp_estado_ctrl)
    strncpy(temp_estado_ctrl_text, string_on, sizeof(string_on));
  else
    strncpy(temp_estado_ctrl_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(P_MenuMonitorizar);
}

void fn_ctrlTemp()
{
  lcd.clear();
  // State_encoder = enc_scrolling;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (temp_estado_ctrl)
    strncpy(temp_estado_ctrl_text, string_on, sizeof(string_on));
  else
    strncpy(temp_estado_ctrl_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(P_CtrlTemperatura);
  //menuInvernadero.set_focusedLine(0);
}

void fn_ctrlTemp_Status()
{
  lcd.clear();
  // State_encoder = enc_switch;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_CtrlTemperatura_ctrlStatus);
  //menuInvernadero.set_focusedLine(0);
}

void fn_ctrlTemp_Temp()
{
  lcd.clear();
  // State_encoder = enc_poll;
  encoder.setPosition(temp_ctrl);
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_CtrlTemperatura_ctrlTemp);
  //menuInvernadero.set_focusedLine(0);
}

void fn_ctrlLuz()
{
  lcd.clear();
  // State_encoder = enc_scrolling;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (luz_modo)
    strncpy(luz_modo_text, string_auto, sizeof(string_auto));
  else
    strncpy(luz_modo_text, string_man, sizeof(string_man));
  if (luz_manual)
    strncpy(luz_manual_text, string_on, sizeof(string_on));
  else
    strncpy(luz_manual_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(P_CtrlLuz);
  //menuInvernadero.set_focusedLine(0);
}

void fn_ctroLuz_modo(){
  lcd.clear();
  // State_encoder = enc_switch;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_CtrlLuz_modo);
  //menuInvernadero.set_focusedLine(0);
}

void fn_ctroLuz_manual(){
  //State_encoder = enc_switch;
  lcd.clear(); 
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_CtrlLuz_manual);
  //menuInvernadero.set_focusedLine(0);
}

void fn_ctroLuz_horaenc(){
  // State_encoder = enc_poll;
  lcd.clear(); 
  encoder.setPosition(luz_hora_enc[0]);
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_CtrlLuz_horaLuz_enc);
  //menuInvernadero.set_focusedLine(0);

  //LiquidScreen *test = menuInvernadero.get_currentScreen()
  
  // for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
  //   // scroll one position left:
  //   lcd.scrollDisplayLeft();
  //   // wait a bit:
  //   delay(500);
  // }
  // lcd.rightToLeft();
}

void fn_ctroLuz_horaapg(){
  // State_encoder = enc_poll;
  lcd.clear(); 
  encoder.setPosition(luz_hora_apag[0]);
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_CtrlLuz_horaLuz_apag);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar()
{
  lcd.clear();
  // State_encoder = enc_scrolling;
  // // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_temp()
{
  lcd.clear();
  // State_encoder = enc_scrolling;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (grabar_temp_modo)
    strncpy(grabar_temp_modo_text, string_auto, sizeof(string_auto));
  else
    strncpy(grabar_temp_modo_text, string_man, sizeof(string_man));
  if (grabar_temp_manual)
    strncpy(grabar_temp_manual_text, string_on, sizeof(string_on));
  else
    strncpy(grabar_temp_manual_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(P_Grabar_temp);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_temp_modo()
{
  lcd.clear();
  // State_encoder = enc_switch;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_temp_modo);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_temp_manual()
{
  lcd.clear();
  // State_encoder = enc_switch;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_temp_manual);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_temp_horaenc()
{
  lcd.clear();
  encoder.setPosition(grabar_temp_hora_enc[0]);
  // State_encoder = enc_poll;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_temp_horaenc);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_temp_horaapg()
{
  lcd.clear();
  encoder.setPosition(grabar_temp_hora_apag[0]);
  // State_encoder = enc_poll;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_temp_horaapg);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_hum()
{
  lcd.clear();
  // State_encoder = enc_scrolling;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (grabar_hum_modo)
    strncpy(grabar_hum_modo_text, string_auto, sizeof(string_auto));
  else
    strncpy(grabar_hum_modo_text, string_man, sizeof(string_man));
  if (grabar_hum_manual)
    strncpy(grabar_hum_manual_text, string_on, sizeof(string_on));
  else
    strncpy(grabar_hum_manual_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(P_Grabar_hum);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_hum_modo()
{
  lcd.clear();
  // State_encoder = enc_switch;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_hum_modo);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_hum_manual()
{
  lcd.clear();
  // State_encoder = enc_switch;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_hum_manual);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_hum_horaenc()
{
  lcd.clear();
  encoder.setPosition(grabar_hum_hora_enc[0]);
  // State_encoder = enc_poll;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_hum_horaenc);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_hum_horaapg()
{
  lcd.clear();
  encoder.setPosition(grabar_hum_hora_apag[0]);
  // State_encoder = enc_poll;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_hum_horaapg);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_luz()
{
  lcd.clear();
  // State_encoder = enc_scrolling;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  if (grabar_luz_modo)
    strncpy(grabar_luz_modo_text, string_auto, sizeof(string_auto));
  else
    strncpy(grabar_luz_modo_text, string_man, sizeof(string_man));
  if (grabar_luz_manual)
    strncpy(grabar_luz_manual_text, string_on, sizeof(string_on));
  else
    strncpy(grabar_luz_manual_text, string_off, sizeof(string_off));
  menuInvernadero.change_screen2(P_Grabar_luz);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_luz_modo()
{
  lcd.clear();
  // State_encoder = enc_switch;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_luz_modo);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_luz_manual()
{
  lcd.clear();
  // State_encoder = enc_switch;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_luz_manual);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_luz_horaenc()
{
  lcd.clear();
  encoder.setPosition(grabar_luz_hora_enc[0]);
  // State_encoder = enc_poll;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_luz_horaenc);
  //menuInvernadero.set_focusedLine(0);
}

void fn_grabar_luz_horaapg()
{
  lcd.clear();
  encoder.setPosition(grabar_luz_hora_apag[0]);
  // State_encoder = enc_poll;
  // fcline_menuAnterior = menuInvernadero.get_focusedLine();
  menuInvernadero.change_screen2(P_Grabar_luz_horaapg);
  //menuInvernadero.set_focusedLine(0);
}

void selectOption(){
  if(digitalRead(encoderBotonPin)==LOW){
    delay(20);
    while (digitalRead(encoderBotonPin)==LOW);
    delay(10);

    if (menuInvernadero.is_callable(1)){
      menuInvernadero.call_function(1);
      if(menuInvernadero.get_currentScreen()->get_encoder_interaction()==LiquidScreen::enc_time){
        int curr_NumScreen = menuInvernadero.get_currentNumScreen() + 1;
        int lcd_pos = 1;
        lcd_pos_hora = 1;
        switch (curr_NumScreen)
        {
          case P_CtrlLuz_horaLuz_enc:
            if(luz_hora_enc[time_select]>9)      
              lcd_pos = 2;
            break;
          case P_CtrlLuz_horaLuz_apag:
            if(luz_hora_apag[time_select]>9)
              lcd_pos = 2;
            break;
          case P_Grabar_temp_horaenc:
            if(grabar_temp_hora_enc[time_select]>9)
              lcd_pos=2;
            break;
          case P_Grabar_temp_horaapg:
            if(grabar_temp_hora_apag[time_select] >9)
            lcd_pos=2;
            break;
          case P_Grabar_hum_horaenc:
            if(grabar_hum_hora_enc[time_select] >9)
            lcd_pos=2;
            break;
          case P_Grabar_hum_horaapg:
            if(grabar_hum_hora_apag[time_select] >9)
            lcd_pos=2;
            break;
          case P_Grabar_luz_horaenc:
            if(grabar_luz_hora_enc[time_select] >9)
            lcd_pos=2;
            break;
          case P_Grabar_luz_horaapg:
            if(grabar_luz_hora_apag[time_select] >9)
            lcd_pos=2;
            break;
        default:
          break;
        }
        lcd_pos_hora = lcd_pos;
        lcd.setCursor(lcd_pos,1);
        lcd.blink();
      }
    }else if(menuInvernadero.get_currentScreen()->get_encoder_interaction()==LiquidScreen::enc_time){
      if(time_select > 0){
        time_select = 0;
        lcd.noBlink();
        pantalla_anterior();
      }else{
        time_select++;

        int curr_NumScreen = menuInvernadero.get_currentNumScreen() + 1;
        int lcd_pos = 1;
        switch (curr_NumScreen)
        {
          case P_CtrlLuz_horaLuz_enc:
            if(luz_hora_enc[time_select]>9)      
              lcd_pos = 2;
            encoder.setPosition(luz_hora_enc[time_select]);
            break;
          case P_CtrlLuz_horaLuz_apag:
            if(luz_hora_apag[time_select]>9)
              lcd_pos = 2;
            encoder.setPosition(luz_hora_apag[time_select]);
            break;
          case P_Grabar_temp_horaenc:
            if(grabar_temp_hora_enc[time_select]>9)
              lcd_pos=2;
            encoder.setPosition(grabar_temp_hora_enc[time_select]);
            break;
          case P_Grabar_temp_horaapg:
            if(grabar_temp_hora_apag[time_select] >9)
            lcd_pos=2;
            encoder.setPosition(grabar_temp_hora_apag[time_select]);
            break;
          case P_Grabar_hum_horaenc:
            if(grabar_hum_hora_enc[time_select] >9)
            lcd_pos=2;
            encoder.setPosition(grabar_hum_hora_enc[time_select]);
            break;
          case P_Grabar_hum_horaapg:
            if(grabar_hum_hora_apag[time_select] >9)
            lcd_pos=2;
            encoder.setPosition(grabar_hum_hora_apag[time_select]);
            break;
          case P_Grabar_luz_horaenc:
            if(grabar_luz_hora_enc[time_select] >9)
            lcd_pos=2;
            encoder.setPosition(grabar_luz_hora_enc[time_select]);
            break;
          case P_Grabar_luz_horaapg:
            if(grabar_luz_hora_apag[time_select] >9)
            lcd_pos=2;
            encoder.setPosition(grabar_luz_hora_apag[time_select]);
            break;
        default:
          break;
        }
        lcd.setCursor(lcd_pos_hora + 1 + lcd_pos,1);
        lcd.blink();
      }
    }else
      pantalla_anterior();
  }
}

void switch_encoder(bool state_sw){
  switch (menuInvernadero.get_currentNumScreen() + 1)
  {
  case P_CtrlTemperatura_ctrlStatus:
    temp_estado_ctrl = state_sw;
    if(state_sw)
      strncpy(temp_estado_ctrl_text, string_on, sizeof(string_on));
    else
      strncpy(temp_estado_ctrl_text, string_off, sizeof(string_off));
    break;
  case P_CtrlLuz_modo:
    luz_modo = state_sw;
    if(state_sw)
      strncpy(luz_modo_text, string_auto, sizeof(string_auto));
    else
      strncpy(luz_modo_text, string_man, sizeof(string_man));
    break;  
  case P_CtrlLuz_manual:
    luz_manual = state_sw;
    if(state_sw)
      strncpy(luz_manual_text, string_on, sizeof(string_on));
    else
      strncpy(luz_manual_text, string_off, sizeof(string_off));
    break; 
  case P_Grabar_temp_modo:
    grabar_temp_modo = state_sw;
    if(state_sw)
      strncpy(grabar_temp_modo_text, string_auto, sizeof(string_auto));
    else
      strncpy(grabar_temp_modo_text, string_man, sizeof(string_man));
    break;
  case P_Grabar_temp_manual:
    grabar_temp_manual = state_sw;
    if(state_sw)
      strncpy(grabar_temp_manual_text, string_on, sizeof(string_on));
    else
      strncpy(grabar_temp_manual_text, string_off, sizeof(string_off));
    break; 
  case P_Grabar_hum_modo:
    grabar_hum_modo = state_sw;
    if(state_sw)
      strncpy(grabar_hum_modo_text, string_auto, sizeof(string_auto));
    else
      strncpy(grabar_hum_modo_text, string_man, sizeof(string_man));
    break;
  case P_Grabar_hum_manual:
    grabar_hum_manual = state_sw;
    if(state_sw)
      strncpy(grabar_hum_manual_text, string_on, sizeof(string_on));
    else
      strncpy(grabar_hum_manual_text, string_off, sizeof(string_off));
    break; 
  case P_Grabar_luz_modo:
    grabar_luz_modo = state_sw;
    if(state_sw)
      strncpy(grabar_luz_modo_text, string_auto, sizeof(string_auto));
    else
      strncpy(grabar_luz_modo_text, string_man, sizeof(string_man));
    break;
  case P_Grabar_luz_manual:
    grabar_luz_manual = state_sw;
    if(state_sw)
      strncpy(grabar_luz_manual_text, string_on, sizeof(string_on));
    else
      strncpy(grabar_luz_manual_text, string_off, sizeof(string_off));
    break; 
  default:
    break;
  }
}

void poll_encoder(int posEnc){
  int curr_NumScreen = menuInvernadero.get_currentNumScreen() + 1;
  if(curr_NumScreen == P_CtrlTemperatura_ctrlTemp){
   if (posEnc > 50) {
      encoder.setPosition(50);
      posEnc = 50;
    } // if
    temp_ctrl = posEnc; 
  }else{
    if (time_select == 0){
      if (posEnc < 0 ){
        encoder.setPosition(23);
          posEnc = 23;
      }
      if (posEnc > 23) {
          encoder.setPosition(0);
          posEnc = 0;
      } // if
    }else if (time_select == 1){
      if (posEnc < 0 ){
        encoder.setPosition(59);
          posEnc = 59;
      }
      if (posEnc > 59) {
          encoder.setPosition(0);
          posEnc = 0;
      } // if
    }
    // lcd.setCursor(2 + time_select * 3, 1);
    switch (curr_NumScreen)
    {
      case P_CtrlLuz_horaLuz_enc:      
        luz_hora_enc[time_select] = posEnc;
        break;
      case P_CtrlLuz_horaLuz_apag:
        luz_hora_apag[time_select] = posEnc;
        break;
      case P_Grabar_temp_horaenc:
        grabar_temp_hora_enc[time_select] = posEnc;
        break;
      case P_Grabar_temp_horaapg:
        grabar_temp_hora_apag[time_select] = posEnc;
        break;
      case P_Grabar_hum_horaenc:
        grabar_hum_hora_enc[time_select] = posEnc;
        break;
      case P_Grabar_hum_horaapg:
        grabar_hum_hora_apag[time_select] = posEnc;
        break;
      case P_Grabar_luz_horaenc:
        grabar_luz_hora_enc[time_select] = posEnc;
        break;
      case P_Grabar_luz_horaapg:
        grabar_luz_hora_apag[time_select] = posEnc;
        break;
    default:
      break;
    }

    menuInvernadero.update();
    
    if(posEnc>9){
      if (time_select==0)
        lcd_pos_hora=2;
      lcd.setCursor(lcd_pos_hora+time_select*3,1);
      lcd.blink();
    }else{
      if (time_select==0)
        lcd_pos_hora=1;
      lcd.setCursor(lcd_pos_hora+time_select*2,1);
      lcd.blink();
    }
  }
}

void encoderRotating(){
  // funcion para scrolling
  static int pos = 0;
  encoder.tick();
  RotaryEncoder::Direction En_direction = encoder.getDirection();

  if(En_direction != RotaryEncoder::Direction::NOROTATION){
    switch (menuInvernadero.get_currentScreen()->get_encoder_interaction())
    {
    case LiquidScreen::enc_scrolling:
      {
        if (menuInvernadero.is_callable(1)){
          if (En_direction == RotaryEncoder::Direction::CLOCKWISE)
            menuInvernadero.switch_focus(true);
          else if (En_direction == RotaryEncoder::Direction::COUNTERCLOCKWISE)
            menuInvernadero.switch_focus(false);
        }
      }
      break;
    case LiquidScreen::enc_switch:
      {
        if (En_direction == RotaryEncoder::Direction::CLOCKWISE){
          switch_encoder(true);
        }else if (En_direction == RotaryEncoder::Direction::COUNTERCLOCKWISE){  
          switch_encoder(false);
        }
        menuInvernadero.update();
        delay(50);
      }
      break;
    case LiquidScreen::enc_polling:
      { 
        int newPos = encoder.getPosition();
        if (newPos < ROTARYMIN) {
          encoder.setPosition(ROTARYMIN);
          newPos = ROTARYMIN;
        }
        poll_encoder(newPos);
        menuInvernadero.update();
        delay(50);
      }
      break;
    case LiquidScreen::enc_time:
      { 
        int newPos = encoder.getPosition();
        poll_encoder(newPos);
        
        // if (time_select == 0){
        //   lcd.setCursor(2,1);
        //   lcd.blink();
        // }
        // else if (time_select == 1){
        //   lcd.setCursor(5,1);
        //   lcd.blink();
        // }
        delay(50);
      }
    break;
    default:
      break;
    }
  }

  // if(En_direction != 0){
  //   //scrolling el Menu
  //   if (menuInvernadero.is_callable(1)){
  //     if (En_direction == 1)
  //       menuInvernadero.switch_focus(true);
  //     else if (En_direction == -1)
  //       menuInvernadero.switch_focus(false);
      
  //     // menuInvernadero.update();
  //     //  delay(50);
  //   }else{
  //     //scrolling variables/switch
  //     switch (menuInvernadero.get_currentNumScreen() + 1)
  //     {
  //     case P_CtrlTemperatura_ctrlStatus:
  //       if (En_direction == 1){
  //         temp_estado_ctrl = true;
  //         strncpy(temp_estado_ctrl_text, string_on, sizeof(string_on));
  //       }else if (En_direction == -1){  
  //         temp_estado_ctrl = false;
  //         strncpy(temp_estado_ctrl_text, string_off, sizeof(string_off));
  //       }
        
  //       break;

  //     case P_CtrlTemperatura_ctrlTemp:
  //       {int newPos = encoder.getPosition();
  //       if (newPos < ROTARYMIN) {
  //         encoder.setPosition(ROTARYMIN / ROTARYSTEPS);
  //         newPos = ROTARYMIN;

  //       } else if (newPos > ROTARYMAX) {
  //         encoder.setPosition(ROTARYMAX / ROTARYSTEPS);
  //         newPos = ROTARYMAX;
  //       } // if
  //       temp_ctrl = newPos;
  //       }
  //       break;

  //     default:
  //       break;
  //     }
  //   }
  //   menuInvernadero.update();
  //   delay(50);
  // }

}



void setup() {

  int tim_inf=500;

  // ------ Iniciar Entradas/Salidas
  pinMode(encoderBotonPin, INPUT);

  // ------ Iniciar LCD
  lcd.init();
  lcd.init();
  lcd.setBacklight(1);
  lcd.setCursor(0, 1);
  lcd.print("   Universidad del");
  lcd.setCursor(0, 2);
  lcd.print("      Pacifico");
  delay(tim_inf);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("    Invernadero");
  lcd.setCursor(0, 2);
  lcd.print("       v0.1");
  delay(tim_inf);
  lcd.clear();

    // ------ Iniciar Reloj
  lcd.setCursor(0, 1);
  lcd.print("       Reloj: ");
  lcd.setCursor(0, 2);
  if (!rtc.begin()) 
    lcd.print(" Error. No iniciado");
  else
    lcd.print("     Conectado");
  delay(tim_inf);
  lcd.clear();

   // ------ Iniciar sht
  lcd.setCursor(0, 1);
  lcd.print("       Sht31: ");
  lcd.setCursor(0, 2);
  if (!sht31.begin(0x44)) 
    lcd.print("   No conectado");
  else
    lcd.print("     Conectado");
  delay(tim_inf);
  lcd.clear();

  // ------ Iniciar SD
  lcd.setCursor(0, 1);
  lcd.print("        SD: ");
  lcd.setCursor(0, 2);
  delay(2000);
  if (!SD.begin(chipSelect)) 
    lcd.print("    No conectado");
  else
    lcd.print("     Conectado");
  delay(tim_inf);
  lcd.clear();

     // ------ Iniciar menus/submenus

  #define displayLineCount 4
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
  pantallaMenuPrincipal.set_displayLineCount(displayLineCount);
  pantallaMenuPrincipal.set_encoder_interaction(LiquidScreen::enc_scrolling);
  pantallaMenuPrincipal.set_focusedLine(0);

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
  pantallaCtrlTemperatura.set_displayLineCount(displayLineCount);
  pantallaCtrlTemperatura.set_encoder_interaction(LiquidScreen::enc_scrolling);
  pantallaCtrlTemperatura.set_focusedLine(0);
  menuInvernadero.add_screen(pantallaCtrlTemperatura);

  // menuCtrlTemperatura_ctrlStatus
  pantallaCtrlTemperatura_ctrlStatus.add_line(CtrlTemp_status_L1);
  pantallaCtrlTemperatura_ctrlStatus.add_line(CtrlTemp_status_L2);
  pantallaCtrlTemperatura_ctrlStatus.set_encoder_interaction(LiquidScreen::enc_switch);
  menuInvernadero.add_screen(pantallaCtrlTemperatura_ctrlStatus);

  //menuCtrlTemperatura_ctrlTemp
  pantallaCtrlTemperatura_ctrlTemp.add_line(CtrlTemp_temp_L1);
  pantallaCtrlTemperatura_ctrlTemp.add_line(CtrlTemp_temp_L2);
  pantallaCtrlTemperatura_ctrlTemp.set_encoder_interaction(LiquidScreen::enc_polling);
  menuInvernadero.add_screen(pantallaCtrlTemperatura_ctrlTemp);

  //menuCtrlLuz
  pantallaCtrlLuz.add_line(CtrlLuz_L1);
  pantallaCtrlLuz.add_line(CtrlLuz_L2);
  pantallaCtrlLuz.add_line(CtrlLuz_L3);
  pantallaCtrlLuz.add_line(CtrlLuz_L4);
  pantallaCtrlLuz.add_line(CtrlLuz_L5);

  CtrlLuz_L1.attach_function(1, fn_principal);
  CtrlLuz_L2.attach_function(1, fn_ctroLuz_modo);
  CtrlLuz_L3.attach_function(1, fn_ctroLuz_manual);
  CtrlLuz_L4.attach_function(1, fn_ctroLuz_horaenc);
  CtrlLuz_L5.attach_function(1, fn_ctroLuz_horaapg);

  pantallaCtrlLuz.set_focusPosition(Position::LEFT);
  pantallaCtrlLuz.set_displayLineCount(displayLineCount);
  pantallaCtrlLuz.set_encoder_interaction(LiquidScreen::enc_scrolling);
  pantallaCtrlLuz.set_focusedLine(0);
  menuInvernadero.add_screen(pantallaCtrlLuz); 

  // pantallaCtrlLuz_Modo
  pantallaCtrlLuz_modo.add_line(CtrlLuz_modo_L1);
  pantallaCtrlLuz_modo.add_line(CtrlLuz_modo_L2);
  pantallaCtrlLuz_modo.set_encoder_interaction(LiquidScreen::enc_switch);
  menuInvernadero.add_screen(pantallaCtrlLuz_modo);

  //pantallaCtrlLuz_Manual
  pantallaCtrlLuz_manual.add_line(CtrlLuz_manual_L1);
  pantallaCtrlLuz_manual.add_line(CtrlLuz_manual_L2);
  pantallaCtrlLuz_manual.set_encoder_interaction(LiquidScreen::enc_switch);
  menuInvernadero.add_screen(pantallaCtrlLuz_manual);

  //pantallaCtrlLuz_horaenc
  pantallaCtrlLuz_horaLuz_enc.add_line(CtrlLuz_horaenc_L1);
  pantallaCtrlLuz_horaLuz_enc.add_line(CtrlLuz_horaenc_L2);
  pantallaCtrlLuz_horaLuz_enc.set_encoder_interaction(LiquidScreen::enc_time);
  menuInvernadero.add_screen(pantallaCtrlLuz_horaLuz_enc); 

  //pantallaCtrlLuz_horaapg
  pantallaCtrlLuz_horaLuz_apag.add_line(CtrlLuz_horaapg_L1);
  pantallaCtrlLuz_horaLuz_apag.add_line(CtrlLuz_horaapg_L2);
  pantallaCtrlLuz_horaLuz_apag.set_encoder_interaction(LiquidScreen::enc_time);
  menuInvernadero.add_screen(pantallaCtrlLuz_horaLuz_apag); 

  //menuGrabar
  pantallaGrabar.add_line(Grabar_L1);
  pantallaGrabar.add_line(Grabar_L2);
  pantallaGrabar.add_line(Grabar_L3);
  pantallaGrabar.add_line(Grabar_L4);

  Grabar_L1.attach_function(1, fn_principal);
  Grabar_L2.attach_function(1, fn_grabar_temp);
  Grabar_L3.attach_function(1, fn_grabar_hum);
  Grabar_L4.attach_function(1, fn_grabar_luz);

  pantallaGrabar.set_focusPosition(Position::LEFT);
  pantallaGrabar.set_displayLineCount(displayLineCount);
  pantallaGrabar.set_encoder_interaction(LiquidScreen::enc_scrolling);
  pantallaGrabar.set_focusedLine(0);
  menuInvernadero.add_screen(pantallaGrabar); 

  //menuGrabar_temp
  pantallaGrabar_temp.add_line(Grabar_temp_L1);
  pantallaGrabar_temp.add_line(Grabar_temp_L2);
  pantallaGrabar_temp.add_line(Grabar_temp_L3);
  pantallaGrabar_temp.add_line(Grabar_temp_L4);
  pantallaGrabar_temp.add_line(Grabar_temp_L5);

  Grabar_temp_L1.attach_function(1, fn_grabar);
  Grabar_temp_L2.attach_function(1, fn_grabar_temp_modo);
  Grabar_temp_L3.attach_function(1, fn_grabar_temp_manual);
  Grabar_temp_L4.attach_function(1, fn_grabar_temp_horaenc);
  Grabar_temp_L5.attach_function(1, fn_grabar_temp_horaapg);

  pantallaGrabar_temp.set_focusPosition(Position::LEFT);
  pantallaGrabar_temp.set_displayLineCount(displayLineCount);
  pantallaGrabar_temp.set_encoder_interaction(LiquidScreen::enc_scrolling);
  pantallaGrabar_temp.set_focusedLine(0);
  menuInvernadero.add_screen(pantallaGrabar_temp);

  //pantallaGrabar_temp_modo
  pantallaGrabar_temp_modo.add_line(Grabar_temp_modo_L1);
  pantallaGrabar_temp_modo.add_line(Grabar_temp_modo_L2);
  pantallaGrabar_temp_modo.set_encoder_interaction(LiquidScreen::enc_switch);
  menuInvernadero.add_screen(pantallaGrabar_temp_modo); 

  //pantallaGrabar_temp_manual
  pantallaGrabar_temp_manual.add_line(Grabar_temp_manual_L1);
  pantallaGrabar_temp_manual.add_line(Grabar_temp_manual_L2);
  pantallaGrabar_temp_manual.set_encoder_interaction(LiquidScreen::enc_switch);
  menuInvernadero.add_screen(pantallaGrabar_temp_manual); 

  //pantallaGrabar_temp_horaenc
  pantallaGrabar_temp_horaenc.add_line(Grabar_temp_horaenc_L1);
  pantallaGrabar_temp_horaenc.add_line(Grabar_temp_horaenc_L2);
  pantallaGrabar_temp_horaenc.set_encoder_interaction(LiquidScreen::enc_time);
  menuInvernadero.add_screen(pantallaGrabar_temp_horaenc); 

  //pantallaGrabar_temp_horaapg
  pantallaGrabar_temp_horaapg.add_line(Grabar_temp_horaapg_L1);
  pantallaGrabar_temp_horaapg.add_line(Grabar_temp_horaapg_L2);
  pantallaGrabar_temp_horaapg.set_encoder_interaction(LiquidScreen::enc_time);
  menuInvernadero.add_screen(pantallaGrabar_temp_horaapg); 

  //menuGrabar_hum
  pantallaGrabar_hum.add_line(Grabar_hum_L1);
  pantallaGrabar_hum.add_line(Grabar_hum_L2);
  pantallaGrabar_hum.add_line(Grabar_hum_L3);
  pantallaGrabar_hum.add_line(Grabar_hum_L4);
  pantallaGrabar_hum.add_line(Grabar_hum_L5);

  Grabar_hum_L1.attach_function(1, fn_grabar);
  Grabar_hum_L2.attach_function(1, fn_grabar_hum_modo);
  Grabar_hum_L3.attach_function(1, fn_grabar_hum_manual);
  Grabar_hum_L4.attach_function(1, fn_grabar_hum_horaenc);
  Grabar_hum_L5.attach_function(1, fn_grabar_hum_horaapg);

  pantallaGrabar_hum.set_focusPosition(Position::LEFT);
  pantallaGrabar_hum.set_displayLineCount(displayLineCount);
  pantallaGrabar_hum.set_encoder_interaction(LiquidScreen::enc_scrolling);
  pantallaGrabar_hum.set_focusedLine(0);
  menuInvernadero.add_screen(pantallaGrabar_hum); 

  //pantallaGrabar_hum_modo
  pantallaGrabar_hum_modo.add_line(Grabar_hum_modo_L1);
  pantallaGrabar_hum_modo.add_line(Grabar_hum_modo_L2);
  pantallaGrabar_hum_modo.set_encoder_interaction(LiquidScreen::enc_switch);
  menuInvernadero.add_screen(pantallaGrabar_hum_modo);

  //pantallaGrabar_hum_manual
  pantallaGrabar_hum_manual.add_line(Grabar_hum_manual_L1);
  pantallaGrabar_hum_manual.add_line(Grabar_hum_manual_L2);
  pantallaGrabar_hum_manual.set_encoder_interaction(LiquidScreen::enc_switch);
  menuInvernadero.add_screen(pantallaGrabar_hum_manual);

  //pantallaGrabar_hum_horaenc
  pantallaGrabar_hum_horaenc.add_line(Grabar_hum_horaenc_L1);
  pantallaGrabar_hum_horaenc.add_line(Grabar_hum_horaenc_L2);
  pantallaGrabar_hum_horaenc.set_encoder_interaction(LiquidScreen::enc_time);
  menuInvernadero.add_screen(pantallaGrabar_hum_horaenc);

  //pantallaGrabar_hum_horaapg
  pantallaGrabar_hum_horaapg.add_line(Grabar_hum_horaapg_L1);
  pantallaGrabar_hum_horaapg.add_line(Grabar_hum_horaapg_L2);
  pantallaGrabar_hum_horaapg.set_encoder_interaction(LiquidScreen::enc_time);
  menuInvernadero.add_screen(pantallaGrabar_hum_horaapg);

  //menuGrabar_luz
  pantallaGrabar_luz.add_line(Grabar_luz_L1);
  pantallaGrabar_luz.add_line(Grabar_luz_L2);
  pantallaGrabar_luz.add_line(Grabar_luz_L3);
  pantallaGrabar_luz.add_line(Grabar_luz_L4);
  pantallaGrabar_luz.add_line(Grabar_luz_L5);

  Grabar_luz_L1.attach_function(1, fn_grabar);
  Grabar_luz_L2.attach_function(1, fn_grabar_luz_modo);
  Grabar_luz_L3.attach_function(1, fn_grabar_luz_manual);
  Grabar_luz_L4.attach_function(1, fn_grabar_luz_horaenc);
  Grabar_luz_L5.attach_function(1, fn_grabar_luz_horaapg);

  pantallaGrabar_luz.set_focusPosition(Position::LEFT);
  pantallaGrabar_luz.set_displayLineCount(displayLineCount);
  pantallaGrabar_luz.set_encoder_interaction(LiquidScreen::enc_scrolling);
  pantallaGrabar_luz.set_focusedLine(0);
  menuInvernadero.add_screen(pantallaGrabar_luz); 

  //pantallaGrabar_luz_modo
  pantallaGrabar_luz_modo.add_line(Grabar_luz_modo_L1);
  pantallaGrabar_luz_modo.add_line(Grabar_luz_modo_L2);
  pantallaGrabar_luz_modo.set_encoder_interaction(LiquidScreen::enc_switch);
  menuInvernadero.add_screen(pantallaGrabar_luz_modo);

  //pantallaGrabar_luz_manual
  pantallaGrabar_luz_manual.add_line(Grabar_luz_manual_L1);
  pantallaGrabar_luz_manual.add_line(Grabar_luz_manual_L2);
  pantallaGrabar_luz_manual.set_encoder_interaction(LiquidScreen::enc_switch);
  menuInvernadero.add_screen(pantallaGrabar_luz_manual);

  //pantallaGrabar_luz_horaenc
  pantallaGrabar_luz_horaenc.add_line(Grabar_luz_horaenc_L1);
  pantallaGrabar_luz_horaenc.add_line(Grabar_luz_horaenc_L2);
  pantallaGrabar_luz_horaenc.set_encoder_interaction(LiquidScreen::enc_time);
  menuInvernadero.add_screen(pantallaGrabar_luz_horaenc);

  //pantallaGrabar_luz_horaapg
  pantallaGrabar_luz_horaapg.add_line(Grabar_luz_horaapg_L1);
  pantallaGrabar_luz_horaapg.add_line(Grabar_luz_horaapg_L2);
  pantallaGrabar_luz_horaapg.set_encoder_interaction(LiquidScreen::enc_time);
  menuInvernadero.add_screen(pantallaGrabar_luz_horaapg);

  //sistema de menus
  menuInvernadero.init();
  menuInvernadero.set_focusedLine(0);
  menuInvernadero.update();
  
}

void loop() {

  selectOption();
  encoderRotating();

  int C_screen = menuInvernadero.get_currentNumScreen();

  if(C_screen == 1){ // screen Monitorizar
    if(Sht31_update()) // si cambia valores de temp o hum, actualizar pantalla
      menuInvernadero.update();
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