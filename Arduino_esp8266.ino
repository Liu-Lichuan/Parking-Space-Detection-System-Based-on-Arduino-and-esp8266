#include <SoftwareSerial.h>

SoftwareSerial espSerial(2, 11);

void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);
  delay(500);
}

String receive;

void loop() {
  if (Serial.available()) {
    espSerial.write(Serial.read());
  }

  if (espSerial.available()) {
    char c;
    receive = "";
    while (espSerial.available()) {
      c = espSerial.read();
      receive += c;
    }

    // 校验和改为接收的字节数
    int sum = receive.length();
    if (receive.indexOf("112") != -1 || receive.indexOf("334") != -1) {
      // 处理包含子串的数据
    }


    // 添加空字符串判断
    if (receive.length() > 0) {
      // 更高级的字符串匹配
      if (receive.indexOf("AT+") != -1) {
        // 解析AT+指令并响应
        Serial.println("AT cmd received");
      } else if (receive.indexOf("112") != -1) {
        Serial.println("stopping");
      } else if(receive.indexOf("334") != -1){
        Serial.println("no stopping");
      }
    }
  }
}