#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     0
#define UTC_OFFSET_DST 0

//pin configurations
#define Buzzer 5
#define LED_1 15
#define LED_2 13
#define PB_CANCEL 34
#define PB_OK 32
#define PB_UP 33
#define PB_DOWN 35
#define DHTPIN 12

//declare objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHTesp dhtSensor;

//globale variables
int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

long utc_offset_seconds = 0;//initial offset is zero in seconds

unsigned long timeNow = 0;
unsigned long timeLast = 0;


//alarm parameters
bool alarm_enabled = true;
int n_alarms = 3;
int alarm_hours[] = {0,0,0};
int alarm_minutes[] = {0, 0, 0};
bool alarm_triggered[] = {false, false, false};

int n_notes = 8;
int C = 292;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int notes[] = {C, D, E, F, G, A, B, C_H};

int current_mode = 0;
int max_modes = 5;
String modes[] = {"1 - Set Time Zone", "2 - Set Alarm 1", "3 - Set Alarm 2", "4 - Set Alarm 3", "5 - Disable Alarms"};



void setup() {
  //pin configurations
  pinMode(Buzzer, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(PB_CANCEL, INPUT);
  pinMode(PB_OK, INPUT);
  pinMode(PB_DOWN, INPUT);
  pinMode(PB_UP, INPUT);
  
  dhtSensor.setup(DHTPIN, DHTesp::DHT22); //initialize and configure the DHT sensor

  Serial.begin(9600);
  if(! display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); //initialize the display
  }  

  display.display();
  delay(500);

  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WIFI",0,0,2);
    
  }

  display.clearDisplay();
  print_line("Connected to WIFI",0,0,2);
  delay(1000);

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);//synchronizing time with NTP server
  

  display.clearDisplay();
  print_line("Welcome to Medibox", 10, 20 ,2);
  delay(500);
  display.clearDisplay();

}


void loop() {
  update_time_with_check_alarm(); //Medibox checks the time and sees if any alarms are enabled

  if(digitalRead(PB_OK) == LOW){
    delay(200);
    go_to_menu(); //if Ok button is pressed it will call go_to_menu function
  }

  check_temp(); //Regularly checks the temperature and displays warnings if needed

}


// Function to print text on the OLED display
//This function allows display text at any location on the OLED display with the desired size and color
void print_line (String text, int column, int row, int text_size){

  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column,row);
  display.println(text);
  
  display.display(); //update the display and make the changes visible on the screen
  

}

//This function is responsible for displaying the current time in a formatted way on the Medibox's OLED screen
//During setup, the code calls configTime to synchronize time with a time server specified by NTP_SERVER
//This retrieves the current accurate time from the internet and adjusts the internal time variables based on the retrieved values
void print_time_now(void){
  display.clearDisplay(); 
  delay(100);
  print_line(String(days), 0, 0, 2);
  print_line(":", 20, 0, 2);
  print_line(String(hours), 30, 0, 2);
  print_line(":", 50, 0, 2);
  print_line(String(minutes), 60, 0, 2);
  print_line(":", 80, 0, 2);
  print_line(String(seconds), 90, 0, 2);
  
}

//retrieves the current time and updates internal variables (Day, Hour, Minute, Second)
void update_time(){
  struct tm timeinfo;
  getLocalTime(&timeinfo); //This retrieves the current local time and stores the information in the timeinfo structure

  char timeHour[3];
  strftime(timeHour,3,"%H",&timeinfo);
  hours = atoi(timeHour);

  char timeMinute[3];
  strftime(timeMinute,3,"%M",&timeinfo);
  minutes = atoi(timeMinute);

  char timeSecond[3];
  strftime(timeSecond,3,"%S",&timeinfo);
  seconds = atoi(timeSecond);

  char timeDay[3];
  strftime(timeDay,3,"%D",&timeinfo);
  days = atoi(timeDay);

}

//Sounds the alarm on the Medibox when alarm time is reached
void ring_alarm(){

  display.clearDisplay();
  print_line("Medicine Time", 0, 0, 2);
  digitalWrite(LED_1, HIGH); //Turns on the first LED

  bool break_happened = false; //Initializes a flag break_happened to check if the alarm is cancelled or not
  
  //loop is continue until alarm not cancelled and cancel button is not pressed
  while(break_happened == false && digitalRead(PB_CANCEL) == HIGH){
    for(int i =0; i<=n_notes; i++){
      if(digitalRead(PB_CANCEL)==LOW){
        delay(200);
        break_happened = true; //sets the flag to true, indicating the alarm is cancelled
        break; 
      }
      tone(Buzzer, notes[i]);
      delay(500);
      noTone(Buzzer);
      delay(2);
    }
  }
  
  digitalWrite(LED_1,LOW);
  display.clearDisplay();


}

//This function checks each set alarm time (alarm_hours and alarm_minutes) against the current time. 
//If the current time matches an alarm time and the alarm_enabled flag is true, then the alarm will trigger.
void update_time_with_check_alarm(void){
  update_time();
  print_time_now();
  
  if(alarm_enabled == true){
    for(int i = 0 ; i<n_alarms; i++ ){
      if(alarm_triggered[i] == false && alarm_hours[i] == hours && alarm_minutes[i] == minutes){
        ring_alarm();
        alarm_triggered[i] = true;
      }  
    }
  }

}

//This function continuously checks for button presses and returns the ID of the pressed button
int wait_for_button_press(){
  while(true){
    if(digitalRead(PB_UP) == LOW){
      delay(200);
      return PB_UP;
    }

    else if(digitalRead(PB_DOWN) == LOW){
      delay(200);
      return PB_DOWN;
    }

    else if(digitalRead(PB_CANCEL) == LOW){
      delay(200);
      return PB_CANCEL;
    }

    else if(digitalRead(PB_OK) == LOW){
      delay(200);
      return PB_OK;
    }

    update_time();
  }
}


