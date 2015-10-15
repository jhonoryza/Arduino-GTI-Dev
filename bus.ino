#include <Bounce2.h>
#include <fonts/Arial14.h>
#include <fonts/SystemFont5x7.h>
/*
  Scrolling alphabet demo, displays characters one at a time into a scrolling box.
 */

#include <SPI.h>
#include <DMD2.h>
#include <SD.h>
//#include <SPI.h>
#include <fonts/Tahoma.h>
#include <LCD5110_Basic.h>


const uint8_t *FONT = SystemFont5x7; 
const uint8_t *FONTKoridor = Tahoma; 
const int W1 = 3;
const int W2 = 5;


boolean state = true;
int lengthKor;
int xDepan,width_1,width_2,xDepan2, width_true;
int lenStr= 1;
int start=0;
boolean btnPress = false;
const int BTN_F = 0;
const int BTN_R = 1;
const int BTN_C = 2;
Bounce bouncer_f = Bounce();
Bounce bouncer_r = Bounce();
Bounce bouncer_c = Bounce();
long last_read = 0;

int RST = 3;
int CLK = 4;
int DAT = 5; 
int wDisplay[8]={5,2};
SoftDMD Depan(W2,1,22,23,24,26,25,27);
SoftDMD Indoor(W1,1,28,29,30,32,31,33);
//DMD_TextBox box(Depan,21,0,160,16); 
DMD_TextBox box(Depan,0,0,64,16); 
DMD_TextBox boxIndoor(Indoor,0,0,160,16); 
char* fileName_k[100]={"k1.txt","k2.txt","k3.txt","k4.txt","k5.txt"};
//char* fileName_w[100]={"w1.txt","w2.txt","w3.txt","w4.txt","w5.txt"};
char* fileName_h1[100]={"1a.txt","2a.txt","3a.txt","4a.txt","5a.txt"};
char* fileName_h2[100]={"1b.txt","2b.txt","3b.txt","4b.txt","5b.txt"};
char* fileName_s1[100]={"s1a.txt","s2a.txt","s3a.txt","s4a.txt","s5a.txt"};
char* fileName_s2[100]={"s1b.txt","s2b.txt","s3b.txt","s4b.txt","s5b.txt"};
int nomor = 0;
int noKoridor = 1;
int noHalte = 1;
boolean pilihKoridor = false;
boolean pilihHalte = false;
String koridor[800];
String halte_1[800];
int halte_1p;
int halte_2p;
String halte_2[30];
int sound_1[30];
int sound_2[30];

File myFile;
int button_f;
int button_r;
int button_c;
int pilih = 0;
int xIndoor = 63;

int lebarAngka;

