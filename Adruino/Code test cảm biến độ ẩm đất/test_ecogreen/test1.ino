int relay = 5;  //Chân relay số 5 
int cb = 6;     //Chân cảm biến số 6

void setup()

  {
    Serial. begin (9600);
        
    pinMode (cb, INPUT);        //Chân cảm biến đưa tín hiệu vào
    
    pinMode (relay, OUTPUT);    //Xuất tiến hiệu ra chân relay
    digitalWrite (relay, LOW);
 
  }

void loop() 

  {
    int digital = digitalRead (cb);                        //Đọc tín hiệu digital ở chân cảm biến 
    
    Serial. print("Digital: "); Serial. println(digital);  //In ra màn hình
    delay (500);

    
    if ( digital == 0 )               //Nếu cảm biến ở mức 0 (mức thấp; có nước)
      
      {
        digitalWrite (relay, LOW);    // Thì relay OFF, không bơm
      }
    
    else                              // Ngược lại, nếu cảm biến ở mức 1 (mức cao, không có nước)
      {
        digitalWrite (relay, HIGH);   // Thì relay ON, bơm nước
      }
   
  }
