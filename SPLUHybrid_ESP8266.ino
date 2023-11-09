 //=======================================================================(MEMASUKKAN SEMUA LIBRARY )========================================================================================//
  
  #include <ThingerESP8266.h>                                                        //Memasukkan Library Thinger IO untuk ESP8266
  #include <ESP8266WiFi.h>                                                           //Memasukkan Library Wifi untuk ESP826
  #include <WiFiClient.h>                                                            //Memasukkan Library WIFI Client
  #include <LiquidCrystal_I2C.h>                                                     //Memasukkan Library LCD I2C
  #include <PZEM004Tv30.h>                                                           //Memasukkan Library PZEM004T
  #include <SoftwareSerial.h>                                                        //Memasukkan Library Serial untuk PZEM 017
  #include <ModbusMaster.h>                                                          //Load the (modified) library for modbus communication command codes. Kindly install at our website.
  #include "DHT.h"                                                                   //Memasukkan Library DHT22
  #include <Wire.h>                                                                  //Memasukkan Library Wire
  #include <BH1750.h>                                                                //Memasukkan Library BH1750
 
 //===================================================================(Konfigurasi ESPWifi serta Thinger IO)=================================================================================//
 
 //konfigurasi Thinger.IO
  #define USERNAME "Noval_17"                                                        //Memasukkan Username Untuk Log in ke Platform Thinger IO
  #define DEVICE_ID "Monitoring_SPLU"                                                //Memasukkan ID Untuk Log in ke Platform Thinger IO
  #define DEVICE_CREDENTIALS "fMZpwt+sR6uvNfj$"                                      //Memasukkan Kode Credentials yang ada di Thinger IO Untuk Log in ke Platform Thinger IO
  
  //Channel untuk WiFi
  const char *ssid    =  "TET";                                                      //Memasukkan User Wifi
  const char *pass    =  "riyadioce";                                                //Memasukkan Passwoard Wifi

  //Variabel untuk Thinger.IO
  ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIALS);                     //Memasukkan Variabel Untuk dikirim ke Thinger IO

//===========================================================================(Konfigurasi LCD I2C)===========================================================================================// 
  
  LiquidCrystal_I2C lcd1(0x25, 20, 4);                                               //Alamat LCD Untuk Memonitoring Daya Yang Dihasilkan Dari Turbin Angin, Temperature dan Kelembaban
  LiquidCrystal_I2C lcd2(0x27, 16, 2);                                               //Alamat LCD Untuk Memonitoring Daya Untuk Pengisian Sepeda Listrik
  LiquidCrystal_I2C lcd3(0x26, 16, 2);                                               //Alamat LCD Untuk Memonitoring Daya Yang Dihasilkan Dari Intensitas Cahaya

//==========================================================================(Konfigurasi PZEM 004T)==========================================================================================//  
  
  #define RXD2 13                                                                    //Memerdekakan pin PZEM004T (PIN D7 = GPIO 13 (RX2))
  #define TXD2 15                                                                    //Memerdekakan pin PZEM004T (Pin D8 = GPIO 15 (TX2))
  #if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
  #define PZEM_RX_PIN 13
  #define PZEM_TX_PIN 15
  #endif
  PZEM004Tv30 pzem(PZEM_RX_PIN, PZEM_TX_PIN); 
  
  float Power;
  float Energy;
  float Voltase;
  float Current;
  float Frequency;
  float PowerFactor;                                                                 //Variabel Penampung Nilai Hasil Pembacaan Dari Sensor PZEM004T

