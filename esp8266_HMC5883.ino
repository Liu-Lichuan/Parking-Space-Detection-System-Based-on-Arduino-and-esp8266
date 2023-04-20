#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <ESP8266WiFi.h>

#define AP_ssid "ESP8266"
#define password "123456780"

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
double ans;
double ans_start;
int LED_R = D3;
int LED_G = D4;
int i = 0;

void displaySensorDetails(void) {
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print("Unique ID:    ");
  Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println(" uT");
  Serial.print("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println(" uT");
  Serial.print("Resolution:   ");
  Serial.print(sensor.resolution);
  Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("HMC5883 Magnetometer Test");
  Serial.println("");

  if (!mag.begin()) {
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1)
      ;
  }
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);

  digitalWrite(LED_G, LOW);
  digitalWrite(LED_R, LOW);

  sensors_event_t event_start;
  mag.getEvent(&event_start);  // 读取初始磁场数据
  ans_start = sqrt(event_start.magnetic.x * event_start.magnetic.x + event_start.magnetic.y * event_start.magnetic.y + event_start.magnetic.z * event_start.magnetic.z);

  WiFi.begin(AP_ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    i++;
    delay(500);
    Serial.print(".");
    if (i > 120) {  //60秒后如果还是连接不上，就判定为连接超时
      Serial.print("连接超时！请检查网络环境");
      break;
    }
  }
  Serial.println("网络连接成功");

  Serial.println(WiFi.localIP());

  Serial.println("AT+CIPMODE=1");
  Serial.println("AT+CIPSEND");

  /* Display some basic information on this sensor */
  displaySensorDetails();
}

const char* host = "192.168.4.1";
const uint16_t port = 8080;
int flag_send;

void loop() {

  WiFiClient tcpclient;

  /* 建立TCP连接 */
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);
  if (!tcpclient.connect(host, port)) {
    Serial.println("connection failed");  // 如果连接失败，则打印连接失败信息，并返回
    delay(5000);
    return;
  }

  sensors_event_t event;
  mag.getEvent(&event);  // 读取磁场数据
  // 输出磁场数据
  Serial.print("X: ");
  Serial.print(event.magnetic.x);
  Serial.print("  ");
  Serial.print("Y: ");
  Serial.print(event.magnetic.y);
  Serial.print("  ");
  Serial.print("Z: ");
  Serial.print(event.magnetic.z);
  Serial.print("  ");
  Serial.print("uT ");

  ans = sqrt(event.magnetic.x * event.magnetic.x + event.magnetic.y * event.magnetic.y + event.magnetic.z * event.magnetic.z);
  Serial.print(ans);
  Serial.print(" ");
  Serial.println(ans_start);

  if (ans < ans_start + 30 && ans > ans_start - 30) {
    digitalWrite(D4, HIGH);
    digitalWrite(D3, LOW);
    flag_send = 0;
  }

  else {
    digitalWrite(D4, LOW);
    digitalWrite(D3, HIGH);
    flag_send = 1;
  }

  if (flag_send) {
    tcpclient.write("112112112112");
    Serial.println("112112112112");

  } else {
    tcpclient.write("334334334334");
    Serial.println("334334334334");
  }

  delay(5000);  // 延时5秒
}