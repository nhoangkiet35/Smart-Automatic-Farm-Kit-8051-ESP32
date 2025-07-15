int relay = 5;    //Chân relay số 5
int cb = A0;      //Chân cảm biến ở chân Analog: A0
int doc_cb, TBcb;

void setup()
  {
    Serial. begin (9600);
        
    pinMode (cb, INPUT);        //Tín hiệu vào từ cảm biến
    
    pinMode (relay, OUTPUT);    //Tín hiệu xuất ra từ relay
    digitalWrite (relay, LOW);
 
  }

void loop() 

  {   
    
    for(int i=0;i<=9;i++)   /*Chúng ta sẽ tạo một hàm for để đọc 10 lần giá trị cảm biến, 
                            sau đó lấy giá trị trung bình để được giá trị chính xác nhất.*/
        {
          doc_cb += analogRead(cb);     
        }
    TBcb=doc_cb/10;     //Tính giá trị trung bình
    
    
    int phantramao = map(TBcb, 0, 1023, 0, 100);    //Chuyển giá trị Analog thành giá trị %
    int phantramthuc = 100 - phantramao;              //Tính giá trị phần trăm thực, chuyển điện thế khô thành ẩm
    
    Serial. print("analog: "); Serial. print(TBcb); Serial.print(" "); Serial. print("~"); Serial.print(" "); 
    Serial.print(phantramthuc); Serial.println('%');
    delay (500);
    doc_cb=0;
    
    if ( phantramthuc >= 30 )         //Nếu phần trăm thực lớn hơn hoặc = 30%
      
      {
        digitalWrite (relay, LOW);    //Thì relay OFF, không bơm
      }
    
    else                              //Ngược lại, phần trăm thực nhỏ hơn 30%
      {
        digitalWrite (relay, HIGH);   //Thì relay ON, Bơm chạy
      }
   
  
  }