boolean success = false;
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];
//LCD5110 myGLCD(12,11,10,8,9);
LCD5110 myGLCD(8,9,10,11,12);
void setup() {
  Serial.begin(9600);
  myGLCD.InitLCD();
   pinMode(RST, OUTPUT);
   pinMode(CLK, OUTPUT); 
   pinMode(DAT, OUTPUT);
    
    
    digitalWrite(RST, HIGH);
    digitalWrite(CLK, HIGH);
    digitalWrite(DAT, HIGH);
    
    digitalWrite(RST, LOW);
    delay(5);
    digitalWrite(RST, HIGH);
    delay(300);

  //LCD("TEST",1);
  Serial.println("Check SD");
  Serial.println("Initializing SD card");
  lcdWrite("Initializing SD card...");
  if(!SD.begin(53)) {
    lcdWrite(" failed!");
    Serial.println("failed");
    return;
  }
  success = true;
  lcdWrite("SUCCESS");
  // hi-speed SPI transfers
  SPI.setClockDivider(4);
  //readWidth();
  pinMode(BTN_F,INPUT_PULLUP);
  pinMode(BTN_R,INPUT_PULLUP);
  pinMode(BTN_C,INPUT_PULLUP);
  bouncer_f.attach(BTN_F);
  bouncer_r.attach(BTN_R);
  bouncer_c.attach(BTN_C);
  bouncer_f.interval(5);
  bouncer_r.interval(5);
  bouncer_c.interval(5);

  Depan.setBrightness(255);
  Depan.selectFont(FONT);
  Depan.begin();
  Indoor.setBrightness(255);
  Indoor.selectFont(FONTKoridor);
  Indoor.begin();
  
  xDepan = W2*32;
  //xDepan = 126;
  xDepan2 = W1*32;
  xIndoor = W1*32;
  width_1 = W1*32;
  width_2 = W2*32;
 
   readKoridor();
//   readHalte_a();
  }
  
  void loop() {
    
    if(success==true){
    button_f = digitalRead(BTN_F);
    button_r = digitalRead(BTN_R);
    button_c = digitalRead(BTN_C);

//    bouncer_f.update();
//    bouncer_r.update();
//    bouncer_c.update();
//  int value1 = bouncer_f.read();
//  int value2 = bouncer_r.read();
//  int value3 = bouncer_c.read();

//          if(value1 == LOW){
//            if(bouncer_f.fell()){
      //jika belum memilih koridor
      if(!pilihKoridor){
        LCD("KORIDOR",noKoridor);
        delay(100);
          if(button_f == LOW){
            noKoridor++;
            if(noKoridor>20)
              noKoridor=20;
              delay(10);
          }
          else if(button_r == LOW){
            noKoridor--;
            if(noKoridor<0)
              noKoridor=0;
               delay(10);
          }
          else if(button_c == LOW && noKoridor>0){
            //readKoridor();
            
            show_koridor();
             delay(10);
          }
      }
      //jika sudah dipilih koridor tampilkan menu halte
      else if(pilihKoridor){
          LCD("Halte",noHalte);
          delay(100);
          loopKoridor();
          if(button_f == LOW){
            noHalte++;
            if(noHalte>20)
              noHalte=20;
               delay(10);
          }
          else if(button_r == LOW){
            noHalte--;
            if(noHalte<0)
              noHalte=0;
               delay(10);
          }
          else if(button_c == LOW && noHalte>0){
            //readHalte_a();
            showHalte();
             delay(10);
          }
      }
    }
    else{
      lcdWrite("FAILED");
      delay(1000);
    }
  
  }

  void readKoridor(){
   //show_koridor();
   char character;
   String settingValue;
   //char* fileName = "k" + String(nomor) + "a.txt";
   int counter = 0;
   myFile = SD.open(fileName_k[0]);
   lcdWrite("Read Koridor ");
   delay(100);
   if (myFile) {
   while (myFile.available()) {
   character = myFile.read();
   //Serial.println(myFile.read());
   if (character != '\n') // define breaking char here (\n isnt working for some reason, i will follow this up later)
    {
         settingValue = settingValue + character;
    }
    else  
    {
      koridor[counter] = settingValue;
      settingValue = "";
      delay (100);
      counter++;
    }
    
   }
    //from_sd = settingValue;
   //halte_1p = counter;
   // close the file:
   myFile.close();
   //lengthKor = koridor[0].length()*7 + koridor[1].length()*7;
   //pilihKoridor = true;
   lcdWrite("Success");
   delay(100);
   readHalte_a();
   //readHalte_a();
   //show_koridor(); 
   } else {
   // if the file didn't open, print an error:
    lcdWrite("ERROR READ");
    delay(100);
   }
   
   
  }

  void readHalte_a(){
   char character;
   String settingValue;
   //char* fileName = String(nomor) +"a.txt";
   int counter = 0;
   myFile = SD.open(fileName_h1[0]);
   lcdWrite("Read Halte ");
    delay(100);
   if (myFile) {
   while (myFile.available()) {
   character = myFile.read();
   
   if (character != '\n') // define breaking char here (\n isnt working for some reason, i will follow this up later)
    {
         settingValue = settingValue + character;
    }
    else  
    {
      halte_1[counter] = settingValue;
      settingValue = "";
      delay (100);
      counter++;
    }
    
   }
    
   //from_sd = settingValue;
   halte_1p = counter;
   // close the file:
   
   //LCD("KORIDOR",nomor);
    myFile.close();
   lcdWrite("Success");
   delay(100);
//   show_koridor();
   
   //readHalte_b();
   } else {
   // if the file didn't open, print an error:
    lcdWrite("ERROR READ");
    delay(1000);
   }
   
   
  }

