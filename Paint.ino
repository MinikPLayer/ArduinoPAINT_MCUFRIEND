// the regular Adafruit "TouchScreen.h" library only works on AVRs

// different mcufriend shields have Touchscreen on different pins
// and rotation.
// Run the TouchScreen_Calibr_native sketch for calibration of your shield

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;       // hard-wired for UNO shields anyway.
#include <TouchScreen.h>
#include <string.h>

char *name = "320x240 allegro";  //edit name of shield
/*const int XP=7,XM=A1,YP=A2,YM=6; //240x320 ID=0x4747
const int TS_LEFT=172,TS_RT=833,TS_TOP=86,TS_BOT=884;*/

int XP=7,XM=A1,YP=A2,YM=6; //240x320 ID=0x4747
int TS_LEFT=250,TS_RT=846,TS_TOP=197,TS_BOT=901;

const int ustawieniaDotyku[2][4] = {
{ 172, 833, 96, 884},
{ 250, 846, 197, 901}
};

enum opcjeDotyku
{
  D_LEFT,
  D_RT,
  D_TOP,
  D_BOT
};

enum konfiguracjeDotyku
{
  KNF_USB,
  KNF_PB
};


TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 200
#define MAXPRESSURE 1000

int16_t BOXSIZE;
int16_t PENRADIUS = 1;
uint16_t ID, oldcolor, currentcolor;
uint8_t Orientation = 0;    //PORTRAIT

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define PINK    0xF891
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define BROWN   0x9A85
#define PURPLE  0x780F
#define ORANGE  0xFD00
#define SKINNY  0xFF58
#define GRAY    0x7BEF
#define LGREY   0xD6DA
#define DRED    0x7800
#define DGREEN  0x03E0
#define DBLUE   0x000F
#define DPINK   0xF891
#define GOLD    0xFEA0
#define TEAL    0x03EF
#define DBROWN  0x8222
#define INDIGO  0x480F
#define OrangeRED 0xFA20
#define DSKIN   0xCD91

#define ILOSCKOLOROW 24
#define ILOSCKOLOROWNASTRONE 4

const int clrclr = BLACK;

const int koloryNaEkranie[ILOSCKOLOROW] = {BLACK, WHITE, RED, GREEN, BLUE, PINK, YELLOW, CYAN, BROWN, PURPLE, ORANGE, SKINNY, GRAY, LGREY, DRED, DGREEN, DBLUE, DPINK, GOLD, TEAL, DBROWN, INDIGO, OrangeRED, DSKIN};

short strona = 0;
short iloscStron = ILOSCKOLOROW / ILOSCKOLOROWNASTRONE;

bool ostatnioPrzelaczony = false;


#define BUTTONDELAYTIME 50

/*const short BLACK = 0x0000;
const short BLUE = 0x001F;
const short RED = 0xF800;
const short GREEN = 0x07E0;
const short CYAN = 0x07FF;
const short MAGENTA = 0xF81F;
const short YELLOW = 0xFFE0;
const short WHITE = 0xFFFF;*/

// TouchScreen_Calibr_native for MCUFRIEND UNO Display Shields
// adapted by David Prentice
// for Adafruit's <TouchScreen.h> Resistive Touch Screen Library
// from Henning Karlsen's original UTouch_Calibration program.
// Many Thanks.

#define PORTRAIT  0
#define LANDSCAPE 1

#define TOUCH_ORIENTATION  PORTRAIT
#define TITLE "TouchScreen.h GFX Calibration"

#include <Adafruit_GFX.h>
#include "TouchScreen_kbv.h"

void readResistiveTouch(void)
{
    tp = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);  //because TFT control pins
    digitalWrite(XM, HIGH);
    //    Serial.println("tp.x=" + String(tp.x) + ", tp.y=" + String(tp.y) + ", tp.z =" + String(tp.z));
}

bool ISPRESSED(void)
{
    // .kbv this was too sensitive !!
    // now touch has to be stable for 50ms
    int count = 0;
    bool state, oldstate;
    while (count < 10) {
        readResistiveTouch();
        state = tp.z > 200;     //ADJUST THIS VALUE TO SUIT YOUR SCREEN e.g. 20 ... 250
        if (state == oldstate) count++;
        else count = 0;
        oldstate = state;
        delay(5);
    }
    return oldstate;
}

