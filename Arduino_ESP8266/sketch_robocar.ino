
#include <ESP8266WiFi.h>           // Библиотека WiFi связь 
#include <WiFiUdp.h>               // Udp-Сокет 


unsigned int localPort = 8888;     // Локальный порт прослушевания сети 

const char *ssid     = "ROBO_CAR"; // SSID имя WiFi точки-доступа робокара 
const char* password = "12345678"; // Пароль WiFii  


IPAddress local_ip(192,168,43,100);  // IP-адрес робокара
IPAddress gateway(192,168,43,1);    // IP-адрес шлюза  
IPAddress subnet(255,255,255,0);   // Подсеть 

#define PACKET_MAX_SIZE 255         // Масимальный размер пакета-данных 
char packetBuffer[PACKET_MAX_SIZE];// Буферы для приема и отправки пакета-данных,
char replayBuffer[PACKET_MAX_SIZE];

WiFiUDP Udp;                       // UPD-сокет обект 


int PWMA=5; //Правая сторона ШИМ-A
int PWMB=4; //Левая  сторона ШИМ-B
int DA=0;   //Правая реверс  
int DB=2;   //Левый реверс  
int LED=16; //Светодиод 

// Структора данных , кнтроля робокара 
struct dataControl                                                                                 
{   
    int   speedX;     // Скорость вращения мотрчиков справа , ШИМ-А
    int   speedY;     // Скорость вращения мотрчиков слева  , ШИМ-B
    bool  enLED;      // включение\выключение светодиода 
};


// Обработчик событий
void EventHandler( const int& _speed_x , const int& _speed_y , const bool& _enabled_led )
{    
    // Устанавливаем реверс ШИМа 
    if( _speed_x > 0 ) digitalWrite(DA, HIGH); else  digitalWrite(DA, LOW); // вкл\выкл реверс спарава 
    if( _speed_y > 0 ) digitalWrite(DB, HIGH); else  digitalWrite(DB, LOW); // вкл\выкл реверс слева 

    delay(1);
     
    // Устанавливаем размер ширины ШИМа
    analogWrite(PWMA, abs(_speed_x)); // Скорость вращения справа  
    analogWrite(PWMB, abs(_speed_y)); // Скорость вращения слева 

    //-------------------------------//
    
    // Управления светодиодами  
    if( _enabled_led == true )
    {
      // Включить светодиод 
      digitalWrite(LED, HIGH);  
    }
    else
    { 
      // Выключить светодиод 
      digitalWrite(LED, LOW); 
    }

}


// Обработчик событий , плиморфизм 
void EventHandler(const dataControl&  _control)
{
   // Полиморфное переопределение 
   EventHandler( _control.speedX , _control.speedY , _control.enLED ); 
}


void setup() 
{
    //----------------------------//
    // Инцилизмруем все пины как OUTPUT
     pinMode(PWMA, OUTPUT); 
     pinMode(PWMB, OUTPUT); 
     pinMode(DA,   OUTPUT); 
     pinMode(DB,   OUTPUT); 
     pinMode(LED,  OUTPUT); 
    //----------------------------//

  Serial.begin(115200);                         // Запускаем сериал порт на скрости 115200.импульсов\сек 


    // присваиваем статичесий IP адрес
  WiFi.mode(WIFI_STA); // режим клиента
  WiFi.config(local_ip, gateway, subnet);
  WiFi.begin(ssid, password);

  int n=0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);

    if( n++ > 20 ) break;
  }

  if(WiFi.status() != WL_CONNECTED)
  {
    boolean result =
    WiFi.softAP(ssid, password);                  // Устанавливаем режым точки доступа WiFi
    WiFi.softAPConfig(local_ip, gateway, subnet); // Устанавливаем статические IP-адреса 
    delay(100);                                   // Ждем 100 милисекунд 

    // вывод данных о сети 
    IPAddress myIP = WiFi.softAPIP();             // IP-адрес робокара
    Serial.print("AP IP address: ");             
    Serial.println(myIP);
    if(result == true) Serial.println("Ready"); else Serial.println("Failed!");
  }
   
    
    // выввод информации о сервере  	
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("UDP server on port %d\n", localPort);
    
    Udp.begin(localPort);                         // Начинаем слушать порт 8888 . Ждем потключения клиента 
}


void loop()
{
    // Ждем ..! И если есть данные, начинаем обрабатывать пакет-данных
    int packetSize = Udp.parsePacket();
    if (packetSize) 
    {
        // Читаем пакеты в packageBufffer
        int n = Udp.read( packetBuffer , sizeof(dataControl));          
	      packetBuffer[n]=0;

        // Переобразуем данные в удобный нам формат 
        dataControl *data_control = (dataControl*)&packetBuffer;  
       
        // Обробатываем данные
        EventHandler(*data_control);                              

         char buff[256];
        sprintf( buff , "sx:%d , sy:%d , led:%d  \n" , 
                 data_control->speedX ,
                 data_control->speedY , 
                 data_control->enLED );

         // Выввод присланых данных в сериал порт 
        Serial.printf( buff );
        
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(buff);
        Udp.endPacket();
    }
    
    // Ждем 10 милисекунд 
    delay(10);
}
