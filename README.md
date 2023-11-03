# SHIS
Smart Home for the Impaired Speech Using Alexa Device SDK, OpenCV and GOOGLE's MediaPipe

## Introduction
Smart Home for the Impaired Speech (SHIS) is a software designed to help people with speech disabilities gain access to smart home features that usually require voice activation. By using hand gestures, users can control smart home devices such as lights, thermostats, door locks, etc.

## Installation

Follow these steps to install and run the SHIS program on your Raspberry Pi.

### Prerequisites

Before proceeding with the installation, ensure that you have a Raspberry Pi with a camera module installed and that it is connected to the internet.

### Step 1: Install AVS Device SDK

1. Follow the instructions to install the AVS Device SDK on your Raspberry Pi. The SDK allows the device to utilize Amazon Alexa Voice Service.
2. For detailed installation instructions, visit [Amazon Developer Guide](https://developer.amazon.com/en-US/docs/alexa/avs-device-sdk-1-2x/raspberry-pi-script.html).

### Step 2: Install Pulse Audio

1. Install Pulse Audio, which is a sound server used to manage audio input and output on your device.
2. Pulse Audio will help in creating a virtual microphone to capture the audio signals for AVS.

### Step 3: Add OpenCV Library

1. Add the OpenCV library to your system. OpenCV is used for image and video analysis and will help in recognizing the hand gestures through the camera feed.

### Step 4: Add Google's MediaPipe Library

1. Install Google's MediaPipe library, a framework for building multimodal (e.g., video, audio, any time-series data) applied machine learning pipelines.
2. MediaPipe will be responsible for the gesture recognition process.

### Step 5: Clone the SHIS Repository

1. Clone the SHIS repository from GitHub to your local machine.

   
### Step 6: Navigate to Repo Folder and Run autorun.sh

1. Navigate to the repository folder in the terminal.
2. Run the autorun script to start the program:


### Step 7: Register Device with Amazon Account

1. If this is your first time running the program, execute it from the terminal and follow the on-screen instructions to register your device with your Amazon account.

## Usage

Once you have completed the installation and setup, use the predefined hand gestures to control your smart home devices through Alexa. Ensure that your gestures are within the camera's field of view for accurate recognition.

## Contributing

Contributions to the SHIS project are welcome. Please feel free to fork the repository, make changes, and submit pull requests.


## Acknowledgments

Special thanks to Dr. Shengli Yuan for mentorship and guidance throughout the development of this project.

