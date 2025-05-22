void setup() {
  Serial.begin(9600);
}

void loop() {
  float temp;
  float val2;
  int val1 = analogRead(A0);  // 아날로그 값 읽기

  temp = val1 / 4.092;        // 값을 보정
  val1 = (int)temp;           // 정수로 변환
  val2 = val1 / 10.0;         // 소수점 전압 계산

  Serial.print("전압 : ");
  Serial.print(val2);
  Serial.println("V");
  delay(100);
}