//  void readHalte_b(){
//   char character;
//   String settingValue;
//  // char* fileName = String(nomor) +"b.txt";
//   int counter = 0;
//   myFile = SD.open(fileName_h2[nomor-1]);
//   lcdWrite("Read Halte " + String(nomor));
//    delay(1000);
//   if (myFile) {
//   while (myFile.available()) {
//   character = myFile.read();
//   
//   if (character != '\n') // define breaking char here (\n isnt working for some reason, i will follow this up later)
//    {
//         settingValue = settingValue + character;
//    }
//    else  
//    {
//      halte_2[counter] = settingValue;
//      settingValue = "";
//      delay (100);
//      counter++;
//    }
//    
//   }
//    
//   //from_sd = settingValue;
//   halte_2p = counter;
//   // close the file:
//   myFile.close();
//   //readSound_a();
//   kor_1();
//   //show_koridor();
//   } else {
//   // if the file didn't open, print an error:
//    lcdWrite("ERROR READ");
//   }
//  }

  void show_koridor(){
        xDepan = W2*32;
        for(int i=150;i>-(1);i--){
            if(nomor<10){
            Depan.drawString(0,2,String(noKoridor));
            }else{
              Depan.drawString(0,2,String(noKoridor));
            }
            Depan.drawString(xDepan,2,koridor[noKoridor-1]);
            box.scrollX(-1);
            xDepan--;
            delay(10);
        }
        pilihKoridor = true;
}
void interruptF(){
  button_f = digitalRead(BTN_F);
  if(button_f == LOW){
    noHalte++;
    if(noHalte>20)
      noHalte=20;
    delay(10);
  }
}
void interruptR(){
  button_r = digitalRead(BTN_R);
  if(button_r == LOW){
    noHalte--;
    if(noHalte<0)
      noHalte=0;
    delay(10);
  }
}
void interruptC(){
  button_c = digitalRead(BTN_C);
  if(button_c == LOW){
    showHalte();
    delay(10);
  }
}
void loopKoridor(){
    
            xDepan = W2*32;
            Depan.clearScreen();
          //  attachInterrupt(BTN_F, interruptF, FALLING);
          //  attachInterrupt(BTN_R, interruptR, FALLING);
          //  attachInterrupt(BTN_C, interruptC, FALLING);
            for(int i=150;i>-(1);i--){
    button_f = digitalRead(BTN_F);
    button_r = digitalRead(BTN_R);
    button_c = digitalRead(BTN_C);
    
          if(button_f == LOW){
            noHalte++;
            if(noHalte>20)
              noHalte=20;
               delay(10);
          }
          else if(button_r == LOW){
            noHalte--;
            if(noHalte<0)
              noHalte=0;
               delay(10);
          }
          else if(button_c == LOW && noHalte>0){
            //readHalte_a();
            showHalte();
             delay(10);
          }
          else{
                if(nomor<10){
                Depan.drawString(0,2,String(noKoridor));
                }else{
                Depan.drawString(0,2,String(noKoridor));
                }
                Depan.drawString(xDepan,2,koridor[noKoridor-1]);
                box.scrollX(-1);
                xDepan--;
                delay(400);
          }
      }
}
void showHalte(){
  xIndoor = W1*32;
  for(int i=0;i<86;i++){
    
            if(nomor<10){
              Indoor.drawString(0,2,String(noHalte));
            }else{
              Indoor.drawString(0,2,String(noHalte));
            }
        if(millis() - last_read > 1) 
        {
            Indoor.drawString(xIndoor,2,halte_1[noHalte-1]);
            boxIndoor.scrollX(-1);
            xIndoor--;
            //delay(100);
        }
        last_read = millis();  
  }
}
  void readSound_a(){
   char character;
   String settingValue;
   int counter = 0;
   //char* fileName = "s"+String(nomor) +"a.txt";
   myFile = SD.open(fileName_s1[nomor-1]);
   lcdWrite("Reading Sound " + String(nomor));
   if (myFile) { 
   while (myFile.available()) {
   character = myFile.read();
   
   if (character != '\n') // define breaking char here (\n isnt working for some reason, i will follow this up later)
    {
         settingValue = settingValue + character;
    }
    else  
    {
      sound_1[counter] = settingValue.toInt();
      settingValue = "";
      delay (100);
      counter++;
    }
    
   }
    
   //from_sd = settingValue;
   
   // close the file:
   myFile.close();
   readSound_b();
   
    } else {
   // if the file didn't open, print an error:
    lcdWrite("ERROR READ");
   }
   
   
  }

  void readSound_b(){
   char character;
   String settingValue;
   int counter = 0;
   //char* fileName = "s"+String(nomor) +"b.txt";
   myFile = SD.open(fileName_s2[nomor-1]);
   lcdWrite("Reading Sound " + String(nomor));
   if (myFile) { 
   while (myFile.available()) {
   character = myFile.read();
   
   if (character != '\n') // define breaking char here (\n isnt working for some reason, i will follow this up later)
    {
         settingValue = settingValue + character;
    }
    else  
    {
      sound_2[counter] = settingValue.toInt();
      settingValue = "";
      delay (100);
      counter++;
    }
    
   }
    
   //from_sd = settingValue;
   
   // close the file:
   
   myFile.close();
   
   lcdWrite("READ DONE");
   delay(2000);
   LCD(halte_1[0],1);
   success = true;
   if(nomor<10)
   {
     width_true = width_1 - 17;
   }else
   {
     width_true = width_1 - 27;
   }
  // kor_1();
   show_koridor();
    } else {
   // if the file didn't open, print an error:
    lcdWrite("ERROR READ");
   }
   
   
  }

