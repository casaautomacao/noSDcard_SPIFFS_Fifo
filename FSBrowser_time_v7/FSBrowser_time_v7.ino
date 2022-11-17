long tamArquivoMax = 200;
long quantArqMax = 10;// tem que ser 2 ou mais...


/*
 
*/


/*
  FSWebServer - Example WebServer with FS backend for esp8266/esp32
  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the WebServer library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  upload the contents of the data folder with MkSPIFFS Tool ("ESP32 Sketch Data Upload" in Tools menu in Arduino IDE)
  or you can upload the contents of a folder if you CD in that folder and run the following command:
  for file in `ls -A1`; do curl -F "file=@$PWD/$file" esp32fs.local/edit; done

  access the sample web page at http://esp32fs.local
  edit the page by going to http://esp32fs.local/edit
*/


int numArquivo = 0;

int quantArqDados = 0;
long somaTamanhos = 0;

#include <Crescer.h>

Tempora temp1;
Tempora tempImpr;

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <time.h>


long timezone = -3;
byte daysavetime = 1;

#define FILESYSTEM SPIFFS
// You only need to format the filesystem once
#define FORMAT_FILESYSTEM false
#define DBG_OUTPUT_PORT Serial

#if FILESYSTEM == FFat
#include <FFat.h>
#endif
#if FILESYSTEM == SPIFFS
#include <SPIFFS.h>
#endif

const char* ssid = "crescer";
const char* password = "cpb32832";
const char* host = "esp32fs";
WebServer server(80);
//holds the current upload
File fsUploadFile;

File myFile;



void setup(void) {

 

  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.print("\n");
  DBG_OUTPUT_PORT.setDebugOutput(true);
  if (FORMAT_FILESYSTEM) FILESYSTEM.format();
  FILESYSTEM.begin();
  {
    File root = FILESYSTEM.open("/");
    File file = root.openNextFile();


    while (file) {
      String fileName = file.name();
      size_t fileSize = file.size();

      Serial.print( file.peek());
      Serial.print( "  -  " );

      if (fileName.indexOf("dados") > -1)
      {
        quantArqDados++;

        somaTamanhos = somaTamanhos + fileSize;
      }

      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
      file = root.openNextFile();
    }

    Serial.print("quant de arq de dados: ");
    Serial.print(quantArqDados);
    Serial.print("tamanho total de dados: ");
    Serial.print(somaTamanhos);

    // numArquivo =
    DBG_OUTPUT_PORT.printf("\n");
  }


  //WIFI INIT
  DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);
  if (String(WiFi.SSID()) != String(ssid)) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DBG_OUTPUT_PORT.print(".");
  }
  DBG_OUTPUT_PORT.println("");
  DBG_OUTPUT_PORT.print("Connected! IP address: ");
  DBG_OUTPUT_PORT.println(WiFi.localIP());



  MDNS.begin(host);
  DBG_OUTPUT_PORT.print("Open http://");
  DBG_OUTPUT_PORT.print(host);
  DBG_OUTPUT_PORT.println(".local/edit to see the file browser");


  //SERVER INIT
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on("/edit", HTTP_GET, []() {

    // Serial.println("passo 1");

    if (!handleFileRead("/edit.htm")) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from FILESYSTEM
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {

      //    Serial.println("passo 1");
      server.send(404, "text/plain", "FileNotFound");
    }
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, []() {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"analog\":" + String(analogRead(A0));
    json += ", \"gpio\":" + String((uint32_t)(0));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });
  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");


  delay(500);
  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  struct tm tmstruct ;
  delay(2000);
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);
  Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct.tm_year) + 1900, ( tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);
  Serial.println("");

 temp1.defiSP(1000);
 tempImpr.defiSP(1000);
}

int setaAuto = 0;


void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks


  struct tm tmstruct ;

// if (temp1.Saida(setaAuto))
  if (temp1.Saida(1))
  {

    tmstruct.tm_year = 0;
    getLocalTime(&tmstruct, 5000);

    String gravar = String((tmstruct.tm_year) + 1900) + "-" + String(( tmstruct.tm_mon) + 1) + "-" + String(tmstruct.tm_mday) + " " + String(tmstruct.tm_hour) + ":" + String(tmstruct.tm_min) + ":" + String(tmstruct.tm_sec);
    gravar += ",";
    gravar += analogRead(39);
    gravar += ",";
    gravar += analogRead(36);
    gravar += ",";
    gravar += analogRead(34);
    gravar += ",";
    gravar += analogRead(35);
    gravar += "\n";

   

salvaDados(gravar);




    temp1.Saida(0);
  }


if (tempImpr.Saida(1))
  {

Serial.print("Flash size: ");
Serial.print(ESP.getFlashChipSize());

Serial.print("Sketch size: ");
Serial.print(ESP.getSketchSize());
 
Serial.print(", SPIFFS size: ");
Serial.print(FILESYSTEM.totalBytes());
Serial.print(", SPIFFS used: ");
Serial.print(FILESYSTEM.usedBytes());

Serial.println();
 tempImpr.Saida(0);   
  }





debugSerial();


}