uint32_t cx, cy, cz;
uint32_t rx[8], ry[8];
int32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center, swapxy;
uint32_t calx, caly, cals;

char *Aval(int pin)
{
    static char buf[2][10], cnt;
    cnt = !cnt;
#if defined(ESP32)
    sprintf(buf[cnt], "%d", pin);
#else
    sprintf(buf[cnt], "A%d", pin - A0);
#endif
    return buf[cnt];
}

void showpins(int A, int D, int value, const char *msg)
{
    char buf[40];
    sprintf(buf, "%s (%s, D%d) = %d", msg, Aval(A), D, value);
    Serial.println(buf);
}

void calibSetup()
{
    Serial.begin(9600);
    Serial.println(TITLE);
    bool ret = true;
#if defined(__arm__) || defined(ESP32)
    Serial.println(F("Not possible to diagnose Touch pins on ARM or ESP32"));
#else
    //ret = diagnose_pins();
#endif
    uint16_t ID = tft.readID();
    Serial.print("ID = 0x");
    Serial.println(ID, HEX);
    //tft.begin(ID);
    //tft.setRotation(TOUCH_ORIENTATION);
    dispx = 240;//tft.width();
    dispy = 320;//tft.height();
    text_y_center = (dispy / 2) - 6;
    if (ret == false) {
        centerprint("BROKEN TOUCHSCREEN", text_y_center);
        while (true);    //just tread water
    }
}

void calibLoop(int & PRM_LEFT, int & PRM_RT, int & PRM_TOP, int & PRM_BOT)
{
    //startup();

    dispx = 240;
    dispy = 320;
    tft.fillScreen(BLACK);
    drawCrossHair(dispx - 11, 10, GRAY);
    drawCrossHair(dispx / 2, 10, GRAY);
    drawCrossHair(10, 10, GRAY);
    drawCrossHair(dispx - 11, dispy / 2, GRAY);
    drawCrossHair(10, dispy / 2, GRAY);
    drawCrossHair(dispx - 11, dispy - 11, GRAY);
    drawCrossHair(dispx / 2, dispy - 11, GRAY);
    drawCrossHair(10, dispy - 11, GRAY);
    centerprint("***********", text_y_center - 12);
    centerprint("***********", text_y_center + 12);

    calibrate(10, 10, 0, F(" LEFT, TOP, Pressure"));
    calibrate(10, dispy / 2, 1, F(" LEFT, MIDH, Pressure"));
    calibrate(10, dispy - 11, 2, F(" LEFT, BOT, Pressure"));
    calibrate(dispx / 2, 10, 3, F(" MIDW, TOP, Pressure"));
    calibrate(dispx / 2, dispy - 11, 4, F(" MIDW, BOT, Pressure"));
    calibrate(dispx - 11, 10, 5, F(" RT, TOP, Pressure"));
    calibrate(dispx - 11, dispy / 2, 6, F(" RT, MIDH, Pressure"));
    calibrate(dispx - 11, dispy - 11, 7, F(" RT, BOT, Pressure"));

    cals = (long(dispx - 1) << 12) + (dispy - 1);
    if (TOUCH_ORIENTATION == PORTRAIT) swapxy = rx[2] - rx[0];
    else swapxy = ry[2] - ry[0];
    swapxy = (swapxy < -500 || swapxy > 500);
    if ((TOUCH_ORIENTATION == PORTRAIT) ^ (swapxy != 0)) {
        clx = (rx[0] + rx[1] + rx[2]) / 3;
        crx = (rx[5] + rx[6] + rx[7]) / 3;
        cty = (ry[0] + ry[3] + ry[5]) / 3;
        cby = (ry[2] + ry[4] + ry[7]) / 3;
    } else {
        clx = (ry[0] + ry[1] + ry[2]) / 3;
        crx = (ry[5] + ry[6] + ry[7]) / 3;
        cty = (rx[0] + rx[3] + rx[5]) / 3;
        cby = (rx[2] + rx[4] + rx[7]) / 3;
    }
    px = float(crx - clx) / (dispx - 20);
    py = float(cby - cty) / (dispy - 20);
    //  px = 0;
    clx -= px * 10;
    crx += px * 10;
    cty -= py * 10;
    cby += py * 10;

    calx = (long(clx) << 14) + long(crx);
    caly = (long(cty) << 14) + long(cby);
    if (swapxy)
        cals |= (1L << 31);

    report(PRM_LEFT, PRM_RT, PRM_TOP, PRM_BOT);          // report results
    //while (true) {}    // tread water
    
}

