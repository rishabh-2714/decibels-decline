//make variables for pin names (see setup) and import necessary libraries. You need to send data to Python (written by Shaurya) via WiFi or IoT.

void setup() {
  //pinMode for a noise sensor, a large red light and a buzzer. Figure out which models.
  serial.begin(/*9600 if Arduino, 112500 is ESP32*/);
}

void loop() {
  //Over here, you need to make the actual code.
  //Start with storing the volume in a variable called `vol`

  //Then, publish that variable to Serial Monitor and Python Serial

  //Then, assuming a variable t (for the volume threshold in deciBels),
  if(vol>t) {
    //Switch on the light
    //Have the buzzer make a noise for two seconds
  }
}
