#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class testApp : public ofBaseApp {
    
public:
    void setup();
    void update();
    void updateSetupMode();
    void updateAppMode();
    void draw();
    void drawSetupMode();
    void drawAppMode();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    int                 viewMode;
    static const int           VIEW_MODE_APP = 0;
    static const int           VIEW_MODE_SETUP = 1;
    
    int                 trackingMode;
    static const int           TRACKING_MODE_RGB = 0;
    static const int           TRACKING_MODE_RED = 1;
    static const int           TRACKING_MODE_GREEN = 2;
    static const int           TRACKING_MODE_BLUE = 3;
    
    int nearMode = 0;
    
    ofVideoGrabber 		vidGrabber;
    int 				camWidth;
    int 				camHeight;
    int                 lightThreshold;
    int                 darkThreshold;
    
    ofRectangle rect;
    
    ofImage img;
    ofImage smallImage;
    ofImage redImage;
    ofImage greenImage;
    ofImage blueImage;
    ofxCvColorImage colorImage;
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshLight; // the near thresholded image
    ofxCvGrayscaleImage grayThreshDark; // the far thresholded image
    bool bThreshWithOpenCV;
    ofxCvContourFinder contourFinder;
    
    ofImage getRedImage( ofImage img );
    ofImage getBlueImage( ofImage img );
    ofImage getGreenImage( ofImage img );
    
    bool switchIsOn = false;
};
