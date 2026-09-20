AURA – Smart Room Monitoring & Energy Management System 🚀 Overview AURA is a low-cost smart room monitoring system designed to provide real-time room occupancy awareness, automatic energy management, resource monitoring, and enhanced physical security. The system detects room occupancy and activity using ultrasonic sensors and an accelerometer, while monitoring electricity consumption using a current sensor and water usage using a water sensor**. An AI-based anomaly score helps identify unusual patterns in occupancy, electricity, water usage, and room activity. The system displays room status through ambient LEDs, supports automatic control of lighting/HVAC based on occupancy, and detects sensor tampering and unusual activity. AURA is designed for environments such as hospitals, libraries, schools, colleges, offices, hotels, and other shared spaces where energy efficiency, resource conservation, intelligent monitoring, and security are important.

🎯 Problem Statement In many buildings, lighting and HVAC systems continue operating even when rooms are empty because they rely on fixed schedules or manual switching. At the same time, electricity and water usage are often not monitored intelligently, making it difficult to identify unusual consumption or wastage. This results in:

Unnecessary electricity consumption and increased energy costs Avoidable water wastage Increased carbon emissions Lack of real-time room occupancy information Difficulty in identifying empty or partially occupied spaces Limited visibility into electricity and water consumption Difficulty in detecting unusual consumption or activity patterns Limited protection against sensor tampering and unauthorized access

AURA addresses these challenges by combining real-time occupancy monitoring, automatic energy control, electricity and water monitoring, AI-based anomaly scoring, and a security layer into a single low-cost system. This enables smarter resource usage, helps identify unusual patterns, and provides better visibility and security for shared spaces.

💡 Our Solution AURA uses two HC-SR04 ultrasonic sensors to detect room entry and exit with directional accuracy. An MPU-6050 accelerometer monitors vibration and activity levels. A current sensor monitors electricity consumption, while a water sensor tracks water usage. Data from these sensors is combined to understand occupancy, activity, and resource consumption in real time.

The system provides:

Real-time occupancy detection
Electricity and water usage monitoring
Visual room-status indication using LEDs
Automatic lighting/HVAC control
Sensor tamper detection
Unauthorized-access detection
Bluetooth-based monitoring through a companion app
AI-based detection of unusual activity and consumption patterns
AURA also uses z-score-based deviation analysis to compare live sensor readings with a learned baseline and identify unusual patterns in room activity, electricity consumption, and water usage. This helps the system detect abnormal behavior or unexpected resource consumption and generate an AI anomaly score for real-time monitoring.

✨ Key Features 👀 Real-Time Occupancy Detection Two ultrasonic sensors detect movement and determine entry/exit direction, providing accurate occupancy information.

🔴🟡🟢 Ambient Occupancy LED The room status can be identified from outside without entering the room.

🔴 Red → Full Occupancy 🟡 Yellow → Half Occupancy 🟢 Green → Empty / Available This silent visual indication is particularly useful in places such as libraries and hospitals, where audio alarms and unnecessary disturbance should be avoided.

💡 Automatic Energy Control AURA automatically responds to room occupancy. When a room becomes empty, the system can trigger energy-saving actions such as switching lighting/HVAC OFF. When occupancy is detected, the system can enable the required lighting/HVAC. This reduces unnecessary electricity consumption and supports sustainable energy management.

🔐 Cyber Security / Tamper Detection The AURA Cyber module adds a security layer by cross-checking sensor readings to identify:

Sensor blocking Sensor tampering Possible sabotage attempts Unauthorized access 🧠 Anomaly Detection AURA performs lightweight, real-time anomaly detection using z-score-based deviation analysis. Live sensor readings are compared against a learned baseline to identify unusual occupancy or activity patterns.

📱 Bluetooth Monitoring AURA uses an HC-05 Bluetooth module to transmit system status to a companion phone application. The prototype uses local Bluetooth communication rather than cloud connectivity.