void readCoordinates()
{
    int iter = 5000;
    int failcount = 0;
    int cnt = 0;
    uint32_t tx = 0;
    uint32_t ty = 0;
    boolean OK = false;

    while (OK == false)
    {
        centerprint("*  PRESS  *", text_y_center);
        while (ISPRESSED() == false) {}
        centerprint("*  HOLD!  *", text_y_center);
        cnt = 0;
        iter = 400;
        do
        {
            readResistiveTouch();
            if (tp.z > 20)
            {
                tx += tp.x;
                ty += tp.y;
                cnt++;
            }
            else
                failcount++;
        } while ((cnt < iter) && (failcount < 10000));
        if (cnt >= iter)
        {
            OK = true;
        }
        else
        {
            tx = 0;
            ty = 0;
            cnt = 0;
        }
        if (failcount >= 10000)
            fail();
    }

    cx = tx / iter;
    cy = ty / iter;
    cz = tp.z;
}

void calibrate(int x, int y, int i, String msg)
{
    drawCrossHair(x, y, WHITE);
    readCoordinates();
    centerprint("* RELEASE *", text_y_center);
    drawCrossHair(x, y, GRAY);
    rx[i] = cx;
    ry[i] = cy;
    Serial.print("\r\ncx="); Serial.print(cx);
    Serial.print(" cy="); Serial.print(cy);
    Serial.print(" cz="); Serial.print(cz);
    if (msg) Serial.print(msg);
    while (ISPRESSED() == true) {}
}

void report(int & PRM2_LEFT, int & PRM2_RT, int & PRM2_TOP, int & PRM2_BOT)
{
    uint16_t TS_LEFT, TS_RT, TS_TOP, TS_BOT, TS_WID, TS_HT, TS_SWAP;
    int16_t tmp;
    char buf[60];
    /*centertitle(TITLE);

    tft.println(F("To use the new calibration"));
    tft.println(F("settings you must map the values"));
    tft.println(F("from Point p = ts.getPoint() e.g. "));
    tft.println(F("x = map(p.x, LEFT, RT, 0, tft.width());"));
    tft.println(F("y = map(p.y, TOP, BOT, 0, tft.height());"));
    tft.println(F("swap p.x and p.y if diff ORIENTATION"));*/

    //.kbv show human values
    TS_LEFT = (calx >> 14) & 0x3FFF;
    TS_RT   = (calx >>  0) & 0x3FFF;
    TS_TOP  = (caly >> 14) & 0x3FFF;
    TS_BOT  = (caly >>  0) & 0x3FFF;
    TS_WID  = ((cals >> 12) & 0x0FFF) + 1;
    TS_HT   = ((cals >>  0) & 0x0FFF) + 1;
    TS_SWAP = (cals >> 31);
    if (TOUCH_ORIENTATION == LANDSCAPE) { //always show PORTRAIT first
        tmp = TS_LEFT, TS_LEFT = TS_BOT, TS_BOT = TS_RT, TS_RT = TS_TOP, TS_TOP = tmp;
        tmp = TS_WID, TS_WID = TS_HT, TS_HT = tmp;
    }

    PRM2_LEFT = TS_LEFT;
    PRM2_RT = TS_RT;
    PRM2_TOP = TS_TOP;
    PRM2_BOT = TS_BOT;
    /*tft.setCursor(0, 120);
    Serial.println("");
    sprintf(buf, "MCUFRIEND_kbv ID=0x%04X  %d x %d",
            tft.readID(), TS_WID, TS_HT);
    tft.println(buf);
    Serial.println(buf);
    sprintf(buf, "\nconst int XP=%d,XM=A%d,YP=A%d,YM=%d; //%dx%d ID=0x%04X", 
            XP, XM - A0, YP - A0, YM, TS_WID, TS_HT, tft.readID());
    Serial.println(buf);
    sprintf(buf, "const int TS_LEFT=%d,TS_RT=%d,TS_TOP=%d,TS_BOT=%d;", 
            TS_LEFT, TS_RT, TS_TOP, TS_BOT);
    Serial.println(buf);
    sprintf(buf, "PORTRAIT CALIBRATION     %d x %d", TS_WID, TS_HT);
    tft.println("");
    tft.println(buf);
    Serial.println(buf);
    sprintf(buf, "x = map(p.x, LEFT=%d, RT=%d, 0, %d)", TS_LEFT, TS_RT, TS_WID);
    tft.println(buf);
    Serial.println(buf);
    sprintf(buf, "y = map(p.y, TOP=%d, BOT=%d, 0, %d)", TS_TOP, TS_BOT, TS_HT);
    tft.println(buf);
    Serial.println(buf);
    sprintf(buf, "Touch Pin Wiring XP=%d XM=%s YP=%s YM=%d",
            XP, Aval(XM), Aval(YP), YM);
    tft.println("");
    tft.println(buf);
    Serial.println(buf);

    tmp = TS_LEFT, TS_LEFT = TS_TOP, TS_TOP = TS_RT, TS_RT = TS_BOT, TS_BOT = tmp;
    tmp = TS_WID, TS_WID = TS_HT, TS_HT = tmp;

    sprintf(buf, "LANDSCAPE CALIBRATION    %d x %d", TS_WID, TS_HT);
    tft.println("");
    tft.println(buf);
    Serial.println(buf);
    sprintf(buf, "x = map(p.y, LEFT=%d, RT=%d, 0, %d)", TS_LEFT, TS_RT, TS_WID);
    tft.println(buf);
    Serial.println(buf);
    sprintf(buf, "y = map(p.x, TOP=%d, BOT=%d, 0, %d)", TS_TOP, TS_BOT, TS_HT);
    tft.println(buf);
    Serial.println(buf);*/
}

