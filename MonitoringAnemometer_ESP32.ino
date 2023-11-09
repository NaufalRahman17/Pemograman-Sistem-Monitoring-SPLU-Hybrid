#include <ThingerESP32.h>                                                        //Memasukkan Library Thinger IO untuk ESP8266
#include <WiFiClient.h>                                                            //Memasukkan Library WIFI Client
#include <ModbusMaster.h>

//===================================================================(Konfigurasi ESPWifi serta Thinger IO)=================================================================================//
 
 //konfigurasi Thinger.IO
  #define USERNAME "Noval_17"                                                        //Memasukkan Username Untuk Log in ke Platform Thinger IO
  #define DEVICE_ID "Monitoring_Anemometer"                                                //Memasukkan ID Untuk Log in ke Platform Thinger IO
  #define DEVICE_CREDENTIALS "WpLcRL0lsx0x7cqe"                                      //Memasukkan Kode Credentials yang ada di Thinger IO Untuk Log in ke Platform Thinger IO
  
  //Channel untuk WiFi
  const char *ssid    =  "TET";                                                  //Memasukkan User Wifi
  const char *pass    =  "riyadioce";                                           
  //Memasukkan Passwoard Wifi

  //Variabel untuk Thinger.IO
  ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIALS);                     //Memasukkan Variabel Untuk dikirim ke Thinger IO

//====================================================================(Konfigurasi Anemometer)===============================================================================================//

  # define windPin 4                                                                 //Memerdekaan pin sensor anemometer
  // Constants definitions
  const float pi = 3.14159265;                                                       // pi number
  int period = 1000;                                                                 // Measurement period (miliseconds)
  int delaytime = 1000;                                                              // Time between samples (miliseconds)
  int radio = 90;                                                                    // Distance from center windmill to outer cup (mm)
  int jml_celah = 18;                                                                // jumlah celah sensor

  // Variable definitions
  unsigned int Sample = 0;                                                           // Sample number
  unsigned int counter = 0;                                                          // B/W counter for sensor
  unsigned int RPM = 0;                                                              // Revolutions per minute
  float speedwind = 0;                                                               // Wind speed (m/s)

  void setup() {
  //Koneksi ke WIFI
  WiFi.begin(ssid,pass);                                                             //Fungsi Wifi Dimulai
  pinMode(windPin, INPUT);
  Serial.begin(9600);                                                               //Fungsi Serial Monitor Dimulai
 
  //hubungkan ESP8266 ke Thinger IO
  thing.add_wifi(ssid, pass);                                                        //Untuk Menghubungkan ESP8266 Dengan Thinger IO

  //Kirim Nilai Turbin Angin Ke Thinger IO
  thing["Anemometer"] >> [](pson & out){
  out["Kecepatan Angin"]   = speedwind;                                                    //Mengirim Data Tegangan Ke Thinger IO
  out["RPM"]               = RPM;                                                    //Mengirim Data Tegangan Ke Thinger IO
  out["Counter"]           = counter;
  };
  
  // Splash screen
  Serial.println("ANEMOMETER");
  Serial.println("n/**********");
  Serial.println("Based on depoinovasi anemometer sensor");
  Serial.print("Sampling period: ");
  Serial.print(period/1000);
  Serial.print(" seconds every ");
  Serial.print(delaytime/1000);
  Serial.println(" seconds.");
  Serial.println("** You could modify those values on code **");
  Serial.println();

}

void loop() {
  while(WiFi.status() != WL_CONNECTED)
  {
    //Apabila Sistem Tidak Terkoneksi, Maka LampBuzzer Menyala / ON
    Serial.println("Disconnected");
    delay(1000);
  }
    //Dan Apabila Sistem Terkoneksi, Maka LampBuzzer Mati / OFF                       
    Serial.println();
    Serial.println("Connected");
    delay(1000);
    
  //Memanggil Fungsi Anemometer
 Sample++;
  Serial.println();
  Serial.print(Sample);
  Serial.print(": Start measurement");
  windvelocity();
  Serial.println(" finished.");
  Serial.print("Counter: ");
  Serial.print(counter);
  Serial.println();
  Serial.print("RPM: ");
  RPMcalc();
  Serial.print(RPM);
  Serial.println();
  Serial.print("Wind speed: ");
  WindSpeed();
  Serial.print(speedwind);
  Serial.print(" m/s");
  Serial.println();
  
  if(speedwind < 3)
  {
    Serial.println("LAMBAT");                                                                //Menampilkan Tulisan Lambat
  };
  
 if((speedwind >= 3) && (speedwind <= 6))
  {
   Serial.println("SEDANG");                                                                //Menampilkan Tulisan Sedang
  }

  if(speedwind > 6)
  {
    Serial.println("CEPAT");                                                                //Menampilkan Tulisan Cepat
  }
  
  Serial.println("\n======================================================================================================================================================");

  thing.handle();
  
}

  // Measure wind speed
  void windvelocity()
  {
  speedwind = 0;
  counter = 0;
  attachInterrupt(digitalPinToInterrupt(windPin), addcount, RISING);
  unsigned long millis();
  long startTime = millis();
  while(millis() < startTime + period) {}

  detachInterrupt(1);
}

  void RPMcalc()
  {
  RPM=((counter/jml_celah)*60)/(period/1000); // Calculate revolutions per minute (RPM)
}

  void WindSpeed()
  {
  speedwind = ((4 * pi * radio * RPM)/60) / 1000; // Calculate wind speed on m/s
}

  void addcount()
  {
  counter++;
}
