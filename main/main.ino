#include <PS2X_lib.h>  // Thư viện điều khiển ps2
#include <Wire.h> //thư viện I2c của Arduino, do PCA9685 sử dụng chuẩn giao tiếp i2c nên thư viện này bắt buộc phải khai báo 
#include <Adafruit_PWMServoDriver.h> // thư viện PCA9685
#include <Adafruit_TCS34725.h> // thư viện của cảm biến màu sắc
#include <Servo.h> // thư viện servo

#define servo3601 15
#define serco3602 
#define PS2_DAT 1
#define PS2_CMD 2
#define PS2_SEL 5
#define PS2_CLK 6
#define pressures false
#define rumble false
PS2X ps2x; // khoi tao ps2
int error = 0; 
byte type = 0;
byte vibrate = 0;
int A;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); // Khởi tạo class của thư viện với địa chỉ gốc
Servo servo180;
const int trig = 24;   // trig của hcsr04
const int echo = 23;   // echio của hcsr04
const int SHOOT = 5;   
unsigned long duration; // biến đo thời gian
int distance;           // biến lưu khoảng cách


void setup() {
  Serial.begin(57600);
  delay(300);
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

  pinMode(trig,OUTPUT);   // pinmode hcsr04
  pinMode(echo,INPUT);    // pinmode hcsr04

  // setup motordc bắn
  pwm.begin(); //khởi tạo PCA9685 
  pwm.setOscillatorFrequency(27000000); // cài đặt tần số dao động 
  pwm.setPWMFreq(50);// cài đặt tần số PWM. Tần số PWM có thể được cài đặt trong khoảng 24-1600 HZ, tần số này được cài đặt tùy thuộc vào nhu cầu xử dụng. Để điều khiển được cả servo và động cơ DC cùng nhau, tần số PWM điều khiển được cài đặt trong khoảng 50-60Hz.
  Wire.setClock(400000); // cài đặt tốc độ giao tiếp i2c ở tốc độ cao nhất(400 Mhz). Hàm này có thể bỏ qua nếu gặp lỗi hoặc không có nhu cầu tử dụng I2c tốc độ cao

  // setup servo
  servo180.attach(30); // chân số 30

  //setup tcs
  if (tcs.begin()) 
  {
    Serial.println("Cảm biến hoạt động");
  } 
  else 
  {
    Serial.println("Cảm biến không kết nối, hãy kiểm tra lại kết nối...");
    while (1);
  }
}
void loop() {  
  int goc_180 = 0;
  int goc_360 = 1500;
  ps2_control();
  if (A==1) {moveforward();}
  if (A==2) {movebackward();}
  if (A==3) {moveleft();}
  if (A==4) {moveright();}
  if (A==5) {nuttron();}
  if (A==6) {nutvuong();}
  if (A==7) {nuttamgiac();}
  if (A==8) {nutX();}
  
  if(color()) servo180.write(60); // neu mau trắng thì chỉnh lên 60 độ
  else servo180.write(0); // đen thì chỉnh góc 0 độ

  // khoảng cách phù thì có thể điều chỉnh góc độ
  if(measure() <= 150){
    if (A==9) {R1(goc_180);} // chinh goc len
    if (A==10) {R2(goc_180);} // chinh goc xuong
    if (A==11) {L1(goc_360);} // chinh goc xoay trai
    if (A==12) {L2(goc_360);} // chinh goc xoay phai
  }
  if (A==0) {reset();} // khong nhan tinh hieu tra ve mac dinh
}
void ps2_control(){
  ps2x.read_gamepad(false, speed_motor); // đọc xog chỉnh tốc độ motor thành "motor_speed"  
  if(ps2x.Button(PSB_PAD_UP)){
    A=1;
  }
  else if(ps2x.Button(PSB_PAD_DOWN)){
    A=2;
  }
  else if(ps2x.Button(PSB_PAD_LEFT)){
    A=3;
  }
  else if(ps2x.Button(PSB_PAD_RIGHT)){
    A=4;
  }
  else if(ps2x.Button(PSB_RED)){ // nút tròn
    A=5;
  }
  else if(ps2x.Button(PSB_PINK)){ // nút vuông
    A=6;
  }
  else if(ps2x.Button(PSB_GREEN)){ // nút tam giác
    A=7;
  }
  else if(ps2x.Button(PSB_BLUE)){ // nút X
    A=8;
  }
  else if(ps2x.Button(PSB_R1)){
    A=9;
  }
  else if(ps2x.Button(PSB_R2)){
    A=10;
  }
  else if(ps2x.Button(PSB_L1)){
    A=11;
  }
  else if(ps2x.Button(PSB_L2)){
    A=12;
  }
  else {A=0;} 
  delay(50);

}
// thêm các lệnh điều khiển, chức năng vào các nút
void moveforward(){

}
void movebackward(){

}
void moveleft(){

}
void moveright(){

}
void nuttron(){
  pwm.setPWM(8, 0, 4096); //chân số 8 set chiều dương là PWM 100%
  pwm.setPWM(9, 0, 0);    //chân số 9 set chiều âm 
  //điều khiển kênh 8 và 9 của động cơ 1
  Serial.println("shooting");
}
void nutvuong(){

}
void nuttamgiac(){
  servo180.write(0); // chinh góc ve 0 độ
}
void nutX(){

}
void R1(int &goc){
  servo180.write(goc+1); // chinh goc len
  goc++;
}
void R2(int goc){
  servo180.write(goc-1); // chinh goc xuong
  goc--;
}
void L1(int &goc){
  goc += 8; // do pwm tu 1000 -> 2000 nen de goc tăng lên 1 độ thì sung tăng 8
  pwm.writeMicroseconds(servo3601, goc); // chinh goc xoay trai
}
void L2(int &goc){
  goc -= 8;// do pwm tu 1000 -> 2000 nen de goc giảm bớt 1 độ thì sung giảm 8
  pwm.writeMicroseconds(servo3601, goc ); // chinh goc xoay trai
}
void reset(){
  // mac dinh goc bắn 60 độ và góc ngang thẳng 
  servo180.write(60);
}
bool color(){
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  
  // Tính giá trị trung bình của các kênh màu
  uint16_t avg = (r + g + b) / 3;
  
  // Ngưỡng để phân loại màu đen và trắng
  uint16_t threshold = 300; // có thể điều chỉnh giá trị này
  
  if (avg < threshold) {
    Serial.println("Màu: Đen");
    return false;
  }
  Serial.println("Màu: Trắng");
  return true;
}

long measure() {
  // Tạo xung trigger
    digitalWrite(trig,0);   
    delayMicroseconds(2);
    digitalWrite(trig,1);   
    delayMicroseconds(5);   
    digitalWrite(trig,0);  

  // Đo thời gian xung echo
  duration = pulseIn(echoPin, HIGH);

  // Tính khoảng cách
  distance = int(duration/2/29.412);
  Serial.print(distance);
  Serial.println("cm");
  delay(200);
  return distance;

}