void drawCrossHair(int x, int y, uint16_t color)
{
    tft.drawRect(x - 10, y - 10, 20, 20, color);
    tft.drawLine(x - 5, y, x + 5, y, color);
    tft.drawLine(x, y - 5, x, y + 5, color);
}

void centerprint(const char *s, int y)
{
    int len = strlen(s) * 6;
    tft.setTextColor(WHITE, RED);
    tft.setCursor((dispx - len) / 2, y);
    tft.print(s);
}

void centertitle(const char *s)
{
    tft.fillScreen(BLACK);
    tft.fillRect(0, 0, dispx, 14, RED);
    tft.fillRect(0, 14, dispx, 1, WHITE);
    centerprint(s, 1);
    tft.setCursor(0, 30);
    tft.setTextColor(WHITE, BLACK);
}

void startup()
{
    centertitle(TITLE);

    tft.println(F("#define NUMSAMPLES 3 in Library\n"));
    tft.println(F("Use a stylus or something"));
    tft.println(F("similar to touch as close"));
    tft.println(F("to the center of the"));
    tft.println(F("highlighted crosshair as"));
    tft.println(F("possible. Keep as still as"));
    tft.println(F("possible and keep holding"));
    tft.println(F("until the highlight is"));
    tft.println(F("removed. Repeat for all"));
    tft.println(F("crosshairs in sequence.\n"));
    tft.println(F("Report can be pasted from Serial\n"));
    tft.println(F("Touch screen to continue"));

    while (ISPRESSED() == false) {}
    while (ISPRESSED() == true) {}
    //    waitForTouch();
}

void fail()
{
    centertitle("Touch Calibration FAILED");

    tft.println(F("Unable to read the position"));
    tft.println(F("of the press. This is a"));
    tft.println(F("hardware issue and can"));
    tft.println(F("not be corrected in"));
    tft.println(F("software."));
    tft.println(F("check XP, XM pins with a multimeter"));
    tft.println(F("check YP, YM pins with a multimeter"));
    tft.println(F("should be about 300 ohms"));

    while (true) {};
}

void show_Serial(void)
{
    Serial.println(F("Most Touch Screens use pins 6, 7, A1, A2"));
    Serial.println(F("But they can be in ANY order"));
    Serial.println(F("e.g. right to left or bottom to top"));
    Serial.println(F("or wrong direction"));
    Serial.println(F("Edit name and calibration statements\n"));
    Serial.println(name);
    Serial.print(F("ID=0x"));
    Serial.println(ID, HEX);
    Serial.println("Screen is " + String(tft.width()) + "x" + String(tft.height()));
    Serial.println("Calibration is: ");
    Serial.println("LEFT = " + String(TS_LEFT) + " RT  = " + String(TS_RT));
    Serial.println("TOP  = " + String(TS_TOP)  + " BOT = " + String(TS_BOT));
    Serial.println("Wiring is always PORTRAIT");
    Serial.println("YP=" + String(YP)  + " XM=" + String(XM));
    Serial.println("YM=" + String(YM)  + " XP=" + String(XP));
}