//==========================================================================(Konfigurasi PZEM 017)===========================================================================================//  
  
  SoftwareSerial PZEMSerial(14, 12);                                                 //Memerdekakan pin PZEM 017 (PIN D5 = GPIO 14 (RX/R0); PIN D6 = GPIO 12 (TX/DI))
  #define MAX485_DE  0                                                               // Define DE Pin to Arduino pin. Connect DE Pin of Max485 converter module to Pin D3 (GPIO 0) Node MCU board
  #define MAX485_RE  16                                                              // Define RE Pin to Arduino pin. Connect RE Pin of Max485 converter module to Pin D0 (GIPO 16) Node MCU board
  static uint8_t pzemSlaveAddr  = 0x01;                                              // Declare the address of device (meter 1) in term of 8 bits. 
  static uint8_t pzemSlaveAddr2 = 0x02;                                              // Declare the address of device (meter 2) in term of 8 bits. 
  static uint16_t NewshuntAddr  = 0x0001;                                            // Declare your external shunt value for DC Meter. Default 0x0000 is 100A, replace to "0x0001" if using 50A shunt, 0x0002 is for 200A, 0x0003 is for 300A
  static uint16_t NewshuntAddr2 = 0x0001;                                            // By default manufacturer may already set, however, to set manually kindly delete the "//" for line "// setShunt(0x01);" in void setup. 
  ModbusMaster node;                                                                 //* activate modbus master codes*/  
  float PZEMVoltage;                                                                 //* Variabel Penampung Nilai Tegangan Hasil Pembacaan Dari Sensor PZEM 017 (Panel Surya)*/
  float PZEMCurrent;                                                                 //* Variabel Penampung Nilai Arus Hasil Pembacaan Dari Sensor PZEM 017 (Panel Surya)*/
  float PZEMPower;                                                                   //* Variabel Penampung Nilai Daya Hasil Pembacaan Dari Sensor PZEM 017 (Panel Surya)*/
  float PZEMEnergy;                                                                  //* Variabel Penampung Nilai Energi Hasil Pembacaan Dari Sensor PZEM 017 (Panel Surya)*/
  ModbusMaster node2;
  float PZEMVoltage2;                                                                //* Variabel Penampung Nilai Tegangan Hasil Pembacaan Dari Sensor PZEM 017 (Pengisian Sepeda Listrik)*/
  float PZEMCurrent2;                                                                //* Variabel Penampung Nilai Arus Hasil Pembacaan Dari Sensor PZEM 017 (Sepeda Listrik)*/
  float PZEMPower2;                                                                  //* Variabel Penampung Nilai Daya Hasil Pembacaan Dari Sensor PZEM 017 (Sepeda Listrik)*/
  float PZEMEnergy2;                                                                 //* Variabel Penampung Nilai Energi Hasil Pembacaan Dari Sensor PZEM 017 (Sepeda Listrik)*/
       
  unsigned long startMillisPZEM;                                                     //* start counting time for LCD Display */
  unsigned long currentMillisPZEM;                                                   //* current counting time for LCD Display */
  unsigned long startMilliSetShunt; 
  const unsigned long periodPZEM = 500;                                              // refresh every X seconds (in seconds) in LED Display. Default 1000 = 1 second 

  unsigned long startMillisReadData;                                                 //* start counting time for data collection */
  unsigned long currentMillisReadData;                                               //* current counting time for data collection */
  const unsigned long periodReadData = 1000;                                         //* refresh every X seconds (in seconds) in LED Display. Default 1000 = 1 second */
  int ResetEnergy = 0;                                                               //* reset energy function */
  int ResetEnergy2 = 0;
  int a = 0;
  unsigned long startMillis1;                                                        // to count time during initial start up

