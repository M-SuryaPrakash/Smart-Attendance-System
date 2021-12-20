#include<RfidDictionaryView.h>
#include<AWS_IOT.h>
#include<WiFi.h>

// Rfid RC522 setup
#define SDA_Pin 21
#define RST_Pin 22

int startBlock = 10;                         
RfidDictionaryView rfid(SDA_Pin, RST_Pin, startBlock);

// led pins
#define yellow 15
#define blue 2
#define green 4
#define red 5

// wifi credentials
char* ssid="vivo 1904";
char* password="Vivo@1234";

// creating instance of AWS_IOT library and thing details
AWS_IOT aws;

char* HOST_ADDRESS="a3e4kg1mwrwqqt-ats.iot.ap-south-1.amazonaws.com";
char* CLIENT_ID="ESP32";
char* TOPIC_NAME="$aws/things/ESP32/shadow/name/Smart-Attendance-System/update";

// Payload array to store thing shadow JSON document
char payload[512];

// connection Status
int status;

void connect_to_wifi()
{
    WiFi.disconnect(true);
    Serial.print("Connecting to WiFi");
    status=WiFi.begin(ssid,password);
    while(status!=WL_CONNECTED)
    {
        status=WiFi.begin(ssid,password);
        Serial.print(".");
        delay(5000);
    }
    Serial.print(" Connected to WiFi ");Serial.println(ssid);
}

void connect_to_AWS()
{
    Serial.print("\nConnecting to AWS");
    status=aws.connect(HOST_ADDRESS, CLIENT_ID);
    while(status!=0)
    {
        Serial.print(".");
        status=aws.connect(HOST_ADDRESS, CLIENT_ID);
        delay(10000);
    }
    Serial.printf(" Successfully Connected to AWS-IoT thing ====> %s\n\n", CLIENT_ID);
}

String connect_and_read_rfidtag()
{
    bool new_tag=false;

    Serial.println("Waiting for tag.....");

    while(!new_tag)
    {
        new_tag=rfid.detectTag();
        delay(100);
    }

    digitalWrite(yellow, LOW);
    digitalWrite(blue, HIGH);
    delay(10000);

    String id=rfid.get("id");

    rfid.disconnectTag(); // remember to disconnect the tag.

    return id;
}

int publish_to_aws(String id)
{
    sprintf(payload,"{\"state\":{\"reported\":{\"id\":\"%s\"}}}", id);

    Serial.println(payload);

    status = aws.publish(TOPIC_NAME, payload);

    return status;
}

void setup()
{
    Serial.begin(9600);

    connect_to_wifi();
    connect_to_AWS();

    pinMode(yellow, OUTPUT);
    pinMode(blue, OUTPUT);
    pinMode(green, OUTPUT);
    pinMode(red, OUTPUT);

    delay(1000);
}

void loop()
{
    digitalWrite(yellow, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);

    //waiting for tag
    digitalWrite(yellow, HIGH);

    // connecting to rfid tag and reading data from it.
    String id=connect_and_read_rfidtag();

    Serial.println("returned id : "+id);

    status = publish_to_aws(id);

    delay(1000);
    digitalWrite(blue, LOW);

    if(status == 0)
    {
        digitalWrite(green, HIGH);
        Serial.println("Attendance Recorded for id: " + id + "\n");
    } 
    else
    {
        digitalWrite(red, HIGH);
        Serial.println("Attendance Not Recorded for id: "+id+" Try Again!!!!!\n");
    }

    delay(10000);
}
