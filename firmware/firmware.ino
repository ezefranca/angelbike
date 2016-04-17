#include <U8glib.h>
#include "right.h"
#include "right_line.h"
#include "left.h"
#include "left_line.h"
#include <SoftwareSerial.h>

#define BLE_RX  2
#define BLE_TX  3
#define BUZZER 9

#define NOTIF_FREQ 3000

#define DIR_RIGHT_CMD 1
#define DIR_LEFT_CMD 2
#define NOT_CMD 3

int cmdCode = 0, flip = 0, notif = 0, oldCmdCode;

SoftwareSerial BLE (BLE_RX,BLE_TX); // RX, TX
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

void POST(void) {
  BLE.println("AT");
  delay(100);
  if ( BLE.find("OK") ) u8g.drawStr(0,20, "BLE is OK!");
  else u8g.drawStr(0,20, "BLE failed!");
}

void drawRightArrow() {
  if (flip) u8g.drawXBMP(0,0, right_width, right_height, right_bits);
  else u8g.drawXBMP(0,0, right_line_width, right_line_height, right_line_bits);
}

void drawLeftArrow() {
  if (flip) u8g.drawXBMP(0,0, left_width, left_height, left_bits);
  else u8g.drawXBMP(0,0, left_line_width, left_line_height, left_line_bits);
}

void notify(int code) {
  if (!notif) return;
  notif = 0;
  tone(BUZZER, NOTIF_FREQ);
  delay(200 * (code - NOT_CMD + 1));
  noTone(BUZZER);
  delay(200 * (code - NOT_CMD + 1));
  for(int i = 0; i < code - NOT_CMD; i++) {
    tone(BUZZER, NOTIF_FREQ);
    delay(200 * (code - NOT_CMD + 1 + i));
    noTone(BUZZER);
    delay(200 * (code - NOT_CMD + 1 - i));
  }
}



void setup(void) {
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  BLE.begin(9600);
  u8g.setFont(u8g_font_unifont);
    
}

int mapCmd2Code(String cmd) {
  notif = NOTIF_FREQ;
  if (cmd.startsWith("DIR+RIGHT") ) return DIR_RIGHT_CMD;
  else if (cmd.startsWith("DIR+LEFT") ) return DIR_LEFT_CMD;
  else if (cmd.startsWith("NOT+1") ) {
    notif = 1;
    return NOT_CMD;
  }
  else if (cmd.startsWith("NOT+2") ) {
    notif = 1;
    return NOT_CMD + 1;
  }
  else if (cmd.startsWith("NOT+3") ) {
    notif = 1;
    return NOT_CMD + 2;
  }
  else if (cmd.startsWith("NOT+4") ) {
    notif = 1;
    return NOT_CMD + 3;
  }
  else if (cmd.startsWith("NOT+5") ) {
    notif = 1;
    return NOT_CMD + 4;
  }
}

void loop(void) {
  String cmd;
  cmd = Serial.readStringUntil('\n');
  if (cmd.length() >= 3) {
    oldCmdCode = cmdCode;
    cmdCode = mapCmd2Code(cmd);
    if (cmdCode > DIR_LEFT_CMD && cmdCode <= NOT_CMD + 4) {
      notify(cmdCode);
      cmdCode = oldCmdCode; 
    }
  }
  
  // picture loop
  u8g.firstPage();  
  do {
    if (cmdCode == 0) POST();
    else if (cmdCode == DIR_RIGHT_CMD) drawRightArrow();
    else if (cmdCode == DIR_LEFT_CMD) drawLeftArrow();
  } while( u8g.nextPage() );
  
  flip = !flip;
  delay(100);
}