bool Touch_getXY(int & param_x, int & param_y)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        param_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        param_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

Adafruit_GFX_Button PBCalibBTN, USBCalibBTN, CalibBTN; // Kalibracja: PowerBank, USB, Skalibruj

void show_tft(void)
{
    
    /*tft.setCursor(0, 0);
    tft.setTextSize(1);
    tft.print(F("ID=0x"));
    tft.println(ID, HEX);
    tft.println("Screen is " + String(tft.width()) + "x" + String(tft.height()));
    tft.println("");
    tft.setTextSize(2);
    tft.println(name);
    tft.setTextSize(1);
    tft.println("PORTRAIT Values:");
    tft.println("LEFT = " + String(TS_LEFT) + " RT  = " + String(TS_RT));
    tft.println("TOP  = " + String(TS_TOP)  + " BOT = " + String(TS_BOT));
    tft.println("\nWiring is: ");
    tft.println("YP=" + String(YP)  + " XM=" + String(XM));
    tft.println("YM=" + String(YM)  + " XP=" + String(XP));
    tft.setTextSize(2);
    tft.setTextColor(RED);
    tft.setCursor((tft.width() - 48) / 2, (tft.height() * 2) / 4);
    tft.print("EXIT");
    tft.setTextColor(YELLOW, BLACK);
    tft.setCursor(0, (tft.height() * 6) / 8);
    tft.print("Touch screen for loc");*/

    
    tft.setCursor(0,0);
    tft.setTextSize(2);
    PBCalibBTN.initButtonUL(&tft, 70, 0, 100, 100, RED, RED, WHITE, "PowerBank", 1);
    USBCalibBTN.initButtonUL(&tft, 70, 110, 100, 100, BLUE, BLUE, WHITE, "USB", 1);
    CalibBTN.initButtonUL(&tft, 70, 220, 100, 100, GOLD, GOLD, WHITE, "Calibrate", 1);

    PBCalibBTN.drawButton(false);
    USBCalibBTN.drawButton(false);
    CalibBTN.drawButton(false);

    delay(1000);
    
    while (1) {
        //tft.fillScreen(BLACK);
        //tp = ts.getPoint();
        //pinMode(XM, OUTPUT);
        //pinMode(YP, OUTPUT);
        //if (tp.z < MINPRESSURE || tp.z > MAXPRESSURE) continue;
        //if (tp.x > 450 && tp.x < 570  && tp.y > 450 && tp.y < 570) break;
        int posX, posY;
        posX = posY = 0;
        if(!Touch_getXY(posX, posY))
        {
          continue;
        }
        tft.setCursor(0, (tft.height() * 3) / 4);
        //tft.print("tp.x=" + String(tp.x) + " tp.y=" + String(tp.y) + "   ");
        tft.print("tp.x=" + String(posX) + " tp.y=" + String(posY) + "   ");
        PBCalibBTN.press(PBCalibBTN.contains(posX, posY));
        USBCalibBTN.press(USBCalibBTN.contains(posX, posY));
        CalibBTN.press(CalibBTN.contains(posX, posY));

        if(PBCalibBTN.justPressed())
        {
          TS_LEFT = ustawieniaDotyku[KNF_PB][D_LEFT];
          TS_RT = ustawieniaDotyku[KNF_PB][D_RT];
          TS_TOP = ustawieniaDotyku[KNF_PB][D_TOP];
          TS_BOT = ustawieniaDotyku[KNF_PB][D_BOT];
          PBCalibBTN.drawButton(true);
          tft.setCursor(0, 120);
          tft.print("PowerBank button clicked!");
          break;
        }
        if(USBCalibBTN.justPressed())
        {
          TS_LEFT = ustawieniaDotyku[KNF_USB][D_LEFT];
          TS_RT = ustawieniaDotyku[KNF_USB][D_RT];
          TS_TOP = ustawieniaDotyku[KNF_USB][D_TOP];
          TS_BOT = ustawieniaDotyku[KNF_USB][D_BOT];
          USBCalibBTN.drawButton(true);
          tft.setCursor(0, 120);
          tft.print("USB button clicked!");
          break;
        }
        if(CalibBTN.justPressed())
        {
          int LCL_LEFT, LCL_RT, LCL_TOP, LCL_BOT;
          LCL_LEFT = LCL_RT = LCL_TOP = LCL_BOT = 0;
          calibLoop(LCL_LEFT, LCL_RT, LCL_TOP, LCL_BOT);
          TS_LEFT = LCL_LEFT;
          TS_RT = LCL_RT;
          TS_TOP = LCL_TOP;
          TS_BOT = LCL_BOT;
          CalibBTN.drawButton(true);
          tft.setCursor(0, 120);
          tft.print("Calib button clicked!");
          break;
        }
        
    }
}