void isPressedd(){
      if(btnPress == true){
       sendd(sound_1[pilih]);
       btnPress = false;
      }
}
void isPressedd_2(){
      if(btnPress == true){
       sendd(sound_2[pilih]);
       btnPress = false;
      }
}

void LCD(String z, long int y){
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myGLCD.print(z, CENTER, 0);
  myGLCD.setFont(BigNumbers);
  myGLCD.printNumI(y, CENTER, 8, 2, '0');
  myGLCD.setFont(SmallFont);
  //delay(300);
}

void lcdWrite(String z)
{
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myGLCD.print(z, CENTER, 0);
}


  void kor_1(){
  pilih = 0;
    while(true){
    btnPress = false;
    attachInterrupt(BTN_C, interruptBtnC, FALLING);
    //attachInterrupt(BTN_R, draw4, FALLING);
    //isPressedd();
      if(lengthKor>width_true){
        
      for(int i=63;i>-(lenStr);i--){
            if(pilih>halte_1p-2){
              pilih=0;
              kor_2();
            }
            if(nomor<10){
            Depan.drawBox(0,0,15,15);
            Depan.drawBox(1,1,14,14);
            Depan.drawString(5,2,String(nomor));
            }else{
              Depan.drawBox(0,0,25,25);
              Depan.drawBox(1,1,24,24);
              Depan.drawString(5,2,String(nomor));
            }
            
            if(xDepan>width_1-koridor[0].length()*9){ 
              
                  Depan.drawString(xDepan,3,koridor[0]);
                  box.scrollX(-1);
                  xDepan--;
                  
                  }else{
                    
                  if(xDepan2>20)
                  {
                      Depan.drawString(xDepan2,3,"%_"+koridor[1]);
                      box.scrollX(-1);
                      xDepan2--;
                      }else
                      {
                          box.scrollX(-1);
                          xDepan2--;
                          if(xDepan2<-(lengthKor))
                          {
                          xDepan=width_1;
                          xDepan2=width_1;
                          }
                      }
                  }
            
            
            
            
            if(halte_1[pilih].length()*9>width_2)
            {
              Indoor.drawString(xIndoor,3,halte_1[pilih]);      
              xIndoor--;
              if(xIndoor<-(lenStr))xIndoor= 63;
              isPressedd();
            }else
            {
              Indoor.drawString(0,3,halte_1[pilih]);
              isPressedd();
            }
            
            
            delay(100);      
        }
      
      }
      else{
        if(xIndoor<-(lenStr))
          xIndoor= width_2;
        for(int i=63;i>-(lenStr);i--){
              
              if(nomor<10)
              {
                Depan.drawBox(0,0,15,15);
                Depan.drawBox(1,1,14,14);
                Depan.drawString(5,2,String(nomor));
              }else
              {
                Depan.drawBox(0,0,25,25);
                Depan.drawBox(1,1,24,24);
                Depan.drawString(5,2,String(nomor));
              }
              if(pilih>halte_1p-2){
                kor_2();
                pilih=0;
              }
              
              Depan.drawString(21,3,koridor[0] + "_%_" + koridor[1]);
              
              
              if(halte_1[pilih].length()*9>width_2){
              Indoor.drawString(xIndoor,3,halte_1[pilih]);      
              xIndoor--;
              if(xIndoor<-(lenStr))xIndoor= width_2;
              isPressedd();
              }else{
                Indoor.drawString(0,3,halte_1[pilih]);
                isPressedd();
              }
              isPressedd();
              delay(100);
                      
           }
      }
    }
  
  }

  void kor_2(){
  pilih = 0;
  while(true){
  btnPress = false;
  attachInterrupt(BTN_C, interruptBtnC_2, FALLING);
  //attachInterrupt(BTN_R, draw4, FALLING);
  //isPressedd();
  if(lengthKor>width_true){
    
  for(int i=63;i>-(lenStr);i--){
        if(pilih>halte_2p-3){
          pilih=0;
          kor_1();
        }
        
        Depan.drawBox(0,0,15,15);
        Depan.drawBox(1,1,14,14);
        Depan.drawString(5,2,String(nomor));
        
        if(xDepan>160-koridor[1].length()*9){     
        Depan.drawString(xDepan,3,koridor[1]);
        box.scrollX(-1);
        xDepan--;
        }else{
        if(xDepan2>20){
        Depan.drawString(xDepan2,3,"%_"+koridor[0]);
        box.scrollX(-1);
        xDepan2--;
        }else{
        box.scrollX(-1);
        xDepan2--;
        if(xDepan2<-(lengthKor)){
          xDepan=160;
          xDepan2=160;
        }
        }
        }
        
        
        if(halte_2[pilih].length()*9>63){
        Indoor.drawString(xIndoor,3,halte_2[pilih]);      
        xIndoor--;
        if(xIndoor<-(lenStr))xIndoor= 63;
        isPressedd_2();
        }else{
          Indoor.drawString(0,3,halte_2[pilih]);
          isPressedd_2();
        }
        isPressedd_2();
        delay(100);      
        }
  
  }else{
  if(xIndoor<-(lenStr))xIndoor= 63;
  for(int i=63;i>-(lenStr);i--){
        Depan.drawBox(0,0,15,15);
        Depan.drawBox(1,1,14,14);
        //dmd2.drawFilledBox(2,2,13,13,GRAPHICS_OFF);
       // LCD(n,pilih);
        Depan.drawString(5,2,String(nomor));
        if(pilih>halte_2p-3){
          pilih=0;
          kor_1();
        }
        Depan.drawString(21,3,koridor[1] + "_%_" + koridor[0]);
        
        
        if(halte_2[pilih].length()*9>63){
        Indoor.drawString(xIndoor,3,halte_2[pilih]);      
        xIndoor--;
        if(xIndoor<-(lenStr))xIndoor= 63;
        isPressedd();
        }else{
          Indoor.drawString(0,3,halte_2[pilih]);
          isPressedd();
        }
        delay(100);
                
        }
  }
  }
  
  }

