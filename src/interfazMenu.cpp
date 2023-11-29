#include "InterfazMenu.h"

InterfazMenu::InterfazMenu(Encoder &encoder, int encoderButtonPin, LiquidSystem &sys, LiquidCrystal_I2C &lcd)
    : _encoder(encoder), _encoderButtonPin(encoderButtonPin), _sys(sys), _lcd(lcd)
{
    //_menuInvernadero = LiquidMenu(_lcd);
}

void InterfazMenu::mensajeInicial()
{

    _lcd.setCursor(0, 0);
    _lcd.print("Universidad del");
    _lcd.setCursor(0, 1);
    _lcd.print("pacifico");
}

void InterfazMenu::configurarMenuPrincipal()
{
    line12 = LiquidLine  (1, 0, "Monitorizar");
    LiquidLine line1 (1, 0, "Monitorizar");
    LiquidLine line2(1, 1, "Control Temp");
    LiquidLine line3(1, 1, "Control Luz");
    LiquidLine line4(1, 1, "Grabar");
    // Add more "lines" than the display has. The extra will be scrolled.
    // LiquidScreen screenMenuPrincipal(line12, line2, line3, line4);
    screenMenuPrincipal = LiquidScreen(line1, line2, line3, line4);
    // Attaching a function to the lines is required for scrolling to work.
    //line1.attach_function(1, reinterpret_cast<void(*)()>(&InterfazMenu::fn_Monitorizar));
    line1.attach_function(1, &blankFunction);
    line2.attach_function(1, &blankFunction);
    line3.attach_function(1, &blankFunction);
    line4.attach_function(1, &blankFunction);

    screenMenuPrincipal.set_displayLineCount(2);

    _menuInvernadero.add_screen(screenMenuPrincipal);
    _menuInvernadero.set_focusedLine(0);
    _menuInvernadero.set_focusPosition(Position::LEFT);

    //_menuInvernadero.update();


    // Set the number of decimal places for a "line".
    // line1.set_decimalPlaces(5);

    //_menuPrincipal.update();
    
    //_menuPrincipal.update();
}

void InterfazMenu::configMenus(){

    LiquidLine line1 (1, 0, "Monitorizar");
    LiquidLine line2(1, 1, "Control Temp");
    LiquidLine line3(1, 0, "Control Luz");
    LiquidLine line4(1, 1, "Grabar");

    LiquidScreen screenMenuPrincipal (line1, line2, line3, line4);
    _menuInvernadero = LiquidMenu(_lcd, screenMenuPrincipal,1);

    line1.set_focusPosition(Position::LEFT);
    line2.set_focusPosition(Position::LEFT);
    line3.set_focusPosition(Position::LEFT);
    line4.set_focusPosition(Position::LEFT);

    line1.attach_function(1, &blankFunction);
    line2.attach_function(1, &blankFunction);
    line3.attach_function(1, &blankFunction);
    line4.attach_function(1, &blankFunction);

    screenMenuPrincipal.set_displayLineCount(2);

    //second Screen

    // LiquidLine line5 (1, 0, "T:16");
    // LiquidLine line6(1, 1, "H:60");

    // LiquidScreen screenMenuMonitorizar (line5, line6);
    // _menuInvernadero.add_screen(screenMenuPrincipal);

    // line5.set_focusPosition(Position::LEFT);
    // line6.set_focusPosition(Position::LEFT);

    // line5.attach_function(1, &blankFunction);
    // line6.attach_function(1, &blankFunction);

    // screenMenuMonitorizar.set_displayLineCount(2);

    //

    _menuInvernadero.init();
    _menuInvernadero.set_focusedLine(0);
    
}

void InterfazMenu::configurarMenuMonitorizar()
{

    LiquidLine Monitorizarline1(1, 0, "T: 25C");
    LiquidLine Monitorizarline2(1, 1, "H: 50%");

    // // Add more "lines" than the display has. The extra will be scrolled.
    // LiquidScreen screenMenuMonitorizar(Monitorizarline1, Monitorizarline2);

    screenMenuMonitorizar = LiquidScreen(Monitorizarline1, Monitorizarline2);

    // _menuMonitorizar.add_screen(screenMenuMonitorizar);
    // _menuMonitorizar.set_focusedLine(0);

    screenMenuMonitorizar.set_displayLineCount(2);
    _menuInvernadero.add_screen(screenMenuMonitorizar);
    _menuInvernadero.set_focusedLine(0);
    
    //_menuInvernadero.update();
}

void InterfazMenu::configurarMenuControlTemperatura()
{

    LiquidLine line1(1, 0, "T: ", "24");
    LiquidLine line2(8, 1, "H: ", "10");

    // Add more "lines" than the display has. The extra will be scrolled.
    LiquidScreen screenMenuMonitorizar(line1, line2);

    _menuMonitorizar.add_screen(screenMenuMonitorizar);

    _menuMonitorizar.update();
}

void InterfazMenu::blankFunction()
{
    
    return;
}

void InterfazMenu::fn_Monitorizar()
{
    moverFlecha(true);
    //_sys.change_menu(_menuMonitorizar);
    return;
}

void InterfazMenu::actualizar()
{
    _menuInvernadero.update();
    //_sys.change_screen();
}
void InterfazMenu::moverFlecha(bool mover)
{
    _menuInvernadero.switch_focus(mover);
}


void InterfazMenu::showMenuMonitorizar()
{
    _menuInvernadero.change_screen(1);
}

void InterfazMenu::showMenuPrincipal()
{
    _menuInvernadero.change_screen(2);
    
}

// void InterfazMenu::fn_monitorizar() {
//       return;
//   }

// void InterfazMenu::fn_controlTemp() {
//       return;
//   }

// void InterfazMenu::fn_controlLuz() {
//       return;
//   }

// void InterfazMenu::fn_grabar() {
//       return;
//}
