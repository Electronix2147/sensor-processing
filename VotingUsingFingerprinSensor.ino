#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint16_t id;
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the kpd 
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

unsigned long loopCount;
unsigned long startTime;
String msg;
int l=0;
char a[3];
int store[10];
int spos=-1;
char cast_to=NO_KEY;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    loopCount = 0;
    startTime = millis();
    msg = "";   
    while (!Serial);  // For Yun/Leo/Micro/Zero/...
    delay(100);
    // set the data rate for the sensor serial port
    finger.begin(57600);
  
    if (finger.verifyPassword()){
      Serial.println("Found fingerprint sensor!");
    } 
    else{
      Serial.println("Did not find fingerprint sensor :(");
      while (1){
        delay(1);
        }
    }
}
int readnumber(){
   int num = 0,t=0;
   char key=NO_KEY;
   do{
    key = kpd.getKey();}
    while(key==NO_KEY);
    while(t<3)
    {
      switch (key)
      {
         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            num = num * 10 + (key - '0');
            break;

         case '*':
            num = 0;
            break;
      }
      t++;
      do{key = kpd.getKey();}
      while(key==NO_KEY);
    }
    if(t==3){
      Serial.println(num);
      }
    return num;
   }
uint16_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
    
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}

int getFingerprintIDez() {
  int temp=0;
  a:
  uint16_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  goto a;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK){Serial.println("No Match Found!");  return -1;}
  
  // found a match!
  if(finger.fingerID){
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  //Serial.print(" with confidence of "); Serial.println(finger.confidence);
  temp=1;}
  if(temp==1){
    return finger.fingerID;
  }
  else{
    goto a;
  }
}

 uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}
  
void loop() {
  // put your main code here, to run repeatedly:
  char what=NO_KEY;
  int check,vote=1;
  Serial.println("* for New,# for checking,0 for deleting");
  do{
    what=kpd.getKey();
  }
  while(what==NO_KEY);
  switch(what){
    case '*':
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID #you want to save this finger as...");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
    return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);
  while (!getFingerprintEnroll() );
  break;
  case '#':
  Serial.println("Place Finger on Sensor");
  check=getFingerprintIDez();
  delay(50);
  for(int i=0;i<=spos;i++)
  {
    if(check==store[i])
    {
      Serial.println("This person has already casted vote!");
      vote=-1; 
    }
  }
  if(vote==1)
  {
    Serial.println("Allowed to Vote");
    store[++spos]=check;
    Serial.println("You are now going to cast your vote!");
    Serial.println("Press the Serial Number infront of the party name to vote for their candidate!")
    Serial.println("For more assistance on how to vote, please contact officer on duty!")
    Serial.println("Vote for Bhartiya Janta Party");
    Serial.println("Vote for Congress");
    Serial.println("Vote for Aam Aadmi Party");
    Serial.println("Vote for Samajvadi Party");
    Serial.println("Vote for None of The Above");
    here:
    do{
      cast_to=kpd.getKey();
    }
    while(cast_to==NO_KEY)
    switch(cast_to)
    {
      case '1': Serial.println("You voted for Bhartiya Janta Party");break;
      case '2': Serial.println("You voted for Congress");break;
      case '3': Serial.println("You voted for Aam Aadmi Party");break;
      case '4': Serial.println("You voted for Samajvadi Party");break;
      case '5': Serial.println("You voted for None of The Above");break;
      default : Serial.println("You opted wrong option. Press a valid option!Try Again!");goto here;break;
    }
    
  }
  vote=1;
  break;
  case '0':
    Serial.println("Please type in the ID # (from 100 to 999) you want to delete...");
  uint8_t id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
     break;
  }

  Serial.print("Deleting ID #");
  Serial.println(id);
  
  deleteFingerprint(id);
}
}
