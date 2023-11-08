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
    string Command1 = PathToFiles+"Weather.wav";
    string Command2 = PathToFiles+"Time.wav";
    string Command3 = PathToFiles+"Music.wav";
    string Command4 = PathToFiles+"Stop.wav";
    
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
        configEnv();
        emptyOutTxt(FilePath);
        time_t lastTimestamp = getTimestamp(FilePath);
        string CapturedGesture = "";
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
                    runAudio(Command4);
                    write(pipefd[1], "h\n", 2);
                    
                }else if (CapturedGesture == "ILoveYou"){
                    write(pipefd[1], "h\n", 2);
                    runAudio(Command3);  //Music file
                    write(pipefd[1], "h\n", 2);
                    
                }else if (CapturedGesture == "Thumb_Down"){
                    write(pipefd[1], "q\n", 2);
                    break;
                    //runAudio("Stop.wav");
                    //write(pipefd[1], "h\n", 2);
                    
                }
            }else{
                this_thread::sleep_for(chrono::seconds(1));
            }
            if (CapturedGesture == "Thumb_Down"){
                break;
            }
        }
        
        

        int status;
        waitpid(pid, &status, 0);  // Wait for child process to finish
        
        close(pipefd[1]);  // Close the write end after child is done
        //system("^C");
        
        //Recognize_Thread.join();
        /////////////////
    }
    return 0;
}