🛠️ Technologies Used Embedded System Arduino Nano Arduino C/C++ Firmware Sensors & Hardware 2 × HC-SR04 Ultrasonic Sensors MPU-6050 Accelerometer 0.96" OLED Display HC-05 Bluetooth Module 4 Status LEDs Activity LED 2 Push Buttons Mobile Application MIT App Inventor Machine Learning / Anomaly Detection Z-score-based deviation analysis Lightweight unsupervised anomaly detection ⚙️ System Workflow ┌──────────────────────────┐ │ AURA SYSTEM │ └────────────┬─────────────┘ │ ┌─────────────┴─────────────┐ │ │ ▼ ▼ HC-SR04 Ultrasonic MPU-6050 Sensors Accelerometer │ │ └─────────────┬─────────────┘ │ ▼ Arduino Nano │ ┌────────────┼─────────────┐ │ │ │ ▼ ▼ ▼ Occupancy Activity Anomaly Detection Detection Detection │ │ │ └────────────┼─────────────┘ │ ▼ AURA Decision Logic │ ┌───────────────┼────────────────┐ │ │ │ ▼ ▼ ▼ Status LEDs Energy Control Cyber Module 🔴 🟡 🟢 Lighting/HVAC Tamper/Access │ │ │ └───────────────┼────────────────┘ │ ▼ HC-05 Bluetooth │ ▼ MIT App Inventor App

⚙️ Installation & Setup

Clone the Repository git clone https://github.com/Bhuvaneshwari-0412/AURA/new/main?filename=README.md

Open the Arduino Project Open the AURA Arduino firmware in the Arduino IDE.

Connect the Hardware Connect the following components to the Arduino Nano:

HC-SR04 Ultrasonic Sensor 1 HC-SR04 Ultrasonic Sensor 2 MPU-6050 Accelerometer 0.96" OLED Display HC-05 Bluetooth Module Status LEDs Activity LED Push Buttons 4. Upload the Firmware Select the appropriate:

Board: Arduino Nano Processor: Appropriate Nano processor configuration COM Port: Port connected to the Arduino Upload the AURA firmware to the Arduino Nano.

Configure the Companion App Open the MIT App Inventor project and connect it to the AURA system through the HC-05 Bluetooth module.

Test the System After powering the system:

Verify ultrasonic sensor readings. Verify accelerometer readings. Check occupancy detection. Check Red/Yellow/Green LED status. Test automatic energy-control logic. Test Bluetooth communication. Test tamper and anomaly detection. Note: Exact library requirements, pin configurations, and firmware setup details should be followed from the source code included in this repository.

▶️ How to Use Step 1 – Power the AURA System Power the Arduino Nano and connected sensors.

Step 2 – Detect Occupancy The two ultrasonic sensors monitor movement and entry/exit direction.

Step 3 – Monitor Activity The MPU-6050 accelerometer detects vibration and activity levels.

Step 4 – View Room Status The ambient LEDs provide an immediate visual indication:

🔴 RED → Full Occupancy 🟡 YELLOW → Half Occupancy 🟢 GREEN → Empty / Available The room status can be checked without entering the room.

Step 5 – Save Energy When the room becomes empty or has low activity, AURA can trigger energy-saving actions such as switching lighting/HVAC OFF.

When occupancy is detected, the system can activate the required lighting/HVAC.

Step 6 – Monitor Security The Cyber module cross-checks sensor information and can identify sensor blocking, tampering, or unauthorized access.

Step 7 – Monitor Through Bluetooth The HC-05 module sends system information to the companion mobile application for local monitoring.

🔐 AURA Cyber Module The Cyber module is an additional security layer integrated into AURA.

Unlike a conventional occupancy system that only detects whether a room is occupied, AURA also checks whether its sensing system is behaving normally.

The Cyber module helps detect:

