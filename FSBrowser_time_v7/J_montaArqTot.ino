void montaTotal()
{
   Serial.println("Monta o ARQUIVO total...");

// copiar o dados001.txt no total.txt

//readFile(SPIFFS, "/dados001.txt");

readWriteFile(SPIFFS, "/dados001.txt", "/total.txt", "w");

for (int i = 2; i <= quantArqMax; i++)
{
  String caminho = criaCaminho(i);
  readWriteFile(SPIFFS, caminho.c_str(), "/total.txt", "a");
}

//readWriteFile(SPIFFS, "/dados002.txt", "/total.txt", "a");
//readWriteFile(SPIFFS, "/dados003.txt", "/total.txt", "a");
//readWriteFile(SPIFFS, "/dados004.txt", "/total.txt", "a");
//readWriteFile(SPIFFS, "/dados005.txt", "/total.txt", "a");
   
}