//======================================================================(Konfigurasi DHT22)=================================================================================================//

  #define DHTPIN 3                                                                   //Memerdekakan pin DHT11 (Pin RX = GPIO 3)
  #define TAMPIL_SERIAL true
  #define DHTTYPE DHT22                                                              //Memerdekakan Jenis DHT Yang Digunakan

  DHT dht(DHTPIN, DHTTYPE);

  const byte KARAKTER_DERAJAT = 0;
  byte derajat[] = {
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

  float hum;                                                                         //Variabel Penampung Nilai Hasil Pembacaan Dari Sensor DHT11
  float temp;
  
//=======================================================================(Konfigurasi BH1750)===================================================================================================//
  
  //Konfigurasi untuk Sensor BH1750
  BH1750 lightMeter;                                                                 //Mengaktifkan Library Sensor BH1750
  float Lux;                                                                         //Variabel Penampung Nilai Hasil Pembacaan Dari Sensor BH1750

//====================================================================(Konfigurasi LampBuzzer)===============================================================================================//
  
  //Konfigurasi untuk LampBuzzer
  #define LampBuzzer 2                                                               //Memerdekakan pin LampBuzzer (Pin TX = GPIO 1)
 
//=======================================================================(Konfigurasi Kecepatan Angin)===================================================================================================//
  
  //Konfigurasi untuk Sensor Kecepatan Angin
  //float Kec = 0;                                                                  //Variabel Penampung Nilai Hasil Pembacaan Dari Sensor Kecepatan Angin
  //float RPM = 0;                                                                  //Variabel Penampung Nilai Hasil Pembacaan Dari Sensor Kecepatan Angin
  //float Con = 0;                                                                  //Variabel Penampung Nilai Hasil Pembacaan Dari Sensor Kecepatan Angin

//######################################################################(MASUK KE VOID SETUP)################################################################################################//
  void setup() {

  //Koneksi ke WIFI
  WiFi.begin(ssid,pass);                                                            //Fungsi Wifi Dimulai
    
  //hubungkan ESP8266 ke Thinger IO
  thing.add_wifi(ssid, pass);                                                       //Untuk Menghubungkan ESP8266 Dengan Thinger IO

  // Aktifkan Serial
  Serial.begin(9600);                                                               //Fungsi Serial Monitor Dimulai
  
  // Konfigurasi LCD
  lcd1.begin();                                                                     //Fungsi LCD 1 (Turbin Angin, Temperature dan Kelembaba) Dimulai
  lcd2.begin();                                                                     //Fungsi LCD 2 (Pengisian Sepeda Listrik) Dimulai
  lcd3.begin();                                                                     //Fungsi LCD 2 (Intensitas Cahaya) Dimulai

  //lcd1.backlight();                                                               //Mengatur Backlight Pada LCD 1 Secara Otomatis
  //lcd2.backlight();                                                               //Mengatur Backlight Pada LCD 2 Secara Otomatis 
  
  //Kirim Nilai Turbin Angin Ke Thinger IO
  thing["TurbinAngin"] >> [](pson & out){
  out["Volt"]   = Voltase;                                                          //Mengirim Data Tegangan Ke Thinger IO
  out["Ampere"] = Current;                                                          //Mengirim Data Arus Ke Thinger IO
  out["Watt"]   = Power;                                                            //Mengirim Data Daya Ke Thinger IO
  out["kWh"]    = Energy;                                                           //Mengirim Data Energi Ke Thinger IO
  out["Hz"]     = Frequency;                                                        //Mengirim Data Frekuensi Ke Thinger IO
  out["PF"]     = PowerFactor;                                                      //Mengirim Data Faktor Daya Ke Thinger IO
  };

  //Kirim Nilai Panel Surya Ke Thinger IO                                           
  thing["PanelSurya"] >> [](pson & out){
  out["Volt"]   = PZEMVoltage;                                                      //Mengirim Data Tegangan Ke Thinger IO
  out["Ampere"] = PZEMCurrent;                                                      //Mengirim Data Arus Ke Thinger IO
  out["Watt"]   = PZEMPower;                                                        //Mengirim Data Daya Ke Thinger IO
  out["Wh"]     = PZEMEnergy;                                                       //Mengirim Data Energi Ke Thinger IO
  out["LuxMeter"]  = Lux;                                                           //Mengirim Data Intensitas Cahaya Ke Thinger IO
  out["Temperature"]  = temp;                                                       //Mengirim Data Temperature Ke Thinger IO
  out["Kelembaban"]   = hum;                                                        //Mengirim Data Kelembaban Ke Thinger IO
  };

  //Kirim Nilai Pengisian Sepeda Listrik Ke Thinger IO
  thing["Output"] >> [](pson & out){
  out["Volt"]   = PZEMVoltage2;                                                     //Mengirim Data Tegangan Ke Thinger IO
  out["Ampere"] = PZEMCurrent2;                                                     //Mengirim Data Arus Ke Thinger IO
  out["Watt"]   = PZEMPower2;                                                       //Mengirim Data Daya Ke Thinger IO
  out["Wh"]     = PZEMEnergy2;                                                      //Mengirim Data Energi Ke Thinger IO
  };

  //Terima Nilai Kecepatan Angin Dari Thinger IO
  //thing["Anemometer"] << [](pson& in){
  //Kec = in["Kecepatan Angin"];
  //RPM = in["RPM"];
  //Con = in["Counter"];
//};

  //Konfigurasi PZEM 017//
  Serial.println(F("PZEM 017 Test begin"));
  PZEMSerial.begin(9600,SWSERIAL_8N2,14,12);                                        // 14 = Rx/R0/ GPIO 14 (D2) & 12 = Tx/DI/ GPIO 12 (D3) on NodeMCU     
  startMilliSetShunt = millis();
        
  //* 1- PZEM-017 DC Energy Meter *//
        
  startMillisPZEM = millis();                                                       /* Start counting time for run code */
  pinMode(MAX485_RE, OUTPUT);                                                       /* Define RE Pin as Signal Output for RS485 converter. Output pin means Arduino command the pin signal to go high or low so that signal is received by the converter*/
  pinMode(MAX485_DE, OUTPUT);                                                       /* Define DE Pin as Signal Output for RS485 converter. Output pin means Arduino command the pin signal to go high or low so that signal is received by the converter*/
  digitalWrite(MAX485_RE, 0);                                                       /* Arduino create output signal for pin RE as LOW (no output)*/
  digitalWrite(MAX485_DE, 0);                                                       /* Arduino create output signal for pin DE as LOW (no output)*/
                                                                                    // both pins no output means the converter is in communication signal receiving mode
  node.preTransmission(preTransmission);                                            // Callbacks allow us to configure the RS485 transceiver correctly
  node.postTransmission(postTransmission);
  node2.preTransmission(preTransmission);                                           // Callbacks allow us to configure the RS485 transceiver correctly
  node2.postTransmission(postTransmission);                   
  delay(1000);                                                                      /* after everything done, wait for 1 second */
  startMillisReadData = millis();

  //Konfigurasi PZEM004T//
  Serial.println(F("PZEM004T Test begin"));
  
  //Konfigurasi DHT22//
  pinMode(3, FUNCTION_3);                                                           //Menggunakan Fungsi Untuk Mengubah Pin RX Menjadi Pin Digital Agar Dapat Membaca Data Dari Sensor
  pinMode(DHTPIN, INPUT);                                                           //Memerdekakan Pin DHT Sebaga Input
  #if TAMPIL_SERIAL
  Serial.println(F("DHT 22 Test begin"));
  #endif
  dht.begin();                                                                      //Fungsi DHT11 Dimulai

  //Konfigurasi BH1750//
  Wire.begin();                                                                     // Aktifkan Wire
  lightMeter.begin();                                                               //Fungsi BH1750 Dimulai
  Serial.println(F("BH1750 Test begin"));

  //Konfigurasi LampBuzzer//
  pinMode(LampBuzzer, OUTPUT);                                                      //Memerdekakan Pin LampBuzzer Sebagai Output
}


//############################################################################(MASUK KE VOID SETUP)##########################################################################################//
  void loop(){
    
  //Pengecekan koneksi wifi
  while(WiFi.status() != WL_CONNECTED)
  {
    //Apabila Sistem Tidak Terkoneksi, Maka LampBuzzer Menyala / ON
    digitalWrite(LampBuzzer, HIGH);
    delay(500);
    Serial.println("Disconnected");
    lcd1.setCursor (4,0);                                                            //Set Kolom Dan Baris
    lcd1.print("Disconnected");                                                      //Menampilkan Status Connection

  }
    //Dan Apabila Sistem Terkoneksi, Maka LampBuzzer Mati / OFF                       
    digitalWrite(LampBuzzer, LOW);
    Serial.println();
    Serial.println("Connected");
    lcd1.setCursor (0,3);                                                            //Set Kolom Dan Baris
    lcd1.print("Connected");
      
  //Memasukkan Fungsi Wild Turbin
  Pzem004t();
 
  //Memasukkan Fungsi Panel Surya dan Pengisian Sepeda Listrik
  Pzem017();
  
  //Memasukkan Fungsi DHT11
  Dht22();

  //Memasukkan Fungsi Bh1750
  Bh1750();
  
  //Memasukkan Fungsi LCD
  Lcd();

  //menghubungkan Thinger IO Ke ESP8266
  thing.handle();
}

//============================================================================(Fungsi PZEM004T)==============================================================================================//
  void Pzem004t(){
  //Tampil di Serial dan Pengukuran
  Serial.println("\n================");
  Serial.println();
  //Baca Nilai Power (W)//
  Power = pzem.power();
  //Jika gagal Membaca Power
  if(isnan(Power))
  {
    Serial.println("Gagal Membaca Power");
  }
  else
  {
    Serial.print("Power : ");
    Serial.print(Power);
    Serial.println("W");
  }

  //Baca Nilai Energy (Wh)//
  Energy = pzem.energy();
  //Jika gagal Membaca Power
  if(isnan(Energy))
  {
    Serial.println("Gagal Membaca Energy");
  }
  else
  {
    Serial.print("Energy : ");
    Serial.print(Energy);
    Serial.println("Wh");
  }

  //Baca Nilai Voltase (Volt)//
  Voltase = pzem.voltage();
  //Jika gagal Membaca Power
  if(isnan(Voltase))
  {
    Serial.println("Gagal Membaca Voltase");
  }
  else
  {
    Serial.print("Voltase : ");
    Serial.print(Voltase);
    Serial.println("V");
  }

  //Baca Nilai Current (A)//
  Current = pzem.current();
  //Jika gagal Membaca Power
  if(isnan(Current))
  {
    Serial.println("Gagal Membaca Current");
  }
  else
  {
    Serial.print("Current : ");
    Serial.print(Current);
    Serial.println("A");
  }

  //Baca Nilai Frequency (Hz)//
  Frequency = pzem.frequency();
  //Jika gagal Membaca Power
  if(isnan(Frequency))
  {
    Serial.println("Gagal Membaca Frequency");
  }
  else
  {
    Serial.print("Frequency : ");
    Serial.print(Frequency);
    Serial.println("Hz");
  }

  //Baca Nilai PowerFactor//
  PowerFactor = pzem.pf();
  //Jika gagal Membaca Power
  if(isnan(PowerFactor))
  {
    Serial.println("Gagal Membaca PowerFactor");
  }
  else
  {
    Serial.print("PowerFactor : ");
    Serial.print(PowerFactor);
    Serial.println();
  }
  
  Serial.println();
  delay(1000);
}

//================================================================================(FUNGSI PZEM017)==========================================================================================//
  void Pzem017(){
  currentMillisPZEM = millis(); 

   /* 3- Set Shunt */
   if (millis()- startMilliSetShunt == 10000) 
   {setShunt(0x01);}
   if (millis()-startMilliSetShunt == 15000)                          
   {setShunt2(0x02);}
        
   if(a == 0)
    {
      node.begin(pzemSlaveAddr, PZEMSerial);                                                              /* Define and start the Modbus RTU communication. Communication to specific slave address and which Serial port */
      if (currentMillisPZEM - startMillisPZEM >= periodPZEM)                                              /* for every x seconds, run the codes below*/
        {
          uint8_t result;                                                                                 /* Declare variable "result" as 8 bits */   
          result = node.readInputRegisters(0x0000, 6);                                                    /* read the 9 registers (information) of the PZEM-014 / 016 starting 0x0000 (voltage information) kindly refer to manual)*/
          if (result == node.ku8MBSuccess)                                                                /* If there is a response */
            {
              uint32_t tempdouble = 0x00000000;                                                           /* Declare variable "tempdouble" as 32 bits with initial value is 0 */ 
              PZEMVoltage = node.getResponseBuffer(0x0000) / 100.0;                                       /* get the 16bit value for the voltage value, divide it by 100 (as per manual)- 0x0000 to 0x0008 are the register address of the measurement value*/
              PZEMCurrent = node.getResponseBuffer(0x0001) / 100.0;                                       /* get the 16bit value for the current value, divide it by 100 (as per manual) */
              tempdouble =  (node.getResponseBuffer(0x0003) << 16) + node.getResponseBuffer(0x0002);      /* get the power value. Power value is consists of 2 parts (2 digits of 16 bits in front and 2 digits of 16 bits at the back) and combine them to an unsigned 32bit */
              PZEMPower = tempdouble / 10.0;                                                              /* Divide the value by 10 to get actual power value (as per manual) */
              tempdouble =  (node.getResponseBuffer(0x0005) << 16) + node.getResponseBuffer(0x0004);      /* get the energy value. Energy value is consists of 2 parts (2 digits of 16 bits in front and 2 digits of 16 bits at the back) and combine them to an unsigned 32bit */
              PZEMEnergy = tempdouble;                                                                    
            }
          if (pzemSlaveAddr==5)                                                                           /* just for checking purpose to see whether can read modbus*/
              {}
          else
              {}
              a=1;
              startMillisPZEM = currentMillisPZEM ; 
          }  
        }
        
   if(a == 1)
    {
      node2.begin(pzemSlaveAddr2, PZEMSerial);
      if (currentMillisPZEM - startMillisPZEM >= periodPZEM)                                              /* for every x seconds, run the codes below*/
        {
          uint8_t result2;                                                                                /* Declare variable "result" as 8 bits */   
          result2 = node2.readInputRegisters(0x0000, 6);                                                  /* read the 9 registers (information) of the PZEM-014 / 016 starting 0x0000 (voltage information) kindly refer to manual)*/
          if (result2 == node2.ku8MBSuccess)                                                              /* If there is a response */
            {
              uint32_t tempdouble2 = 0x00000000;                                                          /* Declare variable "tempdouble" as 32 bits with initial value is 0 */ 
              PZEMVoltage2 = node2.getResponseBuffer(0x0000) / 100.0;                                     /* get the 16bit value for the voltage value, divide it by 100 (as per manual)- 0x0000 to 0x0008 are the register address of the measurement value*/
              PZEMCurrent2 = node2.getResponseBuffer(0x0001) / 100.0;                                     /* get the 16bit value for the current value, divide it by 100 (as per manual) */
              tempdouble2 =  (node2.getResponseBuffer(0x0003) << 16) + node2.getResponseBuffer(0x0002);   /* get the power value. Power value is consists of 2 parts (2 digits of 16 bits in front and 2 digits of 16 bits at the back) and combine them to an unsigned 32bit */
              PZEMPower2 = tempdouble2 / 10.0;                                                            /* Divide the value by 10 to get actual power value (as per manual) */
              tempdouble2 =  (node2.getResponseBuffer(0x0005) << 16) + node2.getResponseBuffer(0x0004);   /* get the energy value. Energy value is consists of 2 parts (2 digits of 16 bits in front and 2 digits of 16 bits at the back) and combine them to an unsigned 32bit */
              PZEMEnergy2 = tempdouble2;                                                                    
            }
              if (pzemSlaveAddr==5)                                                                       /* just for checking purpose to see whether can read modbus*/
              {}
              else
              {}
              a=0;
              startMillisPZEM = currentMillisPZEM ;        
            }
        }
                                                                                                          /* count time for program run every second (by default)*/
        
   currentMillisReadData = millis();                                                                      /* Set counting time for data submission to server*/
   if (currentMillisReadData - startMillisReadData >= periodReadData)                                     /* for every x seconds, run the codes below*/  
     {
       Serial.println("\n================");
       Serial.println();
       Serial.println("Energy Meter 1 : ");
       Serial.print("Vdc : "); Serial.print(PZEMVoltage); Serial.println(" V ");
       Serial.print("Idc : "); Serial.print(PZEMCurrent); Serial.println(" A ");
       Serial.print("Power : "); Serial.print(PZEMPower); Serial.println(" W ");
       Serial.print("Energy : "); Serial.print(PZEMEnergy); Serial.println(" Wh ");
       Serial.println();
           
       Serial.println("Energy Meter 2 : ");
       Serial.print("Vdc : "); Serial.print(PZEMVoltage2); Serial.println(" V ");
       Serial.print("Idc : "); Serial.print(PZEMCurrent2); Serial.println(" A ");
       Serial.print("Power : "); Serial.print(PZEMPower2); Serial.println(" W ");
       Serial.print("Energy : "); Serial.print(PZEMEnergy2); Serial.println(" Wh ");
       startMillisReadData = millis();
       }
       delay(1000);
}

  void preTransmission()                                                                                    /* transmission program when triggered*/
  {
        /* 1- PZEM-017 DC Energy Meter */
        
        if(millis() - startMillis1 > 5000)                                                                // Wait for 5 seconds as ESP Serial cause start up code crash
        {
          digitalWrite(MAX485_RE, 1);                                                                     /* put RE Pin to high*/
          digitalWrite(MAX485_DE, 1);                                                                     /* put DE Pin to high*/
          delay(1);                                                                                       // When both RE and DE Pin are high, converter is allow to transmit communication
        }
  }

  void postTransmission()                                                                                   /* Reception program when triggered*/
  {
        
        /* 1- PZEM-017 DC Energy Meter */
  
        if(millis() - startMillis1 > 5000)                                                                // Wait for 5 seconds as ESP Serial cause start up code crash
        {
          delay(3);                                                                                       // When both RE and DE Pin are low, converter is allow to receive communication
          digitalWrite(MAX485_RE, 0);                                                                     /* put RE Pin to low*/
          digitalWrite(MAX485_DE, 0);                                                                     /* put DE Pin to low*/
        }
  }

  void setShunt(uint8_t slaveAddr)                                                                          //Change the slave address of a node
  {

        /* 1- PZEM-017 DC Energy Meter */
        
        static uint8_t SlaveParameter = 0x06;                                                             /* Write command code to PZEM */
        static uint16_t registerAddress = 0x0003;                                                         /* change shunt register address command code */
        
        uint16_t u16CRC = 0xFFFF;                                                                         /* declare CRC check 16 bits*/
        u16CRC = crc16_update(u16CRC, slaveAddr);                                                         // Calculate the crc16 over the 6bytes to be send
        u16CRC = crc16_update(u16CRC, SlaveParameter);
        u16CRC = crc16_update(u16CRC, highByte(registerAddress));
        u16CRC = crc16_update(u16CRC, lowByte(registerAddress));
        u16CRC = crc16_update(u16CRC, highByte(NewshuntAddr));
        u16CRC = crc16_update(u16CRC, lowByte(NewshuntAddr));
      
        preTransmission();                                                                                /* trigger transmission mode*/
      
        PZEMSerial.write(slaveAddr);                                                                      /* these whole process code sequence refer to manual*/
        PZEMSerial.write(SlaveParameter);
        PZEMSerial.write(highByte(registerAddress));
        PZEMSerial.write(lowByte(registerAddress));
        PZEMSerial.write(highByte(NewshuntAddr));
        PZEMSerial.write(lowByte(NewshuntAddr));
        PZEMSerial.write(lowByte(u16CRC));
        PZEMSerial.write(highByte(u16CRC));
        delay(10);
        postTransmission();                                                                               /* trigger reception mode*/
        delay(100);
        while (PZEMSerial.available())                                                                    /* while receiving signal from Serial3 from meter and converter */
          {  
            Serial.print(char(PZEMSerial.read()), HEX);                                                   //Prints the response and display on Serial Monitor (Serial)
            Serial.print(" "); 
          }
  }

  void setShunt2(uint8_t slaveAddr2)                                                                        //Change the slave address of a node
  {

        /* 1- PZEM-017 DC Energy Meter */
        
        static uint8_t SlaveParameter2 = 0x06;                                                            /* Write command code to PZEM */
        static uint16_t registerAddress2 = 0x0003;                                                        /* change shunt register address command code */
        
        uint16_t u16CRC2 = 0xFFFF;                                                                        /* declare CRC check 16 bits*/
        u16CRC2 = crc16_update(u16CRC2, slaveAddr2);                                                      // Calculate the crc16 over the 6bytes to be send
        u16CRC2 = crc16_update(u16CRC2, SlaveParameter2);
        u16CRC2 = crc16_update(u16CRC2, highByte(registerAddress2));
        u16CRC2 = crc16_update(u16CRC2, lowByte(registerAddress2));
        u16CRC2 = crc16_update(u16CRC2, highByte(NewshuntAddr2));
        u16CRC2 = crc16_update(u16CRC2, lowByte(NewshuntAddr2));
      
        preTransmission();                                                                                /* trigger transmission mode*/
      
        PZEMSerial.write(slaveAddr2);                                                                     /* these whole process code sequence refer to manual*/
        PZEMSerial.write(SlaveParameter2);
        PZEMSerial.write(highByte(registerAddress2));
        PZEMSerial.write(lowByte(registerAddress2));
        PZEMSerial.write(highByte(NewshuntAddr2));
        PZEMSerial.write(lowByte(NewshuntAddr2));
        PZEMSerial.write(lowByte(u16CRC2));
        PZEMSerial.write(highByte(u16CRC2));
        delay(10);
        postTransmission();                                                                               /* trigger reception mode*/
        delay(100);
        while (PZEMSerial.available())                                                                    /* while receiving signal from Serial3 from meter and converter */
          {   
            Serial.print(char(PZEMSerial.read()), HEX);                                                   //Prints the response and display on Serial Monitor (Serial)
            Serial.print(" ");
          }
  }

//=================================================================================(FUNGSI DHT22)============================================================================================//
  void Dht22(){
  hum = dht.readHumidity();
  temp = dht.readTemperature(); // Baca temperatur dalam celcius (default)

  Serial.println("\n================");
  Serial.println();
  if (isnan(hum) || isnan(temp)) {
  #if TAMPIL_SERIAL
  Serial.println(F("Failed to read from DHT sensor!"));
  #endif
  return;

  #if TAMPIL_SERIAL
  Serial.print(F("Humidity: ")); Serial.print(hum); Serial.println("%");
  Serial.print(F("Temperature: ")); Serial.print(temp); Serial.println(F("°C "));
  delay(1000);
  #endif
  }
}

//=================================================================================(FUNGSI Bh1750)============================================================================================//

  void Bh1750(){
  Lux = lightMeter.readLightLevel();                                            //Variabel Penampung Nilai Hasil Pembacaan Dari Sensor BH1750
  Serial.println("\n================");
  Serial.println();
  Serial.print("LUX METER: ");
  Serial.print(Lux);
  Serial.println(" Lx");
  delay(1000);
  
  //Keterangan Kondisi Cahaya 
  if(Lux < 200)
  {
    Serial.print("REDUP");
  }

  if((Lux >= 200) && (Lux <= 500))
  {
    Serial.print("SEDANG");
  }

  if(Lux > 500)
  {
    Serial.print("TERANG");
  }
  
  Serial.println();
  Serial.println("\n================");

//Monitoring Daya Hasil Pengukuran BH1750
  lcd3.clear();                                                                        //Membersihkan LCD                                                         
  lcd3.setCursor(3,0);                                                                //Set Kolom Dan Baris
  lcd3.print("LUX  METER");                                                           //Menampilkan Data Hasil Pengukuran Intensitas Cahaya di LCD
  lcd3.setCursor(0,1);                                                                //Set Kolom Dan Baris
  lcd3.print(Lux);                                                                    //Menampilkan Data Hasil Pengukuran Intensitas Cahaya di LCD

  if(Lux < 200)
  {
    lcd3.setCursor(9,1);
    lcd3.print("REDUP");
  }

  if((Lux >= 200) && (Lux <= 500))
  {
    lcd3.setCursor(9,1);
    lcd3.print("SEDANG");
  }

  if(Lux > 500)
  {
    lcd3.setCursor(9,1);
    lcd3.print("TERANG");
  }
}

//=================================================================================(FUNGSI LCD)============================================================================================//

  void Lcd(){
  //###Tampil Data di LCD###//
  //___Monitoring Daya Hasil Pengukuran Turbin Angin, Panel Surya Temperature, Kelembaban, dan Intensitas Cahaya___//
  lcd1.clear();                                                                        //Membersihkan LCD                                                         
  lcd2.clear();                                                                        //Membersihkan LCD                                                         

  //Monitoring Daya Hasil Pengukuran Turbin Angin
  lcd1.setCursor (0,0);                                                                //Set Kolom Dan Baris
  lcd1.print(Voltase, 1);                                                              //Menampilkan Data Hasil Pengukuran Tegangan Turbin Angin di LCD 
  lcd1.setCursor (5,0);                                                                //Set Kolom Dan Baris
  lcd1.print("V");                                                                     //Menampilkan Satuan Tegangan

  lcd1.setCursor (7,0);                                                               //Set Kolom Dan Baris
  lcd1.print(Current, 1);                                                              //Menampilkan Data Hasil Pengukuran Arus Turbin Angin di LCD
  lcd1.setCursor (11,0);                                                               //Set Kolom Dan Baris
  lcd1.print("A");                                                                     //Menampilkan Satuan Arus
  
  lcd1.setCursor (13,0);                                                                //Set Kolom Dan Baris
  lcd1.print(Power, 1);                                                                //Menampilkan Data Hasil Pengukuran Daya Turbin Angin di LCD
  lcd1.setCursor (18,0);                                                                //Set Kolom Dan Baris
  lcd1.print("W");                                                                     //Menampilkan Satuan Daya

  //Monitoring Daya Hasil Pengukuran Panel Surya
  lcd1.setCursor (0,1);                                                                //Set Kolom Dan Baris
  lcd1.print(PZEMVoltage, 1);                                                          //Menampilkan Data Hasil Pengukuran Tegangan di LCD
  lcd1.setCursor (5,1);                                                                //Set Kolom Dan Baris
  lcd1.print("V");                                                                     //Menampilkan Satuan Tegangan
  
  lcd1.setCursor (7,1);                                                                //Set Kolom Dan Baris
  lcd1.print(PZEMCurrent, 1);                                                          //Menampilkan Data Hasil Pengukuran Arus di LCD
  lcd1.setCursor (11,1);                                                               //Set Kolom Dan Baris
  lcd1.print("A");                                                                     //Menampilkan Satuan Arus

  lcd1.setCursor (13,1);                                                                //Set Kolom Dan Baris
  lcd1.print(PZEMPower, 1);                                                            //Menampilkan Data Hasil Pengukuran Daya di LCD
  lcd1.setCursor (18,1);                                                                //Set Kolom Dan Baris
  lcd1.print("W");                                                                     //Menampilkan Satuan Daya

  //Monitoring Daya Hasil Pengukuran DHT22
  lcd1.setCursor (0,2);                                                                //Set Kolom Dan Baris
  lcd1.print(temp, 2);                                                                 //Menampilkan Data Hasil Pengukuran Temperature di LCD
  lcd1.setCursor (5,2);                                                                //Set Kolom Dan Baris
  lcd1.print("C");                                                                     //Menampilkan Satuan Temperature

  lcd1.setCursor (14,2);                                                                //Set Kolom Dan Baris
  lcd1.print(hum, 2);                                                                  //Menampilkan Data Hasil Pengukuran Kelembaban di LCD
  lcd1.setCursor (19,2);                                                               //Set Kolom Dan Baris
  lcd1.print("%");                                                                     //Menampilkan Satuan Kelembaban

  if(temp < 25)
  {
    lcd1.setCursor(14,3);
    lcd1.print("COOL");
  }

  if((temp >= 25) && (temp <= 30 ))
  {
    lcd1.setCursor(14,3);
    lcd1.print("WARM");
  }

  if(temp > 30)
  {
    lcd1.setCursor(14,3);
    lcd1.print("HOT");
  }
  
  //___Monitoring Daya Hasil Pengukuran Pengisian Sepeda Listrik___//
  lcd2.setCursor(0,0);                                                                //Set Kolom Dan Baris
  lcd2.print(PZEMVoltage2, 2);                                                         //Menampilkan Data Hasil Pengukuran Tegangan di LCD
  lcd2.setCursor(6,0);                                                                //Set Kolom Dan Baris
  lcd2.print("V");                                                                     //Menampilkan Satuan Tegangan
  
  lcd2.setCursor(8,0);                                                                //Set Kolom Dan Baris
  lcd2.print(PZEMCurrent2, 2);                                                         //Menampilkan Data Hasil Pengukuran Arus di LCD
  lcd2.setCursor (12,0);                                                               //Set Kolom Dan Baris
  lcd2.print("A");                                                                     //Menampilkan Satuan Arus

  lcd2.setCursor(0,1);                                                                //Set Kolom Dan Baris
  lcd2.print(PZEMPower2, 2);                                                           //Menampilkan Data Hasil Pengukuran Daya di LCD
  lcd2.setCursor(6,1);                                                                //Set Kolom Dan Baris
  lcd2.print("W");                                                                     //Menampilkan Satuan Daya
  
  lcd2.setCursor(8,1);                                                                //Set Kolom Dan Baris
  lcd2.print(PZEMEnergy2, 2);                                                          //Menampilkan Data Hasil Pengukuran Energi di LCD
  lcd2.setCursor(12,1);                                                               //Set Kolom Dan Baris
  lcd2.print("Wh");                                                                    //Menampilkan Satuan Energi

  delay(1000);
}
