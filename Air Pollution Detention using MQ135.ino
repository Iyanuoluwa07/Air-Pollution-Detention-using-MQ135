#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
const char* ssid = "The World";
const char* password = "qwertyuiop";
// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
String msg_apiKey = "171229";              //Add your Token number that bot has sent you on signal messenger
String phone_number = "+2348150289632"; //Add your signal app registered phone number (same number that bot send you in url)
String msg_content;
String url;                            //url String will be used to store the final generated URL

// Service API Key
String apiKey = "1QJW8K6U7UJF4JDN";
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Set timer to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Timer set to 0 seconds (1000)
unsigned long timerDelay = 1000;
int MQ135_Gas_Pin = 36;
int MQ135_Gas_Value;
int buzzerPin = 18;
float anaValue;
void setup() {
Serial.begin(115200);
lcd.begin(); // initialize the lcd 
lcd.backlight();
pinMode(MQ135_Gas_Pin, INPUT);
pinMode(buzzerPin,LOW);
lcd.setCursor(0,0);
lcd.print(" Air  Pollution");
lcd.setCursor(0,1);
lcd.print("   Monitoring");
delay(1000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("and Alert System");
lcd.setCursor(0,1);
lcd.print("     Done By");
delay(1000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print(" Motun, Iheanyi");
lcd.setCursor(0,1);
lcd.print("  and Adedoyin");
delay(1000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Getting Gas Conc");
lcd.setCursor(0,1);
lcd.print("in the Atmos....");
WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
delay(500);
}

void loop() {
lcd.clear();
MQ135_Gas_Value = analogRead(MQ135_Gas_Pin);
Serial.print("Air Quality:");
anaValue = (MQ135_Gas_Value * 1024.) / 4095.;
Serial.println(anaValue);
lcd.setCursor(0,0);
lcd.print("Air Quality:");
lcd.setCursor(12,0);
lcd.print(anaValue,0);
gasConc_level();
//Send an HTTP POST request every 10 seconds
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "api_key=" + apiKey + "&field1=" + String(anaValue);           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
       // use message_to_signal function to send your own message
      if (anaValue <= 100){
        msg_content = "Fresh Air: " + String(anaValue);
      }
      else if (anaValue > 100 && anaValue <= 200){
        msg_content = "Moderate: " + String(anaValue);
      }
      else{
        msg_content = "Unhealty: " + String(anaValue);
  }
      message_to_signal(msg_content);  // text message content
      
      /*
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // JSON data to send with HTTP POST
      String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(random(40)) + "\"}";           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);*/
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
delay(1000);
}

void gasConc_level(){
  if (anaValue <= 100){
    lcd.setCursor(0,1);
    lcd.print("Fresh Air");
  }
  else if (anaValue > 100 && anaValue <= 200){
    lcd.setCursor(0,1);
    lcd.print("Moderate");
  }
  else{
    lcd.setCursor(0,1);
    lcd.print("DANGER!!!");
    digitalWrite(buzzerPin,HIGH);
    delay(300);
    digitalWrite(buzzerPin,LOW);
  }
}

void  message_to_signal(String message)       // user define function to send meassage to Signal app
{
  //adding all number, your api key, your message into one complete url
  url = "https://api.callmebot.com/signal/send.php?phone=" + phone_number + "&apikey=" + msg_apiKey + "&text=" + urlencode(message);

  postData(); // calling postData to run the above-generated url once so that you will receive a message.
}

void postData()     //userDefine function used to call api(POST data)
{
  int httpCode;     // variable used to get the responce http code after calling api
  HTTPClient http;  // Declare object of class HTTPClient
  http.begin(url);  // begin the HTTPClient object with generated url
  httpCode = http.POST(url); // Finaly Post the URL with this function and it will store the http code
  if (httpCode == 200)      // Check if the responce http code is 200
  {
    Serial.println("Sent ok."); // print message sent ok message
  }
  else                      // if response HTTP code is not 200 it means there is some error.
  {
    Serial.println("Error."); // print error message.
  }
  http.end();          // After calling API end the HTTP client object.
}

String urlencode(String str)  // Function used for encoding the url
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}
