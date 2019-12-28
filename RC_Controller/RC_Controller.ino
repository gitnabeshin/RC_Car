/*************************************************************************************************
 * Arduino ラジコン送信機(コントローラー)プログラム
 * Produced by P-Lab Programming School
 *                                                                              @author P-LAB
 *                                                                              @date 2018/12/24
 ************************************************************************************************/
#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

// コメント:１行(プログラムとして実行されません)
/* コメント:１行(説明などを自由に書き込みましょう) */
/* コメント:複数行 
 * (こうやると何行もコメントにできます。)
 */

/* ジョイスティックX,YのArduino入力ピン定義(A0, A1) */
#define JOY_Y A0 // Y成分(モーター用)は「アナログ0番」に接続する
#define JOY_X A3 // X成分(サーボ用)は「アナログ1番」に接続する

/* 無線モジュール設定 */
RF24 radio(7, 8); // CE, CSN
// 半角5文字の無線識別コード(自分専用コード。送受信側で同じものを使います) 
const byte address[6] = "00001"; //★★自分用の番号を定義してみよう★★
// 送信パラメータ(X:サーボ角度, Y:モーター方向, ボタンの３つ。今回は X,Y しか使いません。)
short ctlParam[3];

/****************************************************
 * セットアップ関数
 * 起動時に１回だけ実行します。
 ****************************************************/
void setup() {
  // シリアル通信開始
  Serial.begin( 9600 );
  Serial.println( "Arduino Controller Start!!!" );
  //★★「シリアルモニタ」を使って確認してみよう★★

  // 無線モジュール開始
  radio.begin();
  radio.openWritingPipe(address); //上で定義した無線識別コード
  radio.setPALevel(RF24_PA_MIN);  //信号の強さは最小
  radio.stopListening();          //受信停止
}

/****************************************************
 * ループ関数 
 * セットアップ関数が実行された後、無限に実行します。
 ***************************************************/
void loop() {

  // サーボ用にジョイスティックから角度Xを読み込みます
  int X_Value = analogRead(JOY_X);
  int ServoAngle = map(X_Value, 0, 1023, 0, 180);
  ServoAngle = 180 - ServoAngle;

  // モータ用にジョイスティックから角度Yを読み込みます
  int Y_Value = analogRead(JOY_Y);
  int MotorAngle = map(Y_Value, 0, 1023, 0, 180);

  // 拡張用（今回は使いません）
  int Button = digitalRead(2);

  // モーター用ジョイスティックの中立角度が90度から少しずれているので、90度に補正
  // 85より大きくて、かつ、95より小さかったら90にする
  if(MotorAngle > 85 && MotorAngle < 95 ){
    MotorAngle = 90;
  }

  // ★★サーボの値を補正してみよう★★
  // 85より大きくて、かつ、95より小さかったら90にする

  

  //シリアル出力して値を確認します
  Serial.print( " ServoAngle=" );
  Serial.print( ServoAngle );
  Serial.print( ", MotorAngle=" );
  Serial.println( MotorAngle );

  // 送信パラメータの変数に値をセットします。
  // 送信パラメータ(X, Y, ボタンの３つ。今回は X,Y しか使いません。)
  ctlParam[0] = ServoAngle;
  ctlParam[1] = MotorAngle;
  ctlParam[2] = Button;

  // パラメータを送信します。
  radio.write(&ctlParam, sizeof(ctlParam));

  // 5ms(0.005秒)間待ってからまたループを繰り返します。
  delay(5);
}
