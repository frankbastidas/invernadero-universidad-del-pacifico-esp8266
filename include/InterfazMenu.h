#ifndef InterfazMenu_h
#define InterfazMenu_h

#include "Arduino.h"
#include <LiquidMenu.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

// Used for attaching something to the lines, to make them focusable.


class InterfazMenu {
public:
  InterfazMenu(Encoder &encoder, 
  int encoderButtonPin, LiquidSystem &sys, LiquidCrystal_I2C &lcd);
  
  void configurarMenuPrincipal();
  void configurarMenuMonitorizar();
  void configurarMenuControlTemperatura();
  void configurarMenuControlLuz();
  void configurarMenuGrabar();
  void mensajeInicial();
  // Used for attaching something to the lines, to make them focusable.
  static void blankFunction();
  void fn_Monitorizar();

  void showMenuMonitorizar();
  void showMenuPrincipal();

  void configMenus();

 /// Actualizar sistema de menus
  /**
  Actualiza los valores de las variables a mostrar en cada
  linea, pantalla y menu del sistema.
  */
  void actualizar();

  /// Mover flecha
  /**
  Mueve la flecha hacia la siguiente linea o la anterior
  deacuerdo al parametro "mover".

  @param mover - true para el siguiente, false para el anterior
  */
  void moverFlecha(bool mover);


  LiquidMenu _menuPrincipal;
  LiquidMenu _menuMonitorizar;
  LiquidMenu _menuControlTemp;
  LiquidMenu _menuControlLuz;
  LiquidMenu _menuGrabar;
  LiquidMenu _menuInvernadero;

  LiquidScreen screenMenuPrincipal;
  LiquidScreen screenMenuMonitorizar;
  
  LiquidLine line12;

private:
  Encoder &_encoder;
  int _encoderButtonPin;
  LiquidCrystal_I2C &_lcd;
  LiquidSystem &_sys;

};

#endif