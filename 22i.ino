#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

// OLED 설정
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// NeoPixel 설정
#define PIN 4
#define NUMPIXELS 8
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN);

// 시간 제어
int interval = 1000;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

// 서보 및 LDR 핀 설정
Servo servo_updown;
Servo servo_rightleft;

int ldrtopl = A1;
int ldrtopr = A2;
int ldrbotl = A3;
int ldrbotr = A4;

int threshold_value = 30;

void setup() {
  Serial.begin(9600);

  // OLED 초기화
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();

  // NeoPixel 초기화
  pixels.begin();
  pixels.setBrightness(20);

  // 서보 초기화
  servo_rightleft.attach(5);
  servo_updown.attach(6);
  servo_rightleft.write(90);  // 중앙 위치
  servo_updown.write(120);    // 약간 위로 향함
  delay(2000);
}

void loop() {
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    float read_val = analogRead(A0);
    display_func(read_val);
  }

  // LDR 값 읽기
  int topl = analogRead(ldrtopl);
  int topr = analogRead(ldrtopr);
  int botl = analogRead(ldrbotl);
  int botr = analogRead(ldrbotr);

  int avgtop = (topl + topr) / 2;
  int avgbot = (botl + botr) / 2;
  int avgleft = (topl + botl) / 2;
  int avgright = (topr + botr) / 2;

  int diffelev = avgtop - avgbot;
  int diffazi = avgright - avgleft;

  // 좌우 제어
  if (abs(diffazi) >= threshold_value) {
    int angle = servo_rightleft.read();
    if (diffazi > 0 && angle > 0) {
      servo_rightleft.write(angle - 1);
    } else if (diffazi < 0 && angle < 180) {
      servo_rightleft.write(angle + 1);
    }
  }

  // 상하 제어
  if (abs(diffelev) >= threshold_value) {
    int angle = servo_updown.read();
    if (diffelev > 0 && angle > 0) {
      servo_updown.write(angle - 1);
    } else if (diffelev < 0 && angle < 180) {
      servo_updown.write(angle + 1);
    }
  }

  delay(10);
}

void display_func(float volt_sensor_val) {
  float temp = volt_sensor_val / 4.092;
  float volt = temp / 100.0;
  if (volt < 0) volt = 0;

  Serial.print("Voltage: ");
  Serial.println(volt, 2);

  // NeoPixel 표시
  int gauge = map((int)(volt * 100), 0, 600, 0, NUMPIXELS);
  for (int i = 0; i < NUMPIXELS; i++) {
    if (i <= gauge) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }
  pixels.show();

  // OLED 표시
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.print(volt, 1);
  display.print("V");
  display.display();
}
