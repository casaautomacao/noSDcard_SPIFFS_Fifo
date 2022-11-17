

void salvaDados(String dadosSalvar) {


  const char* cstr = dadosSalvar.c_str();

  String caminho = criaCaminho(quantArqDados);

 // Serial.println(caminho);

  File file = FILESYSTEM.open(caminho);

  long tamArquivo = file.size();

  //long tamArquivo;
  file.close();







// Serial.print("*** PASSO 0 - ");
// Serial.print(somaTamanhos);
//   Serial.print(" - ");
//  Serial.print(quantArqDados);
 //  Serial.print(" - ");
 //   Serial.print(tamArquivo);

// Serial.println();
 



  if (tamArquivo > tamArquivoMax)
  {





    if (quantArqDados >= quantArqMax)
    {
 //     Serial.println("PASSO 1");
      String path1 = "/dados001.txt";
      File file2 = FILESYSTEM.open(path1);

      long tamTrab = file2.size();
      somaTamanhos = somaTamanhos - tamTrab + tamArquivo;
    //  file2.close();




      deleteFile(SPIFFS, path1.c_str() );




      //renomeia todos os arquivos
      for (int i = 1; i < quantArqDados; i++)
      {
        //renomeia todos os arquivos i para i+1
        String caminho1 = criaCaminho(i);
        String caminho2 = criaCaminho(i + 1);

      

        renameFile(SPIFFS, caminho2.c_str(), caminho1.c_str());


      }


    }


    else
    {
  //    Serial.println("PASSO 2");
      somaTamanhos = somaTamanhos + tamArquivo;
      quantArqDados++;
    }


    String caminho = criaCaminho(quantArqDados);

 //   Serial.println(caminho);

    // criar o novo arquivo

    writeFile(SPIFFS, caminho.c_str(), dadosSalvar.c_str());



  }

  else
  {

  //  Serial.println("PASSO 3");
    appendFile(SPIFFS, caminho.c_str(), dadosSalvar.c_str());

  }
}




















String criaCaminho(int quantArq)
{

  String numeroS = String(quantArq);
  int qtChar = numeroS.length();

  if (qtChar == 1) numeroS = "00" + numeroS;
  else if (qtChar == 2) numeroS = "0" + numeroS;

  return "/dados" + numeroS + ".txt";

}















void debugSerial()
{

  if (Serial.available() > 0)
  {

    int comando = Serial.parseInt();

    if (comando == 1 ) listDir(SPIFFS, "/", 0);

    if (comando == 2 )  // 2/dados001.txt
    {
      String caminho = Serial.readString();

      Serial.println(caminho);

      writeFile(SPIFFS, caminho.c_str(), "oi");

    }


    if (comando == 3 )  // 3/dados001.txt
    {
      String caminho = Serial.readString();

      Serial.println(caminho);

      deleteFile(SPIFFS, caminho.c_str());



    }


    if (comando == 4 )  // 4/dados002.txt
    {
      String caminho = Serial.readString();

      Serial.println(caminho);

      renameFile(SPIFFS, caminho.c_str(), "/dados001.txt");



    }

    if (comando == 5 )  
    {

      setaAuto = 1 - setaAuto;


    }




  }


}






void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}




void writeFile(fs::FS &fs, const char * path, const char * message) {
 // Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
//    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}




void renameFile(fs::FS &fs, const char * path1, const char * path2) {
 // Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (fs.rename(path1, path2)) {
  //  Serial.println("- file renamed");
  } else {
    Serial.println("- rename failed");
  }
}




void deleteFile(fs::FS &fs, const char * path) {
 // Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
  //  Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}


void appendFile(fs::FS &fs, const char * path, const char * message) {
//  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message)) {
 //   Serial.println("- message appended");
  } else {
    Serial.println("- append failed");
  }
}



void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  
  while (file.available()) {
    Serial.write(file.read());
  }
}


void readWriteFile(fs::FS &fs, const char * path_O, const char * path_D, const char * tipo) {
  Serial.printf("Reading file: %s\r\n", path_O);
 Serial.printf("writting file: %s\r\n", path_D);
  File file_O = fs.open(path_O);
File file_D = fs.open(path_D, tipo);

  
  if (!file_O || file_O.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

   if (!file_D || file_D.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  
  while (file_O.available()) {
    byte local = file_O.read();
  //  Serial.print(char(local));
    file_D.write(local);
  }
}