void drawCollorsPallet()
{
    //tft.fillScreen(BLACK);

    tft.fillRect(BOXSIZE * 0, 0, BOXSIZE, BOXSIZE, PINK);
    tft.fillRect(BOXSIZE * 1, 0, BOXSIZE, BOXSIZE, koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 0]);
    tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 1]);
    tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 2]);
    tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 3]);
    tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, PINK);

    tft.drawRect(BOXSIZE * 1, 0, BOXSIZE, BOXSIZE, WHITE);
    currentcolor = koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 0];
}

#define UIBUTTONSNUMBER 3
#define MAXBRUSHSIZE 11

Adafruit_GFX_Button UI_BUTTON[UIBUTTONSNUMBER];

int BOXSIZEUI;
int actualBrushSize = 0;
const int brushSizes[MAXBRUSHSIZE] = {0, 1, 2, 3, 5, 10, 15, 20, 25, 30, 50};

void initUIButtons()
{
  UI_BUTTON[0].initButtonUL(&tft, BOXSIZEUI * 0, 320 - BOXSIZEUI, BOXSIZEUI, BOXSIZEUI, WHITE, BLACK, WHITE, "CLR", 1);
  UI_BUTTON[1].initButtonUL(&tft, BOXSIZEUI * 1, 320 - BOXSIZEUI, BOXSIZEUI, BOXSIZEUI, WHITE, BLACK, WHITE, "-SIZE:", 1);
  UI_BUTTON[2].initButtonUL(&tft, BOXSIZEUI * 2, 320 - BOXSIZEUI, BOXSIZEUI, BOXSIZEUI, WHITE, BLACK, WHITE, "1 +", 1);
  
}

void drawUIButtons()
{
  tft.fillRect(0, 320 - BOXSIZEUI, 240, 320, BLACK);
  
  //tft.drawRect(BOXSIZEUI * 0, 320 - BOXSIZEUI, BOXSIZEUI,  BOXSIZEUI, WHITE);
  for(int i = 0;i<UIBUTTONSNUMBER;i++)
  {
    UI_BUTTON[i].drawButton(false);
  }
}

void drawUI()
{
  drawCollorsPallet();
  drawUIButtons();
}

void updateBrushSize()
{
    PENRADIUS = brushSizes[actualBrushSize];
    //char text[8];
    //char * cache = "SIZE";
    //char cache2[3];
    //itoa(PENRADIUS, cache2, 10);
    //text += cache;
    //sprintf(text, "%s %i", cache, PENRADIUS);
    //strcat(text, cache);
    //strcat(text, cache2);
    Serial.print("Changed brush size: PENRADIUS: ");
    Serial.print(PENRADIUS);
    Serial.print(" actualBrushSize: ");
    Serial.println(actualBrushSize);
    char text[5];
    int dziesietny = PENRADIUS/10;
    if(dziesietny > 0)
    {
      text[0] = dziesietny + 48;
      text[1] = PENRADIUS%10 + 48;
      text[2] = ' ';
      text[3] = '+';
      text[4] = '\0';
    }
    else
    {
      text[0] = PENRADIUS + 48;
      text[1] = ' ';
      text[2] = '+';
      text[3] = '\0';
    }
    UI_BUTTON[2].initButtonUL(&tft, BOXSIZEUI * 2, 320 - BOXSIZEUI, BOXSIZEUI, BOXSIZEUI, WHITE, BLACK, WHITE, text, 1);
    UI_BUTTON[2].drawButton(false);
}

