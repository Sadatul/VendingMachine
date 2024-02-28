#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
}

void loop()
{
  updateSerial();
}

void updateSerial()
{
  delay(500);
  char response[64] = {0};
  int len = 0;
  while(mySerial.available()) 
  {
    char c = mySerial.read();
    Serial.write(c);//Forward what Software Serial received to Serial Port

    response[len++] = c;
  }

  if (len) {
    //Serial.println();
    // for (int i = 0; i < len; ++i) {
    //   Serial.print((int)response[i]);
    //   Serial.print(",");
    // }
    // Serial.println();
    int amount = getAmount(response);
    if (amount) {
      Serial.print("Recharged Tk ");
      Serial.println(amount);
    }
  }
}

int getAmount(char SMS[64]) {
    static char validSMSPrefix[] = "\r\n+CMT: \"+8801531720723";
    static int validSMSPrefixLen = sizeof(validSMSPrefix) - 1;

    {
        int i;
        for (i = 0; i < validSMSPrefixLen; ++i) {
            if (validSMSPrefix[i] != SMS[i]) return 0;
        }
    }

    {
        char buffer[10] = {0};
        int i;
        for (i = 53; SMS[i] != ' ' && SMS[i] != '.'; ++i) {
            buffer[i - 53] = SMS[i];
        }
        return atoi(buffer);
    }

    return 0;
}