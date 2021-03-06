#include "LEDMATRIX.h"

//192.168.1.142/informer?token=esp8266&bright=10&power=0&priority=high&string=привет

#ifdef LED_MATRIX

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
//          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
//          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}

void LedMatrixInit() {
  //Отображакм при имя wifi потключенной сети или точки доступа и ip адрес
  if(Config.ap == "on"){
    tapeMatrix = "AP-SSID:" + ssid1 + "   AP-IP:" + Config.ipap ;
  }
  /*else
  {
    tapeMatrix = "WIFI SSID:" + Config.ssid + " IP:" + Config.ip ;
  } */
  
  matrix.setIntensity(7); // Use a value between 0 and 15 for brightness
  /*
  // Adjust to your own needs
  matrix.setPosition(0, 3, 0); // The first display is at <0, 0>
  matrix.setPosition(1, 2, 0); // The second display is at <1, 0>
  matrix.setPosition(2, 1, 0); // The third display is at <2, 0>
  matrix.setPosition(3, 0, 0); // And the last display is at <3, 0>
  //  ...
  matrix.setRotation(0, 3);    // The first display is position upside down
  matrix.setRotation(1, 3);    // The first display is position upside down
  matrix.setRotation(2, 3);    // The first display is position upside down
  matrix.setRotation(3, 3);    // The same hold for the last display
  */
  // Adjust to your own needs
  matrix.setPosition(0, 4, 0); // The first display is at <0, 0>
  matrix.setPosition(1, 3, 0); // The second display is at <1, 0>
  matrix.setPosition(2, 2, 0); // The third display is at <2, 0>
  matrix.setPosition(3, 1, 0); // And the last display is at <3, 0>
  matrix.setPosition(4, 0, 0); // And the last display is at <3, 0>
//  ...
  matrix.setRotation(0, 3);    // The first display is position upside down
  matrix.setRotation(1, 3);    // The first display is position upside down
  matrix.setRotation(2, 3);    // The first display is position upside down
  matrix.setRotation(3, 3);    // The same hold for the last display
  matrix.setRotation(4, 3);    // The same hold for the last display

  VersionCode += VersionLedMatrix ;
}

void LedMatrix() {
  
  //tape=utf8rus("Проверка информера: Сегодня наступило лето, температура на улице  +22С и солнышко.");
  if(MatrixOFF == 0)
  {
    static long previousMillis = 0;                             
    long currentMillis = millis();
    
    if(currentMillis - previousMillis > wait) 
    {  
      previousMillis = currentMillis; 
      
      if(MatrixCounter == 0) tape=utf8rus(tapeMatrix);    //Загружаем новую строку если закончилось отображение предедущей
      
      if(MatrixCounter < width * tape.length() + matrix.width() - 1 - spacer)
      {
        matrix.fillScreen(LOW);
    
        int letter = MatrixCounter / width;
        int x = (matrix.width() - 1) - MatrixCounter % width;
        int y = (matrix.height() - 8) / 2; // center the text vertically
    
        while ( x + width - spacer >= 0 && letter >= 0 ) {
          if ( letter < tape.length() ) {
            matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
          }
          letter--;
          x -= width;
        }
        matrix.write(); // Send bitmap to display
        MatrixCounter++ ;
        //delay(wait);
      }else
      {
        MatrixCounter = 0 ; 
      }
    }
  }
}

//Обрабатываем ВЕБ запросы
void handleLedMatrix()
{
    String buf = server.arg("token");
    Serial.println(buf);
  
    if (Config.www_password != buf) {
      String message = "access denied";
      server.send(401, "text/plain", message);
      return;
    }
    buf = server.arg("bright");
    char b[3];
    int z = 100;
    if(buf.length())
    {
      buf.getBytes((unsigned char *)b, 3);
      z = StrToULong(b);
      if(z != MatrixOFF)
      {
        MatrixOFF = z;
        Serial.print("LED MATRIX bright ");
        Serial.println(StrToULong(b));
        Serial.println(buf.length());
        matrix.setIntensity(StrToULong(b)); 
      }
    }

    buf = server.arg("power");
    if(buf.length())
    {
      buf.getBytes((unsigned char *)b, 3);
      z = StrToULong(b);
      if(z != MatrixOFF)
      {
        MatrixOFF = z;
        Serial.print("LED MATRIX power ");
        Serial.println(MatrixOFF);
        Serial.println(buf.length());
        //matrix.fillScreen(1);
        matrix.shutdown(MatrixOFF); 
        MatrixCounter = 0;      //Начинаем новую строку сначала
      }
    }
    
    buf = server.arg("priority");
    if(buf.length())  // проверяем задан ли параметр
    {
      if (buf == "high")
      {
        Serial.print("Priority high");
        MatrixCounter = 0;      //Начинаем новую строку сначала
      }
    }
    
    buf = server.arg("string");
    if(buf.length())  // проверяем задан ли параметр
    {
      Serial.print("LED MATRIX sring:  ");
      Serial.println(buf);
      tapeMatrix=buf;
      //MatrixCounter = 0;      //Начинаем новую строку сначала
    }

        String message = "success";
    server.send(200, "text/plain", message);  
}

#endif
