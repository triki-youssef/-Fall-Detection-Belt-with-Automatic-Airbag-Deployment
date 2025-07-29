<h1>🛡️ Fall Detection Airbag Belt </h1>

<h2> - ✅ Description : </h2>

The Fall Detection Airbag Belt is an intelligent wearable safety device designed to detect various types of human falls in real-time and respond instantly by deploying protective airbags. It operates using the Arduino Nano 33 BLE Sense Rev2, which features a built-in 9-axis motion sensor (BMI270 + BMM150 IMU) capable of measuring acceleration and rotational velocity across all axes. Upon powering the system, a calibration phase begins, during which the device records baseline acceleration and gyroscope values over a 3-second window while the user stands still. These values are used to filter out sensor noise and adapt to the user’s posture. After calibration, the device enters a monitoring state where it continuously reads acceleration and gyroscope data at high frequency. The fall detection algorithm is based on a state machine that identifies suspicious patterns such as a sudden vertical drop (collapse), a backward tilt (as seen during fainting), or a general tilt combined with rapid body rotation (e.g., from tripping or falling off a chair). When any of these abnormal movements are detected, the system transitions into a “potential fall” state and begins a short verification process. If further signs of a fall are confirmed — such as a brief free-fall (Z-acceleration below 0.2g) or a strong impact (Z-acceleration above 2.5g) — the system immediately classifies the event as a confirmed fall. In response, it activates two motors connected to mechanical linkages that pull open two manual CO₂ valves, rapidly inflating airbags positioned on both sides of the user's body. This deployment cushions the fall and helps prevent serious injury. The entire process is automatic, fast, and designed to avoid false positives by requiring multiple fall conditions to be met before triggering the airbags. Its dual-sensor approach and multi-stage confirmation process make it a reliable safety solution for elderly individuals, people with epilepsy, or anyone at risk of falling. 
<h2>🧠 Detection Logic: How It Works</h2>
<h3>✅ Calibration Phase </h3>
- First 3 seconds after startup:

     - IMU readings are averaged to determine:
     
    -  Baseline gravity vector (Z-axis ≈ 1g)

     - Gyroscope drift

- These values are stored to reduce noise and false positives.


<h2>📉 Fall Types Detected : </h2>
<h3>Vertical Collapse (e.g. slipping, sudden drop) : </h3>

- Z-acceleration (acZ) drops below 0.8g.

- Suggests downward free-fall or loss of support.

- Logs: "Collapse detected!"

<h3>Backward Tilt (e.g. fainting backward) : </h3>

- Y-acceleration (acY) increases above 0.15g.

- Indicates backward lean/faint with gravity shift.

- Logs: "Backward tilt detected!"

<h3>General Tilt + Rotation (e.g. turning + falling sideways)</h3>

- Either X or Y tilt above 0.3g combined with gyroscope magnitude > 150°/s.

- Indicates user is turning/spinning during tilt (falling off stairs, chair, etc.).

- Logs: "Tilt+rotation detected!"

<h3> 🔄 Fall Confirmation Phase </h3>
<h4>Once in POTENTIAL_FALL state:</h4>




Free-fall	 -------->  Z-axis < 0.2g	 ------------>  Trigger Airbag


Impact -------->  Z-axis > 2.5g	 --------> Trigger Airbag


Timeout  -------->   No confirmation in 1 second  -------->  Return to STABLE


This two-stage filtering prevents false triggers from gestures or sitting down.



<h2>💥 Airbag Deployment</h2>
<h3>When confirmed:</h3>

- AIRBAG_PIN1 = HIGH, AIRBAG_PIN2 = LOW

- Both motors are turned ON simultaneously

- Motors mechanically pull the valves open

- CO₂ inflates two airbags

- Message printed: "AIRBAG DEPLOYED!"

- To stop the motor, deployment time is limited to 50ms.




<h2>🧰 Hardware Summary</h2>

- Arduino Nano 33 BLE Sense Rev2	Senses movement, runs algorithm

  
- BMI270 IMU :	Measures acceleration & gyroscope


- 2× Motors :	Mechanically open CO₂ valves (25GA-370 6V 280 RPM DC)


- 2× Manual CO₂ Valves :	Releases gas into airbags


- 2× Airbags :	Deployed on each side


- Motor Driver : Controls direction and speed (Dual Channel Motor Driver 5A 3V–14V)


- 11.1V, 2500mAh Battery	: Powers full system

  

- Switch ON/OFF


- 2 x CO2 Cartridge 16mg



<h2>📸  Media : </h2> 

<p align="center">
<br/>
<img src="https://i.imgur.com/SLZtCrd.jpeg" height="60%" width="60%" alt="Disk Sanitization Steps"/>

<br />
<br />


<h3>📦 You will find the <h3>demo video 🎬, prototype photos 📸, wiring diagram 🔌, and Arduino code 💻 in the project files.</h3>


