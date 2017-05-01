///////////////////////////////Requires opengl 1.2/////////////////////////////
#include <string>
#include <sstream>
#include "remorse.h"
#include "lib/lodepng.h"
#include "lib/lodepng.cpp"
#include "states.h"
#include "keys.h"
#include "timer.h"
#include "physics.h"
// THINK:maybe height and width should be in settings?
int HEIGHT = 600;
int WIDTH = 800;
//THINK: where to put score, may be in the values given by backend
long long SCORE=0;
// THINK:Where the hell do I keep this texname variable?
// may be make static put inside the function? BTW this for loading texture
GLuint texname;
using namespace std;
namespace R_settings
{
	// TODO:put more things here
    bool ANTIALIAS=true;
    int MINWIDTH=640;
    int MINHEIGHT=550;
}

namespace R_images
{
	// THINK:not sure if this is the right way to store images and its properties
    const char* logoName="morse.png";
    vector<unsigned char> logo;
    unsigned logoWidth=261;
    unsigned logoHeight=261;
    const char* samName[]={"sam01.png","sam02.png"};
    vector<unsigned char> sam[2];
    unsigned samWidth[]={110,110};
    unsigned samHeight[]={186,186};

    /** OpenGL seems to draw images vertically flipped
    	this function inverts our data so that it displays correctly
    	@param img is our image data vector
    	@param width is our image width
    	@param height is our image height
    */
    void invert(vector<unsigned char> &img,const unsigned width,const unsigned height)
    {
        unsigned char *imagePtr = &img[0];
        unsigned char *top = NULL;
        unsigned char *bottom = NULL;
        unsigned char temp = 0;
        for( int h = 0; h <(int) height/2; ++h )
        {

            top = imagePtr + h * width * 4;
            bottom = imagePtr + (height - h - 1) * width*4;
            for( int w = 0; w < (int)width*4; ++w )
            {
                temp = *top;
                *top = *bottom;
                *bottom = temp;
                ++top;
                ++bottom;
            }
        }
    }
    /** Loads all required images for our game
    */
    void loadImages()
    {
    	//THINK:we are doing the same if else many times may be make a function?
        int error;
        if((error=lodepng::decode(logo,logoWidth,logoHeight,logoName)))
        {
            cout<<logoName<<":"<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(logo,logoWidth,logoHeight);
        if((error=lodepng::decode(sam[0],samWidth[0],samHeight[0],samName[0])))
        {
            cout<<samName[0]<<":"<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(sam[0],samWidth[0],samHeight[0]);
        if((error=lodepng::decode(sam[1],samWidth[1],samHeight[1],samName[1])))
        {
            cout<<samName[1]<<":"<<lodepng_error_text(error)<<endl;
            exit(1);
        }
        else
            invert(sam[1],samWidth[1],samHeight[1]);
    }

}

void setTexture()
{
    // TODO:should use a variable input to set specific texture
    int v=rand()%2;
    glGenTextures(1, &texname);
    glBindTexture(GL_TEXTURE_2D, texname);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // without this texture darkens
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, R_images::samWidth[0], R_images::samHeight[0],
                0, GL_RGBA, GL_UNSIGNED_BYTE, &R_images::sam[v][0]);
}
/**
    Set a letter on top of screen, used during game loop
    @param ch specifies the character to be displayed
*/
void setLetter(char ch)
{
    glLineWidth(3);
    glColor3ub(0x42,0x42,0x42);
    //push and pop is required in model view!
    glPushMatrix();
    glTranslatef(WIDTH/2.0-50,HEIGHT-110,0);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)ch);
    //glutStrokeString(GLUT_STROKE_MONO_ROMAN,(unsigned char*)"abcdef");
    glPopMatrix();
}
/**
   Draw a Button at 0,0
   @param str specifies the string to be shown in button
   @param outlined specifies outline, mostly used to show selection
*/
void drawButton(const char* str,bool outlined)
{
	// Currently all our buttons use .3x of size of font
	// anyway we can scale it by calling scalef before drawButton
	// all values found by trial and error method
    float width=glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char*)str)*.3;
    float height=glutStrokeHeight(GLUT_STROKE_ROMAN)*.3;
    glColor3ub(0x42,0x42,0x42);
    glBegin(GL_POLYGON);
        glVertex2f(0,0);
        glVertex2f(0,height);
        glVertex2f(width+10,height);
        glVertex2f(width+10,0);
    glEnd();
    //glRectf(WIDTH/2.0-5-glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char*)"PLAY")*.3/2,HEIGHT-207,WIDTH/2.0+5+glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char*)"PLAY")*.3/2,HEIGHT-207+glutStrokeHeight(GLUT_STROKE_ROMAN)*.3);
    if(outlined)
    {
    	// Draw outline if specified
        glColor3ub(0,0,0);
        glLineWidth(2);
        glBegin(GL_LINE_LOOP);
            glVertex2f(0,0);
            glVertex2f(0,height);
            glVertex2f(width+10,height);
            glVertex2f(width+10,0);
        glEnd();
    }
    glLineWidth(2);
    glColor3ub(0xF4,0x43,0x36);
    glTranslatef(5,7,0);
    glScalef(.3,.3,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)str);
}
float getButtonWidth(const char* str)
{
    return glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char*)str)*.3+10;
}
float getButtonHeight(const char* str)
{
    return glutStrokeHeight(GLUT_STROKE_ROMAN)*.3;
}
void menuLoop()
{
    // Thickness of font
    glLineWidth(3);
    glColor3ub(0xff,0xff,0xff);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-250,HEIGHT-110,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"ReMorse");
    //cout<<glutStrokeHeight(GLUT_STROKE_ROMAN)<<" "<<glutStrokeLength(GLUT_STROKE_ROMAN,(unsigned char*)"ReMorse")<<endl;
    glPopMatrix();
    glPushMatrix();
    glTranslatef(WIDTH/2.0-getButtonWidth("PLAY")/2.0,HEIGHT-200,0);
    drawButton("PLAY",R_keys::CURSOR==0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(WIDTH/2.0-getButtonWidth("QUIT")/2.0,HEIGHT-260,0);
    drawButton("QUIT",R_keys::CURSOR==1);
    glPopMatrix();

    glPushMatrix();
    glRasterPos2i(WIDTH/2-(R_images::logoWidth/2),0);
    glDrawPixels(R_images::logoWidth,R_images::logoHeight, GL_RGBA, GL_UNSIGNED_BYTE, &R_images::logo[0]);
    glPopMatrix();
}

