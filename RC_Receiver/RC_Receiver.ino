/*************************************************************************************************
 * Arduino ラジコン受信機(ラジコン本体)プログラム
 * Produced by P-Lab Programming School
 *                                                                              @author P-LAB
 *                                                                              @date 2018/12/24
 ************************************************************************************************/
#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <Servo.h>

// コメント:１行(プログラムとして実行されません)
/* コメント:１行(説明などを自由に書き込みましょう) */
/* コメント:複数行 
 * (こうやると何行もコメントにできます。)
 */

/* 赤い基板のモーター制御回路のArduino出力ピン定義 */
#define IN1 2   // D2
#define IN2 4   // D4
#define IN3 3   // D3
#define IN4 5   // D5
#define SERVO 6 // D6

/* ヘッドライトLEDの出力ピン定義(自分ではんだ付け) */
#define LIGHT 9 // D9

/* 無線モジュール設定 */
RF24 radio(7, 8); // CE, CSN
// 半角5文字の無線識別コード(自分専用コード。送受信側で同じものを使います) 
const byte address[6] = "00001"; //★★自分用の番号を定義してみよう★★
// 送信パラメータ(X, Y, ボタンの３つ。今回は X,Y しか使いません。)
short ctlParam[3];

/* サーボ制御ハンドル */
Servo myServo;

/****************************************************
 * セットアップ関数
 * 起動時に１回だけ実行します。
 ****************************************************/
void setup() {
  // シリアル通信開始
  Serial.begin( 9600 );
  Serial.println( "Arduino Receiver Start!!!" );
  //★★「シリアルモニタ」を使って確認してみよう★★

  // 無線モジュール開始
  radio.begin();
  radio.openReadingPipe(0, address); //上で定義した無線識別コード
  radio.setPALevel(RF24_PA_MIN);     //信号の強さは最小
  radio.startListening();            //受信開始

  //モーター制御回路の出力ピン設定
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  //ヘッドライトLEDの出力ピン設定
  pinMode(LIGHT, OUTPUT);

 //電源ON時にヘッドライトを３回点滅させます。
  int i=0;
  for( i=0; i<3; i++ ){
    digitalWrite( LIGHT, HIGH );
    delay( 200 );
    digitalWrite( LIGHT, LOW );
    delay( 200 );
  }

  //サーボの出力ピン設定
  myServo.attach(SERVO);
}

/****************************************************
 * ループ関数 
 * セットアップ関数が実行された後、無限に実行します。
 ***************************************************/
void loop() {

  //無線モジュールが使える場合にだけ動きます
  if (radio.available()) {
    //送信パラメータを読み込みます
    radio.read(&ctlParam, sizeof(ctlParam));

    //送信パラメータから値を取り出して変数に代入します
    int ServoAngle = ctlParam[0];
    int MotorAngle = ctlParam[1];
//    int button = ctlParam[2];  //ボタンの感度がイマイチなので使わない

    //サーボを動かす角度を決定します
    // ★★サーボの角度が右と左で違います。自分のマシンに合わせて調整してみよう★★
    // 停止：90      (★最初にタイヤが真っすぐに走るように取り付けておく)
    // 左：0から90   (★  0まで回しちゃダメ。サーボのギアが壊れます)
    // 右：90から180 (★180まで回しちゃダメ。サーボのギアが壊れます)
    if(ServoAngle < 90){
      //左向きにはそんなに動かさない
      ServoAngle = 0.4 * ServoAngle + 35;
    } else if (ServoAngle > 90){
      //右向きの場合は結構動かす
      ServoAngle = 0.5 * (ServoAngle - 90) + 90;
    } else {
      //真ん中は90度
      ServoAngle = 90;
    }
    //決定したサーボ角度を出力します
    myServo.write(ServoAngle);

    //モーターを動かす方向を決定します
    if(MotorAngle > 90){
      //モーターに「青色→黄色」の向きに電流を流します
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      digitalWrite(LIGHT, LOW);    //ヘッドライト
      Serial.print("BACKWARD: "); //後ろに進む
    } else if(MotorAngle < 90) {
      //モーターに「青色←黄色」の向きに電流を流します
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      digitalWrite(LIGHT, HIGH);  //ヘッドライト
      Serial.print("FORWARD: "); //前に進む
    } else {
      //モータに電流を流しません。ストップさせます。
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      digitalWrite(LIGHT, LOW);   //ヘッドライト
      Serial.print("STOP: ");    //ストップ
    }

    //シリアル出力して値を確認します
    Serial.print("ServoAngle=");
    Serial.print(ServoAngle);
    Serial.print(", MotorAngle=");
    Serial.println(MotorAngle);
  }

  // 5ms(0.005秒)間待ってからまたループを繰り返します。
  delay(5);
}
