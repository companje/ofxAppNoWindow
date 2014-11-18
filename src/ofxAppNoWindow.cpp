#include "ofxAppNoWindow.h"
#include "ofBaseApp.h"
#include "ofGraphics.h"

#if defined TARGET_OSX || defined TARGET_LINUX
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <signal.h>

struct termios orig_termios;
struct sigaction act_open;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
	// setup new_termios for raw keyboard input
    cfmakeraw(&new_termios);
	// handle "\n" properly
	new_termios.c_oflag |= OPOST;
	//new_termios.c_oflag |= ONLCR;
	// set the new_termios
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
	return 0;
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

#endif

const string ofNoopRenderer::TYPE="NOOP";

//----------------------------------------------------------
ofxAppNoWindow::ofxAppNoWindow(){
	ofAppPtr = NULL;
	width = 0;
	height = 0;
}


void ofxAppNoWindow::setupOpenGL(int w, int h, int screenMode){
	width = w;
	height = h;
}

//------------------------------------------------------------
void ofxAppNoWindow::runAppViaInfiniteLoop(ofBaseApp * appPtr){

	ofSetCurrentRenderer(ofPtr<ofBaseRenderer>(new ofNoopRenderer));

	ofAppPtr = appPtr;

	#if defined TARGET_OSX || defined TARGET_LINUX
	// for keyboard
	//set_conio_terminal_mode();
	#endif

	ofNotifySetup();

//    ofLogNotice("ofxAppNoWindow")
//		<< "***" << endl
//		<< "***" << endl
//		<< "*** running a headless (no window) app" << endl
//		<< "***" << endl
//		<< "*** keyboard input works here" << endl
//		<< "***"<< endl
//		<< "*** press Esc or Ctrl-C to quit" << endl
//		<< "***" << endl;

	while (true)
	{
        /// listen for escape
        #ifdef TARGET_WIN32
        if (GetAsyncKeyState(VK_ESCAPE))
            ofNotifyKeyPressed(OF_KEY_ESC);
        #endif

		#if defined TARGET_OSX || defined TARGET_LINUX
		while ( kbhit() )
		{
			int key = getch();
			if ( key == 27 )
			{
				ofNotifyKeyPressed(OF_KEY_ESC);
			}
			else if ( key == /* ctrl-c */ 3 )
			{
				ofLogNotice("ofxAppNoWindow") << "Ctrl-C pressed" << endl;
				OF_EXIT_APP(0);
			}
			else
			{
				ofNotifyKeyPressed(key);
			}
		}
		#endif


		ofNotifyUpdate();
		ofNotifyDraw();

	}
}

//------------------------------------------------------------
void ofxAppNoWindow::exitApp(){

//  -- This already exists in ofExitCallback

//	static ofEventArgs voidEventArgs;
//
//	if(ofAppPtr)ofAppPtr->exit();
//
//	#ifdef OF_USING_POCO
//		ofNotifyEvent( ofEvents.exit, voidEventArgs );
//	#endif

	ofLogVerbose("ofxAppNoWindow") << "terminating headless (no window) app!";


#if defined TARGET_OSX || defined TARGET_LINUX
    // this doesn't exist on windows and gives linking errors, so commented out.
	reset_terminal_mode();
#endif

	OF_EXIT_APP(0);
}

ofPoint	ofxAppNoWindow::getWindowPosition(){
	return ofPoint(0,0);
}

ofPoint	ofxAppNoWindow::getWindowSize(){
	return ofPoint(width,height);
}

ofPoint	ofxAppNoWindow::getScreenSize(){
	return ofPoint(width,height);
}


int	ofxAppNoWindow::getWidth(){
	return width;
}

int	ofxAppNoWindow::getHeight(){
	return height;
}