void gameLoop()
{
	//Update physics
	R_physics::stepPhysics();

    //setLetter('R');
    setLetter(R_physics::curLetter);

    //display score
    //why StringStream? Cuz to_string() doesn't freaking work in mingw compiler
    glLineWidth(3);
    ostringstream stm;
    stm<<"$:"<<SCORE;
    glColor3ub(0xff,0xff,0xff);
    glPushMatrix();
    glTranslatef(5,HEIGHT-40,0);
    glScalef(.3,.3,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)stm.str().c_str());
    glPopMatrix();
    /* enable texture.
    !!!!!!!!Very dangerous!!!!!!!. might affect other objects. disable before drawing other objects */
    glEnable(GL_TEXTURE_2D);
    setTexture();
    glPushMatrix();

    // TODO:should use global box variables for drawing character quad
    glBegin(GL_POLYGON);
        /*glTexCoord2d(0,0);  glVertex2f(0+50,0+50);
        glTexCoord2d(0,1);  glVertex2f(0+50,R_images::samHeight[0]+50);
        glTexCoord2d(1,1);  glVertex2f(R_images::samWidth[0]+50,R_images::samHeight[0]+50);
        glTexCoord2d(1,0);  glVertex2f(R_images::samWidth[0]+50,0+50);*/

    	float p00x = R_physics::getPlayerX()*45.0 - R_images::samWidth[0]/2.0 + 30.0*8;
    	float p00y = R_physics::getPlayerY()*45.0 - R_images::samHeight[0]/2.0 + 30.0*8;

    	glTexCoord2d(0,0);  glVertex2f(p00x, p00y);
        glTexCoord2d(0,1);  glVertex2f(p00x, p00y+R_images::samHeight[0]);
        glTexCoord2d(1,1);  glVertex2f(p00x+R_images::samWidth[0], p00y+R_images::samHeight[0]);
        glTexCoord2d(1,0);  glVertex2f(p00x+R_images::samWidth[0], p00y);

    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    //draw ground... TODO:use actual values later
    glPushMatrix();
    /*glBegin(GL_POLYGON);
        glColor3ub(0xF4,0x43,0x36);
        glVertex2f(0,0);
        glVertex2f(WIDTH,0);

        glColor3ub(0xC6,0x28,0x28);
        //glVertex2f(WIDTH,50);
        //glVertex2f(0,50);

        float g_height = R_physics::groundHeight * 20.0;
        glVertex2f(WIDTH,g_height);
        glVertex2f(0,g_height);
    glEnd(); */
    glPopMatrix();
}

