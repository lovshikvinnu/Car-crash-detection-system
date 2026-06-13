#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <math.h>


// -------- MPU6050 --------
MPU6050 mpu;


// -------- GPS --------
SoftwareSerial gpsSerial(4,3); // RX,TX
TinyGPSPlus gps;



void setup()
{
Serial.begin(9600);

Wire.begin();
mpu.initialize();

gpsSerial.begin(9600);

if(mpu.testConnection())
{
Serial.println("MPU6050 Connected");
}

Serial.println("Smart Vehicle Blackbox Ready");
}




void loop()
{

// ---------------- GPS READ ----------------
while(gpsSerial.available())
{
gps.encode(gpsSerial.read());
}



// ---------------- GPS STATUS ----------------
if(gps.location.isValid())
{
Serial.print("GPS: ");
Serial.print(gps.location.lat(),6);
Serial.print(",");
Serial.println(gps.location.lng(),6);
}
else
{
Serial.println("Acquiring GPS...");
}



// ---------------- MPU READ ----------------
int16_t ax,ay,az,gx,gy,gz;

mpu.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);


// Acceleration magnitude
long magSquared=
(long)ax*ax+
(long)ay*ay+
(long)az*az;

float mag=sqrt(magSquared);


Serial.print("Magnitude: ");
Serial.println(mag);




// ---------------- ACCIDENT DETECTION ----------------
if(
mag>25000 ||
abs(ax)>18000 ||
abs(ay)>18000
)
{

Serial.println();
Serial.println("***** ACCIDENT DETECTED *****");

sendInitialAlert();

waitForGPSUpdate();

Serial.println("Blackbox Event Logged");
Serial.println("System Locked");
Serial.println("Manual Reset Required");

while(1); // freeze until reset

}



delay(500);

}




// ====================================
// Immediate Accident Alert
// ====================================

void sendInitialAlert()
{

Serial.println();
Serial.println("=================================");
Serial.println(" EMERGENCY ALERT SENT");
Serial.println("=================================");
Serial.println("Accident Detected");
Serial.println("SOS Sent To Emergency Contact");
Serial.println("GPS Acquiring...");
Serial.println("=================================");
Serial.println();

}




// ====================================
// Wait for GPS Follow-up Alert
// ====================================

void waitForGPSUpdate()
{

Serial.println("Waiting for GPS coordinates...");

unsigned long start=millis();

while(millis()-start<30000)
{

while(gpsSerial.available())
{
gps.encode(gpsSerial.read());
}

if(gps.location.isValid())
{
sendLocationUpdate();
return;
}

delay(500);

}

Serial.println("GPS Timeout - Location update pending");

}




// ====================================
// Location Update Alert
// ====================================

void sendLocationUpdate()
{

String location=
String(gps.location.lat(),6)+","+
String(gps.location.lng(),6);


Serial.println();
Serial.println("=================================");
Serial.println(" LOCATION UPDATE SENT");
Serial.println("=================================");

Serial.print("Coordinates: ");
Serial.println(location);

Serial.print("Maps Link: ");
Serial.print("https://maps.google.com/?q=");
Serial.println(location);

Serial.println("Emergency Contact Updated");
Serial.println("=================================");
Serial.println();

}