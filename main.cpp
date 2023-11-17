/*
    Smart Home for the Impaired Speech (SHIS) - Main Module

    This program is part of the SHIS project, a system designed to enable individuals 
    with speech disabilities to interact with and control smart home devices 
    using hand gestures. The software translates these gestures into commands 
    for smart home systems, specifically tailored for integration with Amazon's Alexa.

    The main module of SHIS orchestrates the overall functionality, integrating 
    hand gesture recognition with smart home command execution. It leverages 
    the MediaPipe framework for gesture detection and interfaces with the Alexa Voice Service (AVS) 
    to perform actions based on the recognized gestures.

    Credits:
    - Developer: Chady Doumit, Computer Science Student
    - Mentor: Dr. Shengli Yuan
    - University: University of Houston - Downtown
    - Year: 2023

    The SHIS project showcases the innovative application of computer vision and IoT technologies 
    in enhancing accessibility for individuals with speech impairments, contributing 
    to the development of more inclusive smart home solutions.

    Note: This project is part of the academic curriculum under the guidance of Dr. Shengli Yuan
    and is intended for educational and demonstrative purposes only.
*/


#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctime>

using namespace std;

//Global Variable for filepath

string PathToFiles = "/home/chady/shis/";
string FilePath = PathToFiles + "NG.txt";


//Function that configures the Raspberry Pi environment
void configEnv(){
    system("pactl load-module module-null-sink sink_name=VirtualMic sink_properties=device.description=\"Virtual_Microphone\"");
    system("pactl load-module module-loopback source=VirtualMic.monitor");
    system("pactl set-default-source VirtualMic.monitor");
    //system("pactl set-sink-volume 1 0%");

}

//Function that runs AVS device SDK
void RunAVS() {
    // Set the environment variable    
    setenv("PA_ALSA_PLUGHW", "1", 1);

    string command = "cd /home/chady/sdk-folder/sdk-build && ./SampleApplications/ConsoleSampleApplication/src/SampleApp ./Integration/AlexaClientSDKConfig.json DEBUG9";
    system(command.c_str());
}

void runRecognize(){
    system("python3 /home/chady/shis/recognize.py");
}
//Run AUdioPlayer
void runAudio2(string audioName){
    
    string command = "cd /home/chady/shis && ./AudioPlayer "+audioName;
    system(command.c_str());
}

void runAudio(string audioName){
    system("pactl set-sink-mute @DEFAULT_SINK@ 1");
    string command = "paplay --device=VirtualMic "+audioName;
    system(command.c_str());
    //this_thread::sleep_for(chrono::seconds(5));
    system("pactl set-sink-mute @DEFAULT_SINK@ 0");

}


//Function that reads gesture from text file
string getGesture(string filename){
    ifstream gestureFile;
    gestureFile.open(filename);
    
    string content;
    getline(gestureFile, content);
    gestureFile.close();
    return content;
}

//Function that overwrites text file
void emptyOutTxt(string filename){
    ofstream gestureFile;
    gestureFile.open(filename);
    gestureFile << "";
    gestureFile.close();
}


//Function that  gets timestamp of file
time_t getTimestamp(string filename){
    struct stat fileStat;
    if (stat(filename.c_str(), &fileStat) != 0) {
        std::cerr << "Error getting file statistics for: " << filename << std::endl;
        return false;
    }

    time_t currentTimestamp = fileStat.st_mtime;
    return currentTimestamp;
}

//Function that checks whether timestamp of file has changed since last opening
bool hasTimestampChanged(string filename, time_t previousTimestamp) {
    time_t currentTimestamp = getTimestamp(filename);
    return currentTimestamp != previousTimestamp;
}

string fileController(string filename,time_t lastTimestamp){        
        if(hasTimestampChanged(filename, lastTimestamp)){
            string Gesture = getGesture(filename);
        
            if(Gesture != ""){                     
                cout << endl<<"Gesture Detected" <<endl;
                cout << endl <<"Gesture: " <<Gesture <<endl;
                this_thread::sleep_for(chrono::seconds(1));
                emptyOutTxt(filename);
                lastTimestamp = getTimestamp(filename);   
                return Gesture;
            }   
        }
        return "";     
}




int main() {
    //Setting Each Command to Appropriate Audio File
    string Command1 = PathToFiles+"Weather.wav";    //Open Palm
    string Command2 = PathToFiles+"Time.wav";    //Thumbs Up
    string Command3 = PathToFiles+"Music.wav";    //ILoveYou
    string Command4 = PathToFiles+"Stop.wav";    //Closed Fist
    
    //thread Recognize_Thread(runRecognize);
    
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        cerr << "Failed to create pipe" << endl;        
    }

    pid_t pid = fork();
    if (pid == -1) {
        cerr << "Failed to fork" << endl;        
    }

    if (pid == 0) {  // Child process to run AVS SDK
        close(pipefd[1]);  // Close the unused write end

        // Redirect stdin to the read end of the pipe
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);  // Close the now unnecessary original read end
        RunAVS();
    }else {  // Parent process to send commands to AVS SDK
        close(pipefd[0]);  // Close the unused read end
        this_thread::sleep_for(chrono::seconds(10));  // Give SDK some time to start
        
        ////////////
        configEnv(); //Configure RaspberryPi Environment
        emptyOutTxt(FilePath); //Resets Gesture Text File
        time_t lastTimestamp = getTimestamp(FilePath);
        string CapturedGesture = "";
        
        //Checks for new recognized gesture and runs appropriate Command Audio
        while(true){
            CapturedGesture = fileController(FilePath, lastTimestamp);
            if (CapturedGesture != ""){
                cout <<"Gesture Recorded"<<endl;
                if (CapturedGesture == "Open_Palm"){
                    write(pipefd[1], "h\n", 2);
                    runAudio(Command1); //Weather file
                    write(pipefd[1], "h\n", 2);
                    
                }else if (CapturedGesture == "Thumb_Up"){
                    write(pipefd[1], "h\n", 2);
                    runAudio(Command2);  //Time file
                    write(pipefd[1], "h\n", 2);
                    
                }else if (CapturedGesture == "Closed_Fist"){
                    write(pipefd[1], "h\n", 2);
                    runAudio(Command4); //Stop file
                    write(pipefd[1], "h\n", 2);
                    
                }else if (CapturedGesture == "ILoveYou"){
                    write(pipefd[1], "h\n", 2);
                    runAudio(Command3);  //Music file
                    write(pipefd[1], "h\n", 2);
                    
                }else if (CapturedGesture == "Thumb_Down"){
                    write(pipefd[1], "q\n", 2); //Quits Alexa AVS SDK
                    break;
                   
                    
                }
            }else{
                this_thread::sleep_for(chrono::seconds(1));
            }
            //Exits Loop if Thumbs Down
            if (CapturedGesture == "Thumb_Down"){
                break;
            }
        }
        
        

        int status;
        waitpid(pid, &status, 0);  // Wait for child process to finish
        
        close(pipefd[1]);  // Close the write end after child is done
        
    }
    return 0;
}
