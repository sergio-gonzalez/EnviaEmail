#include <DHCP.h>
#include <DNS.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUDP.h>
#include <Ethernet_W5500.h>
#include <Twitter.h>
#include <util.h>

/*
    Email client sketch for IDE v1.0.1 and w5100/w5200
    Posted December 2012 by SurferTim
 */
     
#include <SPI.h>
//#include <Ethernet.h>

     
// this must be unique
byte mac[] = { 0x70, 0xB3, 0xD5, 0x0A, 0xC1, 0x28 };  

IPAddress ip(172, 16, 11, 14);
IPAddress gateway( 172, 16, 5, 7);
IPAddress subnet( 255, 255, 0, 0 );

// your email server ip or domain
char server[] = "mail.ird.gov.br" ;
int port = 25;
     
EthernetClient client;
 
void setup()
{
  Serial.begin(9600);
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  delay(2000);
  Serial.println(F("Ready. Press 'e' to send."));
}
  
void loop()
{
  byte inChar;
 
  inChar = Serial.read();
 
  if(inChar == 'e')
  {
      if(sendEmail()) Serial.println(F("Email sent"));
      else Serial.println(F("Email failed"));
  }
}
 
byte sendEmail()
{
  byte thisByte = 0;
  byte respCode;
  int codError;
  codError = client.connect(server,port);
 
  if(codError == 1) {
    Serial.println(F("connected"));
  } 
  else {
    Serial.println(F("Codigo erro com Gateway"));
    Serial.println(server);
    Serial.println(codError);
    Serial.println(F("Cliente Status "));
    Serial.println(client.status());
    Serial.println(F("connection failed"));
    return 0;
  }
 
  if(!eRcv()) return 0;
  Serial.println(F("Sending helo"));
 
// change to your public ip
  client.println(F("helo 1.2.3.4"));
 
  if(!eRcv()) return 0;
  Serial.println(F("Sending From"));
 
// change to your email address (sender)
  client.println(F("MAIL From: <gonzalez@ird.gov.br>"));
 
  if(!eRcv()) return 0;
 
// change to recipient address
  Serial.println(F("Sending To"));
  client.println(F("RCPT To: <sensorln@ird.gov.br>"));
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending DATA"));
  client.println(F("DATA"));
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending email"));
 
// change to recipient address
  client.println(F("To: You <sensorln@ird.gov.br>"));
 
// change to your address
  client.println(F("From: Me <gonzalez@ird.gov.br>"));
 
  client.println(F("Subject: Temperatura Sensor LN\r\n"));
 
  client.println(F("Temperatura Sensor LN"));
 
  client.println(F("."));
 
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending QUIT"));
  client.println(F("QUIT"));
 
  if(!eRcv()) return 0;
 
  client.stop();
 
  Serial.println(F("disconnected"));
 
  return 1;
}
 
byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }
 
  respCode = client.peek();
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  if(respCode >= '4')
  {
    efail();
    return 0;  
  }
 
  return 1;
}
 
 
void efail()
{
  byte thisByte = 0;
  int loopCount = 0;
 
  client.println(F("QUIT"));
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  client.stop();
 
  Serial.println(F("disconnected"));
}
