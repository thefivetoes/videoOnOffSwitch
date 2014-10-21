#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    viewMode = VIEW_MODE_SETUP;
    trackingMode = TRACKING_MODE_BLUE;
    
    camWidth 		= 853;	// try to grab at this size.
    camHeight 		= 480;
    
    //we can now get back a list of devices.
    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    
    for(int i = 0; i < devices.size(); i++){
        cout << devices[i].id << ": " << devices[i].deviceName;
        if( devices[i].bAvailable ){
            cout << endl;
        }else{
            cout << " - unavailable " << endl;
        }
    }
    
    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(60);
    vidGrabber.initGrabber(camWidth,camHeight);
    
    img.allocate(camWidth,camHeight,OF_IMAGE_COLOR);
    grayImage.allocate(camWidth,camHeight);
    grayThreshLight.allocate(camWidth,camHeight);
    grayThreshDark.allocate(camWidth,camHeight);
    
    bThreshWithOpenCV = true;
    lightThreshold = 204;
    darkThreshold = 50;
    
    ofSetVerticalSync(true);
    
    rect = ofRectangle(0,0,camWidth,camHeight);
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    ofBackground(100,100,100);
    
    vidGrabber.update();
    
    switch( viewMode ){
        case VIEW_MODE_SETUP:
            updateSetupMode();
            break;
        case VIEW_MODE_APP:
            updateAppMode();
            break;
    }
}
void testApp::updateSetupMode(){
    vidGrabber.update();
    
    unsigned char * sourcePix = vidGrabber.getPixels();
    img.setFromPixels( sourcePix, camWidth, camHeight, OF_IMAGE_COLOR);
    
    smallImage = ofImage();
    smallImage.setFromPixels(sourcePix, camWidth, camHeight, OF_IMAGE_COLOR);
    smallImage.resize( 360, 202 );
    
    redImage = getRedImage(smallImage);
    blueImage = getBlueImage(smallImage);
    greenImage = getGreenImage(smallImage);
}
void testApp::updateAppMode(){
    grayImage.clear();
    vidGrabber.update();
    
    unsigned char * sourcePix = vidGrabber.getPixels();
    img.setFromPixels( sourcePix, camWidth, camHeight, OF_IMAGE_COLOR);
    img.crop(rect.x, rect.y, rect.width, rect.height);
    
    switch (trackingMode){
        case TRACKING_MODE_RGB:
            sourcePix = img.getPixels();
            break;
        case TRACKING_MODE_RED:
            sourcePix = getRedImage(img).getPixels();
            break;
        case TRACKING_MODE_GREEN:
            sourcePix = getGreenImage(img).getPixels();
            break;
        case TRACKING_MODE_BLUE:
            sourcePix = getBlueImage(img).getPixels();
            break;
    }
    img.setFromPixels(sourcePix, camWidth, camHeight, OF_IMAGE_COLOR);
    //img.crop(rect.x, rect.y, rect.width, rect.height);
    ofxCvColorImage colorImage = ofxCvColorImage();
    //colorImage.setFromPixels( img.getPixels(), img.width, img.height );
    colorImage.setFromPixels( img.getPixels(), rect.width, rect.height );
    grayImage.setFromColorImage( colorImage );
    //grayImage.setFromPixels(vidGrabber.getPixels(), camWidth, camHeight);
    
    if(bThreshWithOpenCV) {
        grayThreshLight = grayImage;
        grayThreshDark = grayImage;
        grayThreshLight.threshold(lightThreshold);
        grayThreshDark.threshold(darkThreshold);
        cvAnd(grayThreshLight.getCvImage(), grayThreshDark.getCvImage(), grayImage.getCvImage(), NULL);
    } else {
        
        // or we do it ourselves - show people how they can work with the pixels
        unsigned char * pix = grayImage.getPixels();
        
        int numPixels = grayImage.getWidth() * grayImage.getHeight();
        for(int i = 0; i < numPixels; i++) {
            if(pix[i] < lightThreshold && pix[i] > darkThreshold) {
                pix[i] = 255;
            } else {
                pix[i] = 0;
            }
        }
    }
    
    // update the cv images
    grayImage.flagImageChanged();
    
    // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
    // also, find holes is set to true so we will get interior contours as well....
    //contourFinder.findContours(grayImage, 10, (1024*768)/2, 20, false);
    contourFinder.findContours(grayImage, 100, 2000, 1, false);
    
    if( contourFinder.blobs.size() > 0){
        ofPoint pt = contourFinder.blobs[0].centroid;
        pt.x += rect.x;
        pt.y += rect.y;
        if(contourFinder.blobs.size() == 0){
            switchIsOn = false;
        }
        else if( pt.y > rect.y+rect.height/2 )
        {
            switchIsOn = false;
        }
        else
        {
            switchIsOn = true;
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    switch( viewMode ){
        case VIEW_MODE_SETUP:
            drawSetupMode();
            break;
        case VIEW_MODE_APP:
            drawAppMode();
            break;
    }
}
void testApp::drawSetupMode(){
    ofSetColor(255,255,255);
    
    stringstream title;
    title << "SETUP MODE" <<  endl;
    title <<  "Frame Rate: " << ofToString(ofGetFrameRate(), 2) << endl;
    switch (trackingMode){
        case TRACKING_MODE_RGB:
            img.draw( 40,70 );
            title <<  "RGB";
            break;
        case TRACKING_MODE_RED:
            getRedImage(img).draw( 40,70 );
            title <<  "Red";
            break;
        case TRACKING_MODE_GREEN:
            getGreenImage(img).draw( 40,70 );
            title <<  "Green";
            break;
        case TRACKING_MODE_BLUE:
            getBlueImage(img).draw( 40,70 );
            title <<  "Blue";
            break;
    }
    ofDrawBitmapStringHighlight(title.str(), 20, 20);
    
    smallImage.draw( 1060, 25);
    ofDrawBitmapStringHighlight("RGB", 1060, 25);
    
    redImage.draw( 1060, 241 );
    ofDrawBitmapStringHighlight("Red", 1060, 241);
    
    greenImage.draw( 1060, 457 );
    ofDrawBitmapStringHighlight("Green", 1060, 457);
    
    blueImage.draw( 1060, 673 );
    ofDrawBitmapStringHighlight("Blue", 1060, 673);
    
    stringstream instructionStream;
    instructionStream << "Light Threshold: " << lightThreshold << endl;
    instructionStream << "Dark Threshold: " << darkThreshold << endl;
    instructionStream << "Currently Tracking: ";
    switch( trackingMode ){
        case TRACKING_MODE_RGB:
            instructionStream << "RGB ";
            break;
        case TRACKING_MODE_RED:
            instructionStream << "Red ";
            break;
        case TRACKING_MODE_GREEN:
            instructionStream << "Green ";
            break;
        case TRACKING_MODE_BLUE:
            instructionStream << "Blue ";
            break;
    }
    instructionStream << "(Keys 1-4 to change)" << endl;
    instructionStream << "Rectangle: (x:" << rect.x << " y:" << rect.y << " w:" << rect.width << " h:" << rect.height << ")"<< endl;
    ofDrawBitmapString(instructionStream.str(), 40, 640);
    
    //draw rect
    ofSetColor(255, 255, 255);
    ofNoFill();
    ofRect( rect.x+40, rect.y+70, rect.width, rect.height );
}
void testApp::drawAppMode(){
    
    ofSetColor(255, 255, 255);
    
    vidGrabber.draw(40,70,camWidth,camHeight);
    
    if(nearMode  == 0 ){
        //disabled full frame monitoring below
        //grayThreshLight.draw(40,70, camWidth, camHeight);
        grayThreshLight.draw(40+rect.x,70+rect.y, rect.width, rect.height);
        stringstream threshMode;
        threshMode << "Showing Light Threshold";
        ofDrawBitmapString(threshMode.str(), 20, 800);
    }
    else if( nearMode == 1){
        //disabled full frame monitoring below
        //grayThreshDark.draw(40,70, camWidth, camHeight);
        grayThreshDark.draw(40+rect.x,70+rect.y, rect.width, rect.height);
        stringstream threshMode;
        threshMode << "Showing Dark Threshold";
        ofDrawBitmapString(threshMode.str(), 20, 800);
    }
    else {
        //disabled full frame monitoring below
        //grayImage.draw(40,70, camWidth, camHeight);
        grayImage.draw(40+rect.x,70+rect.y, rect.width, rect.height);
        stringstream threshMode;
        threshMode << "Showing Combined Thresholds";
        ofDrawBitmapString(threshMode.str(), 20, 800);
    }
    
    //disabled full frame monitoring below
    //contourFinder.draw(40,70, camWidth, camHeight);
    contourFinder.draw(40+rect.x,70+rect.y, rect.width, rect.height);
    
    vector<ofxCvBlob> blobs = contourFinder.blobs;
    for( int i = 0; i < blobs.size(); i ++ ){
        ofSetColor(255, 0, 0);
        ofxCvBlob blob = blobs[i];
        ofCircle( blob.centroid.x+40+rect.x, blob.centroid.y+70+rect.y, 5);
        
        ofSetColor(255, 128, 0);
        stringstream label;
        label << blob.area;
        ofDrawBitmapString(label.str(), blob.centroid.x+40+10+rect.x, blob.centroid.y+10+rect.y);
    }
    
    stringstream tresholdStream;
    tresholdStream << "Light Threshold: " << lightThreshold << endl;
    tresholdStream << "Dark Threshold: " << darkThreshold << endl;
    tresholdStream << "Rectangle: (x:" << rect.x << " y:" << rect.y << " w:" << rect.width << " h:" << rect.height << ")"<< endl;
    ofDrawBitmapString(tresholdStream.str(), 20, 600);
    ofSetColor(255,255,255);
    
    stringstream title;
    title << "APP MODE" <<  endl;
    title <<  "Frame Rate: " << ofToString(ofGetFrameRate(), 2);
    title << "Found Blobs: " << contourFinder.nBlobs << endl;
    
    if(switchIsOn){
        title << "Switch is on" <<endl;
    }
    else{
        title << "Switch is off" << endl;
    }
    
    ofDrawBitmapStringHighlight(title.str(), 20, 20);
}

ofImage testApp::getRedImage( ofImage img ){
    
    //clear out green
    img.getPixelsRef().setChannel( 1, img.getPixelsRef().getChannel(0) );
    
    //clear out blue
    img.getPixelsRef().setChannel( 2, img.getPixelsRef().getChannel(0) );
    
    return img;
}

ofImage testApp::getGreenImage( ofImage img ){
    
    //clear out red
    img.getPixelsRef().setChannel( 0, img.getPixelsRef().getChannel(1) );
    
    //clear out blue
    img.getPixelsRef().setChannel( 2, img.getPixelsRef().getChannel(1) );
    
    return img;
}

ofImage testApp::getBlueImage( ofImage img ){
    
    //clear out red
    img.getPixelsRef().setChannel( 0, img.getPixelsRef().getChannel(2) );
    
    //clear out green
    img.getPixelsRef().setChannel( 1, img.getPixelsRef().getChannel(2) );
    
    return img;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'z':
            nearMode++;
            nearMode%=3;
            break;
        case 's':
            viewMode = VIEW_MODE_SETUP;
            break;
        case 'a':
            viewMode = VIEW_MODE_APP;
            break;
        case '1':
            trackingMode = TRACKING_MODE_RGB;
            break;
        case '2':
            trackingMode = TRACKING_MODE_RED;
            break;
        case '3':
            trackingMode = TRACKING_MODE_GREEN;
            break;
        case '4':
            trackingMode = TRACKING_MODE_BLUE;
            break;
        case OF_KEY_UP:
            lightThreshold ++;
            break;
        case OF_KEY_DOWN:
            lightThreshold --;
            break;
        case OF_KEY_LEFT:
            darkThreshold --;
            break;
        case OF_KEY_RIGHT:
            darkThreshold ++;
            break;
    }
}

void testApp::exit(){
    switchIsOn = false;
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    rect.width = 0;
    rect.height = 0;
    rect.x = x-40;
    rect.y = y-70;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    rect.width = x - rect.x-40;
    rect.height = y - rect.y-70;
    
    //reset threshold images
    grayImage.clear();
    grayThreshLight.clear();
    grayThreshDark.clear();
    
    grayImage.allocate(rect.width,rect.height);
    grayThreshLight.allocate(rect.width,rect.height);
    grayThreshDark.allocate(rect.width,rect.height);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    
}
