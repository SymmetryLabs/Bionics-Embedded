#define MAX_TEXT_LENGTH 32
char text[MAX_TEXT_LENGTH] = "";

char name1[] = "UnitParamReport";

char name2[] = "UnitReport";

char name3[] = "UnitControl";


void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("BEGIN");
  delay(1000);
  
  for ( int i = 0; i < sizeof(name1); i++ ) text[i] = name1[i];
  whichName();
  memset(&text[0], 0, sizeof(text));
  
  for ( int i = 0; i < sizeof(name1); i++ ) text[i] = name3[i];
  whichName();
  memset(&text[0], 0, sizeof(text));
  
  for ( int i = 0; i < sizeof(name1); i++ ) text[i] = name2[i];
  whichName();
  memset(&text[0], 0, sizeof(text));
  
}


void loop() {
}


void whichName() {
  if ( strcmp(text,name1) == 0 ) Serial.println("name1!");
  if ( strcmp(text,name2) == 0 ) Serial.println("name2!");
  if ( strcmp(text,name3) == 0 ) Serial.println("name3!");
}