Sensor tampering Blocked sensors Possible sabotage Unauthorized access Unusual activity patterns The system uses sensor cross-validation and anomaly detection to identify suspicious conditions. This makes AURA more than an energy-saving system — it becomes a smart room monitoring and security solution.

🌱 Sustainability AURA supports sustainable energy management by reducing unnecessary lighting and HVAC operation. Instead of keeping systems continuously active based on fixed schedules, AURA uses real-time occupancy information to make energy-saving decisions.

Sustainability Benefits Reduces electricity wastage Prevents lights from remaining ON in empty rooms Supports efficient HVAC usage Reduces unnecessary energy costs Supports green-building objectives Enables smarter utilization of shared spaces 📸 Screenshots & Prototype AURA Hardware Prototype AURA Hardware Prototype

Suggested images:

AURA hardware prototype Arduino Nano and sensor connections Red LED – Full Occupancy Yellow LED – Half Occupancy Green LED – Empty Room OLED display MIT App Inventor companion application Bluetooth communication Cyber / anomaly detection demonstration 🏥 Potential Applications AURA can be adapted for multiple real-world environments.

🏥 Hospitals Provides silent room-status monitoring and helps manage lighting/HVAC without disturbing patients.

📚 Libraries Allows users to check room availability without entering and creating unnecessary disturbance.

🏫 Schools & Colleges Can be used for classrooms, laboratories, seminar halls, and other shared spaces.

🏢 Offices Helps manage meeting rooms and workspaces while reducing unnecessary energy consumption.

🏨 Hotels Can support room occupancy monitoring and energy-efficient lighting/HVAC control.

🔒 Security-Sensitive Areas The Cyber module can be useful in laboratories, server rooms, restricted areas, and other spaces where tamper or unauthorized-access detection is important.

📈 Scalability AURA is designed to scale from a single-room prototype to multi-room and building-level deployment.

Current Stage Working prototype Occupancy sensing Ambient status LEDs Activity monitoring Anomaly detection Bluetooth communication Future Scaling Single Room ↓ Multiple Rooms ↓ Building-Level Deployment ↓ Centralized Monitoring Dashboard ↓ Smart Building Management

The low-cost and retrofit-friendly hardware approach allows AURA to be adapted to existing facilities.

🔮 Future Enhancements 📱 Mobile application for remote room-status monitoring 🧠 AI-based occupancy pattern prediction 🔐 Expanded Cyber module with security and access logs ☁️ IoT/cloud connectivity for centralized multi-room monitoring 🏢 Integration with smart-building and smart-city systems 📊 Centralized facility-management dashboard 🏫 Pilot deployment in campus spaces such as libraries and hostels ⚡ Expanded smart control of lighting and HVAC systems 📈 Collection of real-world usage data for system refinement 🗺️ Development Roadmap Current – Working Prototype Occupancy sensing, ambient LEDs, activity monitoring, and anomaly detection at the breadboard stage.

🌍 Long-Term Impact AURA aims to create spaces that are:

🌱 Energy-efficient 🤫 Silent and disturbance-free 🔒 Secure 🧠 Intelligent ♻️ Sustainable By combining occupancy awareness, automatic energy management, and Cyber-based security monitoring, AURA can help institutions reduce wasted electricity while improving room visibility, convenience, and physical security.

👥 Team Team Name SMART MINDS

Team Members Aishwarya S – Team Leader Divya Dharshini V Aarthi I Bhuvaneshwari T Institution R.M.K Engineering College

Hackathon NexBuildOn Hack 2026 – Innovating for Real-World Impact

📌 Project Summary AURA – Smart Room Monitoring & Energy Management System

Silent occupancy awareness + Automatic energy saving + Cyber security

AURA enables users to know room occupancy from outside, automatically reduce unnecessary energy usage, and detect sensor tampering or unusual activity — all through a low-cost smart-room solution.

⭐ Why AURA? “Know the room status from outside, maintain silence inside, save energy automatically, and keep the space secure.”