void pauseLoop()
{
    glLineWidth(3);
    glColor3ub(0xff,0xff,0xff);
    glPushMatrix();
    glTranslatef(WIDTH/2.0-250,HEIGHT-110,0);
    glScalef(.5,.5,0);
    glutStrokeString(GLUT_STROKE_ROMAN,(unsigned char*)"Game Paused!");
    glPopMatrix();

    glPushMatrix();
    glTranslatef(WIDTH/2.0-getButtonWidth("RESUME")/2.0,HEIGHT-200,0);
    drawButton("RESUME",R_keys::CURSOR==0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(WIDTH/2.0-getButtonWidth("QUIT")/2.0,HEIGHT-260,0);
    drawButton("QUIT",R_keys::CURSOR==1);
    glPopMatrix();

}

static void resize(int width, int height)
{
    // check if window size is too small, call reshape appropriately
    // THINK/TODO: may be we should find a scale factor or something and scale each objects
    if(width<R_settings::MINWIDTH && height<R_settings::MINHEIGHT)
        glutReshapeWindow(R_settings::MINWIDTH,R_settings::MINHEIGHT);
    else if(width<R_settings::MINWIDTH)
        glutReshapeWindow(R_settings::MINWIDTH,height);
    else if(height<R_settings::MINHEIGHT)
        glutReshapeWindow(width,R_settings::MINHEIGHT);
    else
    {
        glClearColor(0.9568f,0.2627f,0.2117f,1.0f);
        WIDTH=width;
        HEIGHT=height;
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0,width,0,height);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity() ;
    }

}

static void display(void)
{
    static int frame=0,curtime,timebase=0;
    // THINK:Maybe clearing should be done in each loop separately,
    // That way pause menu can show game in bg
    glClear(GL_COLOR_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Draw stuff here
    switch(R_states::STATE)
    {
    case R_states::MENU:
        menuLoop();break;
    case R_states::GAME:
        gameLoop();break;
    case R_states::PAUSE:
        pauseLoop();break;
    }
    // FPS calculation
    frame++;
    curtime=glutGet(GLUT_ELAPSED_TIME);
    if (curtime - timebase > 1000) {
        printf("FPS:%4.2f\n",
            frame*1000.0/(curtime-timebase));
        timebase = curtime;
        frame = 0;
    }
    glutSwapBuffers();
}


static void idle(void)
{
    // display opengl error for debugging
    if (GLenum err = glGetError())
    {
      cerr << "OpenGL ERROR: " << gluErrorString(err) << endl;
    }
    glutPostRedisplay();
}
/**
    Do anti alias if set in settings
*/
void antialias()
{
    if(R_settings::ANTIALIAS)
    {
        ///////////////////////Do anti alias/////////////////////////
        // creates spaces (lines) bw polygon if multisample does not work
        glEnable(GL_POLYGON_SMOOTH);
        // THINK:not sure enabling again is required?
        glEnable(GL_MULTISAMPLE);
        ////////////////////////end of anti alias////////////////////
        GLint iMultiSample = 0;
        GLint iNumSamples = 0;
        glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
        glGetIntegerv(GL_SAMPLES, &iNumSamples);
        printf("MSAA on, GL_SAMPLE_BUFFERS = %d, GL_SAMPLES = %d\n", iMultiSample, iNumSamples);
    }
    else
    {
        glDisable(GL_MULTISAMPLE);
        printf("MSAA off\n");
    }
}
/* Program entry point */

int main(int argc, char *argv[])
{
    //TODO:should put this in init
    R_images::loadImages();
    glutInit(&argc, argv);
    glutInitWindowSize(WIDTH,HEIGHT);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GL_MULTISAMPLE);
    if(!glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
    {
    //fallback if multisample is not possible
        glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
        R_settings::ANTIALIAS=false;
    }
    else
        glutSetOption(GLUT_MULTISAMPLE, 8);
    glClearColor(0.9568f,0.2627f,0.2117f,1.0f);
    glutCreateWindow("ReMorse");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
     /* smoothen lines n points, doesn't seem to get affected by MULTISAMPLE.
     works only if called after the BlendFunc */
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    antialias();
    // make cursor invisible
    glutSetCursor(GLUT_CURSOR_NONE);
    // set appropriate functions, THINK::may be we should put this in init as well
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(R_keys::key);
    glutKeyboardUpFunc(R_keys::keyup);
    glutSpecialFunc(R_keys::splkey);
    glutSpecialUpFunc(R_keys::splkeyup);
    glutTimerFunc(17,timer,UPDATE);
    // glutMouseFunc(R_mouse::mouse);
    glutIdleFunc(idle);
    // make key not repeat events on long press
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    glutMainLoop();

    return EXIT_SUCCESS;
}
