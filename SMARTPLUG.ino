#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>

#define FIREBASE_HOST "********************"         //database connection must be written. Ex: oda-kontrol-default-rtdb.firebaseio.com
#define FIREBASE_AUTH "**********************"       //database token must be written. Ex: NJBwtdaıdsnaASjuyA4B00PnRZexgCTx2fgSUek
#define WIFI_SSID "*********"                        //wifi id must be written. Ex: NetMASTER Uydunet-1234
#define WIFI_PASSWORD "**********"                   //wifi password must be written. Ex: password12345

#define DHTPIN 4  
#define DHTTYPE DHT11
DHT dht (DHTPIN,DHTTYPE);


void setup() {
  Serial.begin(9600);
  pinMode(D7,OUTPUT);
  pinMode(2,OUTPUT);
  digitalWrite(D7,HIGH);                              //to start with the relay off. (optocoupler relay)
  dht.begin();
  basla:
  int hata = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);               //connection to the Internet
  Serial.print("Bağlanılıyor");
  while (WiFi.status() != WL_CONNECTED)  {
    Serial.print(".");
    hata=hata+1;
    delay(500);
    if(hata>40)  {                                    //for timeout
      Serial.println("RESET!!!");
      goto basla;
    }
  }
  Serial.println();
  Serial.println("Ağa Bağlanıldı.");
  Serial.print("Bağlantı: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);         //connection to database

  Serial.println("TAMAMI HAZIR.");
  
  for (int i=0;i<6;i++)  {                              //Flashing of the led lamp on the NodeMCU that everything is ok
  digitalWrite(2,LOW);
  delay(250);
  digitalWrite(2,HIGH);
  delay(250);
  }
}

void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Sicaklik: ");
  Serial.println(t);
  Serial.print("Nem: ");
  Serial.println(h);

  Firebase.setFloat("Nem", h);                            //write to database
  if (Firebase.failed())  {
      Serial.print("Database Yazım Hatası!!! :");
      Serial.println(Firebase.error());  
  }
  else  {
    Serial.println("Nem DataBase'e yazildi");              //info
  }

  delay(500);

  Firebase.setFloat("Sicaklik", t);                        //write to database
  if (Firebase.failed())  {
      Serial.print("Database Yazım Hatası!!! :");
      Serial.println(Firebase.error());  
  }
  else  {
    Serial.println("Sicaklik DataBase'e yazildi");         //info
  }
  
  delay(500);

  String prizdurum = Firebase.getString("Durum");           //read from database
  
  Serial.print("Priz Durum: ");
  Serial.println(prizdurum);
  if (prizdurum == "0")  {
    digitalWrite(D7,HIGH);                                  //turn off the relay (optocoupler relay)
  }
  if (prizdurum == "1")  {
    digitalWrite(D7,LOW);                                   //turn on the relay (optocoupler relay)
  }

  Serial.println("baglanti kontrolu ");                    //connection check
  if(WiFi.status() != WL_CONNECTED)  {
    Serial.println("Elektrik ya da internet kesintisi! Baglanti yenileniyor... ");
    restart();
  }
  Serial.println("baglanti kontrolu tamamlandi");
}



int restart(){                                              //connection restart function
  basla:
  int hata = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Bağlanılıyor");
  while (WiFi.status() != WL_CONNECTED)  {
    Serial.print(".");
    hata=hata+1;
    delay(500);
    if(hata>40)  { 
      Serial.println("RESET!!!");
      goto basla;
    }
  }
  Serial.println();
  Serial.println("Ağa Bağlanıldı.");
  Serial.print("Bağlantı: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Serial.println("HAZIR.");
  
  for (int i=0;i<6;i++)  {
  digitalWrite(2,LOW);
  delay(250);
  digitalWrite(2,HIGH);
  delay(250);
  }
}
