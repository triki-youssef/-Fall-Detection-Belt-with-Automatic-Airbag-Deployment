#include <Arduino_BMI270_BMM150.h>

#define AIRBAG_PIN1 2
#define AIRBAG_PIN2 3
#define AIRBAG2_PIN1 4
#define AIRBAG2_PIN2 5
#define CALIBRATION_TIME 3000

// Thresholds for all detection modes 
#define COLLAPSE_THRESHOLD 0.8f     // Vertical free-fall (g)
#define SLOW_FALL_TILT 0.15f        // Backward tilt (g)
#define TILT_THRESHOLD 0.3f         // General tilt (any direction) (g)
#define ROTATION_THRESHOLD 150.0f   // Rotation confirmation (°/s)
#define FREE_FALL_THRESHOLD 0.2f    // Free-fall confirmation (g)
#define IMPACT_THRESHOLD 2.5f       // Impact detection (g)

// Calibration data
float calibAcX = 0, calibAcY = 0, calibAcZ = 1.0;
float calibGyX = 0, calibGyY = 0, calibGyZ = 0;
unsigned long fallStartTime = 0;

enum FallState { CALIBRATING, STABLE, POTENTIAL_FALL, CONFIRMED_FALL };
FallState fallState = CALIBRATING;

void setup() {
   
    Serial.begin(115200);
   pinMode(AIRBAG_PIN1, OUTPUT);
   pinMode(AIRBAG_PIN2, OUTPUT);
   pinMode(AIRBAG2_PIN1, OUTPUT);
   pinMode(AIRBAG2_PIN2, OUTPUT);
    
    if (!IMU.begin()) {
        Serial.println("IMU Initialization Failed!");
        while (1);
    }
    Serial.println("Calibrating... Stand normally for 3 seconds");
}

void calibrateSensors() {
    float sumAcX = 0, sumAcY = 0, sumAcZ = 0;
    float sumGyX = 0, sumGyY = 0, sumGyZ = 0;
    int samples = 0;
    
    unsigned long startTime = millis();
    while (millis() - startTime < CALIBRATION_TIME) {
        if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
            float acX, acY, acZ, gyX, gyY, gyZ;
            IMU.readAcceleration(acX, acY, acZ);
            IMU.readGyroscope(gyX, gyY, gyZ);
            
            sumAcX += acX; sumAcY += acY; sumAcZ += acZ;
            sumGyX += gyX; sumGyY += gyY; sumGyZ += gyZ;
            samples++;
        }
        delay(10);
    }
    
    calibAcX = sumAcX / samples;
    calibAcY = sumAcY / samples;
    calibAcZ = (sumAcZ / samples) - 1.0; // Remove gravity
    calibGyX = sumGyX / samples;
    calibGyY = sumGyY / samples;
    calibGyZ = sumGyZ / samples;
    
    Serial.println("Calibration complete!");
}

void triggerAirbag() {
    digitalWrite(AIRBAG_PIN1, HIGH);
    digitalWrite(AIRBAG_PIN2, LOW);
     digitalWrite(AIRBAG2_PIN1, HIGH);
    digitalWrite(AIRBAG2_PIN2, LOW);
    delay(50); // Deployment pulse
     digitalWrite(AIRBAG_PIN1, HIGH);
    digitalWrite(AIRBAG_PIN2, LOW);
    digitalWrite(AIRBAG2_PIN1, HIGH);
    digitalWrite(AIRBAG2_PIN2, LOW);
    fallState = CONFIRMED_FALL;
    Serial.println("AIRBAG DEPLOYED!");
}

void loop() {
    float acX, acY, acZ, gyX, gyY, gyZ;
    
    if (fallState == CALIBRATING) {
        calibrateSensors();
        fallState = STABLE;
        return;
    }

    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
        IMU.readAcceleration(acX, acY, acZ);
        IMU.readGyroscope(gyX, gyY, gyZ);
        
        // Apply calibration
        float relX = acX - calibAcX;
        float relY = acY - calibAcY;
        float relZ = (acZ - calibAcZ) + 1.0; // Restore gravity
        float rotation = sqrt(gyX*gyX + gyY*gyY + gyZ*gyZ);

        switch(fallState) {
            case STABLE:
                // 1. Vertical collapse detection
                if (relZ < COLLAPSE_THRESHOLD) {
                    fallStartTime = millis();
                    fallState = POTENTIAL_FALL;
                    Serial.println("Collapse detected!");
                }
                // 2. Backward tilt detection
                else if (relY > SLOW_FALL_TILT) {
                    fallStartTime = millis();
                    fallState = POTENTIAL_FALL;
                    Serial.println("Backward tilt detected!");
                }
                // 3. General tilt + rotation detection
                else if ((abs(relX) > TILT_THRESHOLD || abs(relY) > TILT_THRESHOLD) && 
                         rotation > ROTATION_THRESHOLD) {
                    fallStartTime = millis();
                    fallState = POTENTIAL_FALL;
                    Serial.println("Tilt+rotation detected!");
                }
                break;
                
            case POTENTIAL_FALL:
                // Confirm with free-fall or impact
                if (relZ < FREE_FALL_THRESHOLD) {
                    triggerAirbag();
                } 
                else if (relZ > IMPACT_THRESHOLD) {
                    triggerAirbag();
                }
                // Timeout after 1 second
                else if (millis() - fallStartTime > 1000) {
                    fallState = STABLE;
                    Serial.println("Fall not confirmed");
                }
                break;
                
            case CONFIRMED_FALL:
                // System reset logic would go here
                break;
        }
    }
} 