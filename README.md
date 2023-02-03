# followme_opencv
face detection with follow me function

Install opencv on linux 

Open this folder
Ensure haarcascade files are in the same folder
Compile with 

g++ -ggdb facedetect.cpp -o facedetect `pkg-config --cflags --libs opencv4`

Run with 

./facedetect