void manageButtonsTouch(int xpos, int ypos)
{
  for(int i = 0;i<UIBUTTONSNUMBER;i++)
  {
    UI_BUTTON[i].press(UI_BUTTON[i].contains(xpos, ypos));
    if(UI_BUTTON[i].isPressed())
    {
      drawUIButtons();
    }
  }
  if(UI_BUTTON[0].justPressed())
  {
    clearScreen();
  }
  if(UI_BUTTON[1].justPressed())
  {
    if(actualBrushSize > 0)
    {
      actualBrushSize--;
    }
    else
    {
      actualBrushSize = MAXBRUSHSIZE - 1;
    }
    updateBrushSize();
  }
  if(UI_BUTTON[2].justPressed())
  {
    if(actualBrushSize < MAXBRUSHSIZE - 1)
    {
      actualBrushSize++;
    }
    else
    {
      actualBrushSize = 0;
    }
    updateBrushSize();
  }
}


bool areYouSure()
{
  tft.fillRect(0, 320 - BOXSIZEUI, 240, 320, BLACK);
  Adafruit_GFX_Button yes, no;
  yes.initButton(&tft, 240 * 2 / 3 , 320 - BOXSIZEUI + (BOXSIZEUI / 2.0), BOXSIZEUI, BOXSIZEUI / 2.0, WHITE, BLACK, WHITE, "YES", 1);
  no.initButton(&tft, 240 * 1 / 3 , 320 - BOXSIZEUI + (BOXSIZEUI / 2.0), BOXSIZEUI, BOXSIZEUI / 2.0, WHITE, BLACK, WHITE, "NO", 1);
  yes.drawButton(false);
  no.drawButton(false);

  while(true)
  {
    int local_x, local_y;
    local_x = local_y = 0;
    Touch_getXY(local_x, local_y);
    yes.press(yes.contains(local_x, local_y));
    no.press(no.contains(local_x, local_y));
    if(yes.isPressed())
    {
      return 1;
    }
    if(no.isPressed())
    {
      return 0;
    }
  }
}

void clearScreen()
{
  if(!areYouSure())
  {
    drawUIButtons();
    return;
  }
  tft.fillRect(0, BOXSIZE, tft.width(), tft.height() - BOXSIZE, BLACK);
  drawUIButtons();
}

void setup(void)
{
    
    Serial.begin(9600);  
    uint16_t tmp;

    //delay(10000);
    tft.reset();
    ID = tft.readID();
    tft.begin(ID);
    
    //show_Serial();
    tft.setRotation(Orientation);
    tft.fillScreen(BLACK);
    show_tft();

    BOXSIZE = tft.width() / (ILOSCKOLOROWNASTRONE + 2);

    BOXSIZEUI = BOXSIZE;

    tft.fillScreen(BLACK);
    
    //drawCollorsPallet();
    initUIButtons();
    drawUI();

    pinMode(LED_BUILTIN, OUTPUT); 
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    
    //delay(1000);
}

unsigned long delayButtonTime = 0;
unsigned long delayButtonTimeCache = 0;

bool pageChanged = false;



