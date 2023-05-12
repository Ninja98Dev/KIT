#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <BluetoothSerial.h>
#include <ELMduino.h>

//  Display varibales 2, 4, 126, 60,
#define i2c_Address 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//  ELM327 variables
BluetoothSerial SerialBT;
#define ELM_PORT   SerialBT
#define DEBUG_PORT Serial
ELM327 myELM327;

uint32_t rpm = 0;
uint32_t pids;

void connect(){
  if (!ELM_PORT.connect("OBDII"))
  {
    /*
    display.print("Couldn't connect to OBD scanner - Phase 1");
    display.display();
    */
    while(1);
  }

  if (!myELM327.begin(ELM_PORT, true, 2000))
  {
    /*
    display.print("Couldn't connect to OBD scanner - Phase 2");
    display.display();
    */
    while (1);
  }

  /*
  display.print("Connected to KITT");
  display.display();
  */
}

void setup() {
  //  Set up display
  delay(250);
  display.begin(i2c_Address, true);
  display.display();
  delay(500);
  display.clearDisplay();

  #if LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
  #endif

  DEBUG_PORT.begin(115200);
  SerialBT.setPin("1234");
  ELM_PORT.begin("KITT", true);

  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(2, 4);
  
  connect();
}

const unsigned char temps_icons [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x30, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0f, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0xfc, 0x30, 0x00, 0x00, 0x00, 0x00, 
	0x0f, 0x86, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3f, 0xcc, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0d, 0xff, 0x87, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x18, 0x44, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0xff, 0xcf, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x30, 0xa5, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x30, 0x9a, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0c, 0xff, 0xff, 0x08, 0x00, 0x00, 0x00, 0x00, 0x30, 0xee, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0e, 0xff, 0xfe, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x30, 0x9d, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07, 0xff, 0xfe, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x30, 0x96, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xff, 0xfc, 0x08, 0x00, 0x00, 0x00, 0x00, 0x30, 0xa9, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x48, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3f, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3b, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x30, 0x19, 0xcc, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x71, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x38, 0x3b, 0xee, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1f, 0xf3, 0xe7, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0f, 0xe7, 0xf3, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x03, 0x83, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x30, 0x1b, 0xec, 0x06, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x38, 0x3c, 0x1e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1e, 0xff, 0x7f, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1f, 0xf7, 0xf7, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07, 0xc3, 0xe1, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint32_t oilTemp = 80;
uint32_t coolantTemp = 32;
uint32_t intakeTemp = 41;
uint32_t outSideTemp = 98;

void loop() {
  //  Check connection
  if(!ELM_PORT.hasClient()){
    connect();
  }

  //  Get oil temp
  float tempOilTemp = myELM327.oilTemp();
  if (myELM327.nb_rx_state == ELM_SUCCESS)
  {
    oilTemp = (uint32_t)tempOilTemp;
  }
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    myELM327.printError();

  //  Get coolant temp
  float tempCoolantTemp = myELM327.engineCoolantTemp();
  if (myELM327.nb_rx_state == ELM_SUCCESS)
  {
    coolantTemp = (uint32_t)tempCoolantTemp;
  }
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    myELM327.printError();

  //  Get intake temp
  float tempIntakeTemp = myELM327.intakeAirTemp();
  if (myELM327.nb_rx_state == ELM_SUCCESS)
  {
    intakeTemp = (uint32_t)tempIntakeTemp;
  }
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    myELM327.printError();
 
  //  Get outSide temp
  float tempOutSideTemp = myELM327.ambientAirTemp();
  if (myELM327.nb_rx_state == ELM_SUCCESS)
  {
    outSideTemp = (uint32_t)tempOutSideTemp;
  }
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
    myELM327.printError();

  //  Display everything
  display.clearDisplay();
  display.drawBitmap(2, 4, temps_icons, 126, 60, SH110X_WHITE);

  display.setCursor(39, 12);
  display.print(oilTemp);
  display.setCursor(39, 43);
  display.print(coolantTemp);
  display.setCursor(99, 12);
  display.print(intakeTemp);
  display.setCursor(99, 43);
  display.print(outSideTemp);

  display.display();
}