void interruptBtnC(){
button_c = digitalRead(BTN_C);

        if(millis() - last_read > 2000) 
        {
          if(button_c==LOW){
          pilih++;  
          btnPress = true;         
          Indoor.clearScreen();
          xIndoor=0;
          lenStr = halte_1[pilih].length()*9;
          LCD(halte_1[pilih],pilih+1);
          
          }
                    
          
          }
     last_read = millis();  
}

void interruptBtnC_2(){
button_c = digitalRead(BTN_C);

        if(millis() - last_read > 2000) 
        {
          if(button_c==LOW){
          pilih++;  
          btnPress = true;         
          Indoor.clearScreen();
          xIndoor=0;
          lenStr = halte_2[pilih].length()*9;
          LCD(halte_2[pilih],pilih+1);
          
          }
                    
          
          }
     last_read = millis();  
}



void sendd(int data)
{
  digitalWrite(CLK, LOW);
  delay(2);
  for (int i=15; i>=0; i--)
  { 
    delayMicroseconds(50);
    if((data>>i)&0x0001 >0)
      {
        digitalWrite(DAT, HIGH);
        //Serial.print(1);
      }
    else
       {
         digitalWrite(DAT, LOW);
        // Serial.print(0);
       }
    delayMicroseconds(50);
    digitalWrite(CLK, HIGH);
    delayMicroseconds(50);
    
    if(i>0)
    digitalWrite(DAT, LOW);
    else
    digitalWrite(DAT, HIGH);
    delayMicroseconds(50);
    
    if(i>0)
    digitalWrite(CLK, LOW);
    else
    digitalWrite(CLK, HIGH);
  }
  
  delay(20); 
}

void runTest(){
}
