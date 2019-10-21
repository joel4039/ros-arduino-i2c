#include <ESP8266WiFi.h>
#include <math.h>
#include <Wire.h>
#include <ros.h>
#include <geometry_msgs/Twist.h>
//Allow patching over wireless/
//#include <ESP8266mDNS.h>
//#include <ArduinoOTA.h>

//Set up the ros node and publisher
IPAddress server(0,0,0,0);
const uint16_t serverPort = 11411;
ros::NodeHandle nh;

void onTwist(const geometry_msgs::Twist &msg);
void startWiFi();

const int redLedPin  = 0; // Onboard LED = digital pin 0 (red LED on ESP8266)
const int blueLedPin = 2; // Onboard LED = digital pin 0 (blue LED on ESP8266)

//Set up the ros subscribers for both motor controllers
ros::Subscriber <geometry_msgs::Twist> sub("/cmd_vel", &onTwist);


std_msgs::String str_msg;
ros::Publisher chatter("/chatter", &str_msg);

void setup()
{
  Serial.begin(9600);
  Wire.begin(4,5);
  Serial.println("test");
  startWiFi();
//  ArduinoOTA.begin();
  nh.getHardware()->setConnection(server, serverPort);

  nh.initNode();
  nh.subscribe(sub);
  checkAddresses();
}

char hello[13] = "hello world!";
void loop()
{
  nh.spinOnce();
}

void startWiFi(){
  WiFi.begin("SSID", "PASSWORD");

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(blueLedPin, LOW);
    delay(500);
    digitalWrite(blueLedPin, HIGH);
    delay(500);
  }
  digitalWrite(blueLedPin, LOW);
}

void onTwist(const geometry_msgs::Twist &msg)
{
  // Cap values at [-1 .. 1]
  float x = _max(_min(msg.linear.x, 1.0f), -1.0f);
  float z = _max(_min(msg.angular.z, 1.0f), -1.0f);
  // Calculate left and right wheels values based on a super rough translation.
  float left = (msg.linear.x - msg.angular.z) / 2*255;
  float right = (msg.linear.x + msg.angular.z) / 2*255;

  //Scale the motor values
  float scale = 0;
  if (right > 255){
    scale = 255/right;
  }
  if (left > 255){
    scale = 255/left;
  }
  left = scale * left;
  right = scale * right;
  setMotor((int)left,false);
  setMotor((int)right,true);
}

void checkAddresses(){
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 80; address < 100; address++ ) {
    Wire.beginTransmission(address);
    Serial.println(address);
    Wire.write(0x5);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);
}
