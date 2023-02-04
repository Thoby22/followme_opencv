// ***************************************
// *
// *  DRS - robotics
// *  (c) 2023 by Thomas Dose
// *
// ***************************************

// follow me function for opencv

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <unistd.h> 

using namespace std;
using namespace cv;

#define count_mov_av  15                                                       // moving average cou
#define magify  1.5
#define DEBUG

int main( int argc, const char** argv )
{
    VideoCapture capture;                                                       // opencv instance
    Mat frame, frame_show,frame_cropped, gray, smallImg, cropped;      //
    CascadeClassifier cascade;                                                  // opencv cascade
    cascade.load(samples::findFile("haarcascade_frontalface_alt.xml"));         // referrence frontal face
    capture.open(0);                                                            // required for opencv
    vector<Rect> faces;                                                         // faces
    Rect r;                                                                     // vector for faces
    double ratio;                                                               // Video ratio
    
    struct point {                                                              // declare point
        int x, y;
    };
    
    struct found_faces {                                                        // declare face data
        point  start, end;                                                      // start and end point of face
    }
    mov_av[count_mov_av] = {0},                                                 // average face data array
    face_av = {0};                                                              // average face result
    
    int counter = 0;                                                            // counter for face data
    
    double t = 0;                                                               // timer
    
    for(;;)                                                                     // infinite loop
    {
        t = (double)getTickCount();                                             // set timer
        
        capture >> frame;                                                       // get frame
        if( frame.empty() )                                                     // if empty stopp
            break;
        
        imshow( "original", frame);	                                            // show original
        
        cvtColor( frame, gray, COLOR_BGR2GRAY );                                // some preperation
        resize( gray, smallImg, Size(), 1, 1, INTER_LINEAR_EXACT );
        equalizeHist( smallImg, smallImg );
        
        cascade.detectMultiScale(frame, faces,                                  // get face
                                 1.1,3,0
                                 |CASCADE_SCALE_IMAGE,
                                 Size(20, 20),
                                 Size(200, 200) );
        
        frame_show = frame.clone();                                             // make copy of original
        
        ratio = double(frame.rows ) / double(frame.cols );                      // calculate frame ratio
        
        
        
        if (faces.size() == 0)                                                   // if no faces detected
        {
            r.x = 0;                                                            // screens starts at 0
            r.y = 0;
            r.width  = frame.cols;                                              // full size
            r.height = cvRound(double(frame.cols) * ratio);                     // ensure same ratio
        } else {
            r = faces[0];                                                      // get data from first face
            
            r.width = r.width * magify;                                             // set size of face in x
            r.height = cvRound(double(r.width) * ratio);                            // set size of face in y
            mov_av[counter].start.x = r.x - r.width / 2;                            // set start point of face
            mov_av[counter].start.y = r.y - r.height / 2;
            mov_av[counter].end.x   = r.x + r.width;                                // set size of face
            mov_av[counter].end.y   = r.y + r.height;
            
#ifdef DEBUG
            cout <<                                                                 // debug
            "x " << mov_av[counter].start.x <<
            " y: " << mov_av[counter].start.y <<
            " w: " << mov_av[counter].end.x <<
            " h: " << mov_av[counter].end.y <<
            " c: " << counter ;
#endif
            counter += 1;                                                           // increase counter by one
            if (counter > count_mov_av) counter = 0;
            
            face_av.start.x = 0;
            face_av.start.y = 0;
            face_av.end.x   = 0;
            face_av.end.y   = 0;
            
            for (int c = 0; c < count_mov_av; c++){                                 // build average
                face_av.start.x += mov_av[c].start.x;
                face_av.start.y += mov_av[c].start.y;
                face_av.end.x += mov_av[c].end.x;
                face_av.end.y += mov_av[c].end.y;
            }
            face_av.start.x = cvRound(double(face_av.start.x) / double(count_mov_av));
            face_av.start.y = cvRound(double(face_av.start.y) / double(count_mov_av));
            face_av.end.x = cvRound(double(face_av.end.x) / double(count_mov_av));
            face_av.end.y = cvRound(double(face_av.end.y) / double(count_mov_av));
            
            if (face_av.start.x < 1) face_av.start.x = 0;                           // check lower values
            if (face_av.start.y < 1) face_av.start.y = 0;                           // check lower values
            
            
            // under construction
            // frame gets squeezed at the borders
            // the size must be changed to fit into frame
            /*
             if  ((face_av.end.x - face_av.start.x > frame.cols) ||
             (face_av.end.y - face_av.start.y > frame.rows))
             {
             break;
             }
             */
            
            
            if (face_av.end.x > frame.cols)                                         // check upper values
            {
                face_av.end.x = frame.cols;                                         // end is not bigger than frame end
            }
            if (face_av.end.y > frame.rows)                                         // check upper values
            {
                face_av.end.y = frame.rows;
            }
            
            
#ifdef DEBUG
            cout << "  start" << face_av.start.x << " END: "<< face_av.end.x <<  endl;
#endif
            
            
            
            frame_cropped=frame(cv::Rect(face_av.start.x,                           // crop the video
                                         face_av.start.y,
                                         face_av.end.x - face_av.start.x,
                                         face_av.end.y - face_av.start.y));
            
            resize(frame_cropped,                                                   // resize the video
                   frame_show,
                   Size(frame.cols,frame.rows),
                   1, 1 ,INTER_LINEAR_EXACT);
            
            
            
            imshow( "detect", frame_show );	                                        // show face
            
            t = (double)getTickCount() - t;                                         // get new time
            
#ifdef DEBUG
            printf( "speed = %g fps\n", 1/t*getTickFrequency());                    // show fps
#endif
            char c = (char)waitKey(1);                                              // key for ending
            if( c == 27 || c == 'q' || c == 'Q' )
                break;
            
        }
    }
    return 0;
}