void loop()
{
    uint16_t xpos, ypos;  //screen coordinates
    tp = ts.getPoint();   //tp.x, tp.y are ADC values

    // if sharing pins, you'll need to fix the directions of the touchscreen pins
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!

    if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
        // most mcufriend have touch (with icons) that extends below the TFT
        // screens without icons need to reserve a space for "erase"
        // scale the ADC values from ts.getPoint() to screen values e.g. 0-239
        //
        // Calibration is true for PORTRAIT. tp.y is always long dimension 
        // map to your current pixel orientation
        switch (Orientation) {
            case 0:
                xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
                ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
                break;
            case 1:
                xpos = map(tp.y, TS_TOP, TS_BOT, 0, tft.width());
                ypos = map(tp.x, TS_RT, TS_LEFT, 0, tft.height());
                break;
            case 2:
                xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
                ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
                break;
            case 3:
                xpos = map(tp.y, TS_BOT, TS_TOP, 0, tft.width());
                ypos = map(tp.y, TS_LEFT, TS_RT, 0, tft.height());
                break;
        }

        // are we in top color box area ?
        if (ypos < BOXSIZE) {               //draw white border on selected color box
            oldcolor = currentcolor;

            if (xpos < BOXSIZE) {
                //currentcolor = koloryNaEkranie[0];
                //tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
                if(strona > 0 && !ostatnioPrzelaczony)
                {
                  strona--;
                  ostatnioPrzelaczony = true;
                  pageChanged = true;
                }
                drawCollorsPallet();
            } else if (xpos < BOXSIZE * 2) {
                currentcolor = koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 0];
                tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
                ostatnioPrzelaczony = false;
                if(currentcolor == WHITE) tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, BLACK);
            } else if (xpos < BOXSIZE * 3) {
                currentcolor = koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 1];
                tft.drawRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, WHITE);
                ostatnioPrzelaczony = false;
                if(currentcolor == WHITE) tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, BLACK);
            } else if (xpos < BOXSIZE * 4) {
                currentcolor = koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 2];
                tft.drawRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, WHITE);
                ostatnioPrzelaczony = false;
                if(currentcolor == WHITE) tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, BLACK);
            } else if (xpos < BOXSIZE * 5) {
                currentcolor = koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 3];
                tft.drawRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, WHITE);
                ostatnioPrzelaczony = false;
                if(currentcolor == WHITE) tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, BLACK);
            } else if (xpos < BOXSIZE * 6) {
                //currentcolor = koloryNaEkranie[5];
                //tft.drawRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, WHITE);
                if(strona < iloscStron - 1 && !ostatnioPrzelaczony)
                {
                  strona++;
                  ostatnioPrzelaczony = true;
                  //delayButtonTime = millis();
                  pageChanged = true;
                  
                }
                drawCollorsPallet();
            }

            if (oldcolor != currentcolor) { //rub out the previous white border
                if (oldcolor == koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 0]) tft.fillRect(BOXSIZE * 1, 0, BOXSIZE, BOXSIZE, koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 0]);
                if (oldcolor == koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 1]) tft.fillRect(BOXSIZE * 2, 0, BOXSIZE, BOXSIZE, koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 1]);
                if (oldcolor == koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 2]) tft.fillRect(BOXSIZE * 3, 0, BOXSIZE, BOXSIZE, koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 2]);
                if (oldcolor == koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 3]) tft.fillRect(BOXSIZE * 4, 0, BOXSIZE, BOXSIZE, koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 3]);
                //if (oldcolor == koloryNaEkranie[strona * ILOSCKOLOROWNASTRONE + 4]) tft.fillRect(BOXSIZE * 0, 0, BOXSIZE, BOXSIZE, koloryNaEkranie[4]);
                //if (oldcolor == clrclr) tft.fillRect(BOXSIZE * 5, 0, BOXSIZE, BOXSIZE, clrclr);
            }
        }
        // are we in drawing area ?
        if (((ypos - PENRADIUS) > BOXSIZE) && ((ypos + PENRADIUS) < tft.height())) {
          if((ypos - ((PENRADIUS / 2.0) + BOXSIZE) > 0) && (ypos + ((PENRADIUS)+(BOXSIZEUI)) < 320))
          {
            tft.fillCircle(xpos, ypos, PENRADIUS, currentcolor);
          }
        }
        // are we in erase area ?
        // Plain Touch panels use bottom 10 pixels e.g. > h - 10
        // Touch panels with icon area e.g. > h - 0
        /*if (ypos > tft.height() - 10) {
            // press the bottom of the screen to erase
            clearScreen();
        }*/
        
        manageButtonsTouch(xpos, ypos);
    } else
    {
      //ostatnioPrzelaczony = false;
      manageButtonsTouch(xpos, ypos);
    }
    /*if(millis() > delayButtonTime + BUTTONDELAYTIME)
    {
      ostatnioPrzelaczony = false;
    }*/
    if(tp.z < MINPRESSURE)
    {
      if(pageChanged)
      {
        pageChanged = false;
        delayButtonTime = millis();
      }
      if(millis() > delayButtonTime + BUTTONDELAYTIME)
      {
        ostatnioPrzelaczony = false;
      }
    }
    else
    {
      if(!(millis() > delayButtonTime + BUTTONDELAYTIME))
      {
        delayButtonTime = millis();
      }
    }
    if(millis() < 1000)
    {
      while(true)
      {
        tft.fillScreen(RED);
      }
    }
    /*pinMode(LED_BUILTIN, OUTPUT); 
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);*/
}
