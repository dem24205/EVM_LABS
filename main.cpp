#include <opencv2/opencv.hpp> 
#include <iostream>
#include <ctime>

using namespace cv;
using namespace std;

static void applyExtremeRedEffect(Mat& frame) {
    Mat extremeRed = frame.clone();
    for (int i = 0; i < extremeRed.rows; i++) {
        for (int j = 0; j < extremeRed.cols; j++) {
            Vec3b& pixel = extremeRed.at<Vec3b>(i, j);  // BGR format
            int brightness = (pixel[2] + pixel[1] + pixel[0]) / 3;

            if (brightness > 80) {
                pixel[2] = 180;
                pixel[1] = 20;
                pixel[0] = 20;
            }
            else {
                pixel[2] = 30;
                pixel[1] = 5;
                pixel[0] = 5;
            }
        }
    }
    addWeighted(frame, 0.15, extremeRed, 0.85, 0, frame);
}

static void updateAndDisplayFPS(Mat& frame, long& frameCounter, clock_t& lastFpsTime, string& fpsText) {
    clock_t currentTime = clock();
    double elapsedSeconds = (currentTime - lastFpsTime) / 1000.0;
    if (elapsedSeconds >= 1.0) {
        int fps = frameCounter;
        fpsText = "FPS: " + to_string(fps);
        frameCounter = 0;
        lastFpsTime = currentTime;
    }
    putText(frame, fpsText, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);
}


static void printStatistics(clock_t totalTime, clock_t inputTime, clock_t procTime,
    clock_t outputTime, long frameCounter) {
    if (totalTime > 0) {
        double percentage = totalTime / 100.0;
        cout << "\n=== Performance Statistics ===" << endl;
        cout << "Input time: " << (inputTime / percentage) << "%" << endl;  
        cout << "Process time: " << (procTime / percentage) << "%" << endl;      
        cout << "Output time: " << (outputTime / percentage) << "%" << endl; 
        cout << "Other time: " << ((totalTime - inputTime - procTime - outputTime) / percentage) << "%" << endl;
        cout << "Total frames processed: " << frameCounter << endl;              
    }
}

static void processVideoStream() {
    clock_t startTime = clock(); 
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error: Could not open camera" << endl;
        return;
    }

    long frameCounter = 0;      
    Mat frame;                    
    int effectMode = 0;           
    clock_t inputTime = 0;        
    clock_t procTime = 0;         
    clock_t outputTime = 0;       
    clock_t lastFpsTime = startTime; 
    string fpsText = "FPS: 0";   

    cout << "Video stream started. Press SPACE to toggle effect, ESC to exit." << endl;

    while (true) {
        clock_t inputStart = clock(); 
        cap >> frame;
        if (frame.empty()) {
            break;
        }
        inputTime += (clock() - inputStart);

        clock_t procStart = clock();
        if (effectMode == 1) {
            applyExtremeRedEffect(frame);
        }
        procTime += (clock() - procStart); 

        updateAndDisplayFPS(frame, frameCounter, lastFpsTime, fpsText);

        clock_t outputStart = clock();
        imshow("Camera Feed", frame);
        frameCounter++;
        outputTime += (clock() - outputStart); 

        char key = (char)waitKey(1);
        if (key == 27) {
            break;
        }
        else if (key == 32) {
            effectMode = 1 - effectMode;
            cout << "Effect " << (effectMode ? "ENABLED" : "DISABLED") << endl;
        }
    }
    clock_t totalTime = clock() - startTime; 

    printStatistics(totalTime, inputTime, procTime, outputTime, frameCounter);
    cap.release();
    destroyAllWindows();
}

int main(int argc, char* argv[]) {
    processVideoStream();
    return 0;
}
