#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include <Encoder.h>
#include <InterfazMenu.h>

// Configuración del encoder y pulsador
Encoder myEncoder(2, 3);
#define encoderButtonPin 6
long oldPosition = -999;

// Configuración del LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configuración de la interfaz del menú
LiquidSystem sys;
InterfazMenu interfazMenu(myEncoder, encoderButtonPin, sys, lcd);

void fn_Monitorizar()
{
    //interfazMenu.moverFlecha(true);
    //_sys.change_menu(_menuMonitorizar);
    interfazMenu.showMenuMonitorizar();
    // return;
}

void setup() {
  lcd.begin(16,2);
  //lcd.init();
  pinMode(encoderButtonPin, INPUT_PULLUP);

  //interfazMenu.mensajeInicial();
  delay(1000);
  
  // interfazMenu.configurarMenuPrincipal();
  // interfazMenu.configurarMenuMonitorizar();

  interfazMenu.configMenus();

  // interfazMenu.line12.attach_function(1, fn_Monitorizar);
  oldPosition = myEncoder.read();
  // interfazMenu.showMenuPrincipal();
  // interfazMenu._menuInvernadero.change_screen(1);
  interfazMenu.actualizar();
}

void selectOption(){
  if(digitalRead(encoderButtonPin)==LOW){
    sys.call_function(1);
    delay(500);
  }
}

void loop() {
  selectOption();
  long newPosition = myEncoder.read();
  if (newPosition != oldPosition) {
    if (newPosition > oldPosition)
      interfazMenu.moverFlecha(true);
    else
      interfazMenu.moverFlecha(false);
    
    oldPosition = newPosition;
    interfazMenu.actualizar();
  }
}
