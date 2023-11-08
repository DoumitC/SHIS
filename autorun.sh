#!/bin/bash

#sleep 60  # Waits for 30 seconds

# Run the Python script
python3 /home/chady/shis/recognize.py &
# Run the C++ executable
/home/chady/shis/Main &

#python3 /home/chady/mediapipe/examples/gesture_recognizer/raspberry_pi/recognize.py