//This code shows the menu system
//it displays the current menu option, waits for button presses (Up, Down, OK, Cancel), and takes actions based on the pressed button and selected mode.
void go_to_menu(){
  while(digitalRead(PB_CANCEL) == HIGH){
    display.clearDisplay();
    print_line(modes[current_mode],0,0,2);

    int pressed = wait_for_button_press();
    if(pressed == PB_UP){
      delay(200);
      current_mode += 1;
      current_mode = current_mode % max_modes;
    }

    else if(pressed == PB_DOWN){
      delay(200);
      current_mode -= 1;
      if(current_mode < 0 ){
        current_mode = max_modes -1;
      }
    }

    else if(pressed == PB_OK){
      delay(200);
      run_mode(current_mode); //call run_mode function
    }

    else if(pressed == PB_CANCEL){
      delay(200);
      break;
    }

  }
  
} 

void run_mode(int mode){
  if(mode == 0){
    timezone_set();
  }

  //Modes 1, 2, and 3 call set_alarm to configure individual alarms
  else if (mode == 1 || mode == 2 || mode == 3){
    set_alarm(mode - 1);

  }
  //Mode 4 disables all alarms and displays a confirmation message.
  else if(mode == 4){
    alarm_enabled = false;
    display.clearDisplay();
    print_line("Alarms are  disabled", 0, 0, 2);
    delay(1000);
  }
}

//This function allows  to set the time for a specific alarm using Up/Down buttons and confirms it with the OK button, also letting cancel the process with the Cancel button.
void set_alarm(int alarm) {
  alarm_enabled = true; //make elarm_enabled true again

  int temp_hour = alarm_hours[alarm];
  int temp_minute = alarm_minutes[alarm];

  while (true) {
    display.clearDisplay();
    print_line("Enter hour: " + String(temp_hour), 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == PB_UP) {
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;
    } else if (pressed == PB_DOWN) {
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0) {
        temp_hour = 23;
      }
    } else if (pressed == PB_OK) {
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
    } else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }
  }

  while (true) {
    display.clearDisplay();
    print_line("Enter minute: " + String(temp_minute), 0, 0, 2);

    int pressed = wait_for_button_press();
    if (pressed == PB_UP) {
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;

    } else if (pressed == PB_DOWN) {
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0) {
        temp_minute = 59;
      }

    } else if (pressed == PB_OK) {
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      alarm_triggered[alarm] = false; // Reset the alarm trigger status
      break;

    } else if (pressed == PB_CANCEL) {
      delay(200);
      break;
    }
  }

  display.clearDisplay();
  print_line("Alarm is set", 0, 0, 2);
  delay(1000);
}



void timezone_set(){
  int current_offset_hours = utc_offset_seconds/3600; //initial offset hours
  int current_offset_minutes = (utc_offset_seconds%3600)/60; //initial offset minutes

  int temp_hour = current_offset_hours;
  int temp_minute = current_offset_minutes;

  while(true){
    display.clearDisplay();
    print_line("Enter offset hours: "+ String(temp_hour),0,0,2);
 
    int pressed = wait_for_button_press();
    if(pressed == PB_UP){
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 15; //since maximum offset hours present is +14

    }

    else if(pressed == PB_DOWN){
      delay(200);
      temp_hour -= 1;
      temp_hour = temp_hour % 13; //since maximum offset hours present is -12
    }


    else if(pressed == PB_OK){
      delay(200);
      current_offset_hours = temp_hour;
      break;
    }

    else if(pressed == PB_CANCEL){
      delay(200);
      break;
    }

  }


  while(true){
    display.clearDisplay();
    print_line("Enter offset minutes: "+String(temp_minute),0,0,2);

    int pressed = wait_for_button_press();
    if(pressed == PB_UP){
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;

    }

    else if(pressed == PB_DOWN){
      delay(200);
      temp_minute -= 1;
      if(temp_minute < 0 ){
        temp_minute = 59;
      }
    }


    else if(pressed == PB_OK){
      delay(200);
      current_offset_minutes = temp_minute;
      break;
    }

    else if(pressed == PB_CANCEL){
      delay(200);
      break;
    }
    
  }

  utc_offset_seconds = current_offset_hours * 3600 + current_offset_minutes * 60;
  configTime(utc_offset_seconds, UTC_OFFSET_DST, NTP_SERVER);

  display.clearDisplay();
  print_line("Timezone set to " + String(current_offset_hours) + ":" + String(current_offset_minutes), 0, 0, 2);
  delay(2000);
}

void check_temp(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  if(data.temperature>32){
    display.clearDisplay();
    digitalWrite(LED_2, HIGH);
    print_line("TEMP HIGH", 0, 40,1);
    delay(1000);
  }
  if(data.temperature<26){
    display.clearDisplay();
    print_line("TEMP LOW", 0, 40,1);
    digitalWrite(LED_2, HIGH);
    delay(1000);
  }
  if(data.humidity>80){
    display.clearDisplay();
    print_line("HUMIDITY HIGH", 0, 50,1);
    digitalWrite(LED_2, HIGH);
    delay(1000);
  }
  if(data.humidity<60){
    display.clearDisplay();
    print_line("HUMIDITY LOW", 0, 40,1);
    digitalWrite(LED_2, HIGH);
    delay(1000);
  }

  if(60<data.humidity<80 && 26<data.temperature<32){
    digitalWrite(LED_2, LOW);
  }
}









  