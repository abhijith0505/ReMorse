#include "remorse.h"
#include "states.h"
namespace R_keys
{
    bool UP=false;
    int CURSOR=0;
    void menu_key(unsigned char key, int x, int y)
    {
        switch (key)
        {
            case 13:
                switch(CURSOR)
                {
                    case 0:
                        R_states::STATE=R_states::GAME; break;
                    case 1:
                        exit(0); break;
                }
                CURSOR=0;
                break;

        }
    }
    void menu_splkey(unsigned char key, int x, int y)
    {
        switch (key)
        {
            case GLUT_KEY_F11:glutFullScreenToggle();break;
            case GLUT_KEY_UP:
                            if(CURSOR>0)
                                CURSOR--;
                            else CURSOR=1;
                            break;
            case GLUT_KEY_DOWN:
                             CURSOR=(CURSOR+1)%2; break;
        }
    }
    void pause_key(unsigned char key, int x, int y)
    {
        switch (key)
        {
            case 27 :
            case 'q':
            case 'Q':
                R_states::STATE=R_states::GAME; break;
            case 13:
                switch(CURSOR)
                {
                    case 0:
                        R_states::STATE=R_states::GAME; break;
                    case 1:
                        R_states::STATE=R_states::MENU; break;
                }
                CURSOR=0;
                break;

        }
    }
    void pause_splkey(unsigned char key, int x, int y)
    {
        switch (key)
        {
            case GLUT_KEY_F11:glutFullScreenToggle();break;
            case GLUT_KEY_UP:
                            if(CURSOR>0)
                                CURSOR--;
                            else CURSOR=1;
                            break;
            case GLUT_KEY_DOWN:
                             CURSOR=(CURSOR+1)%2; break;
        }
    }
    void game_key(unsigned char key, int x, int y)
    {
        switch (key)
        {
            case 27 :
            case 'q':
            case 'Q':
                R_states::STATE=R_states::PAUSE; break;
        }
    }
    void game_keyup(unsigned char key, int x, int y)
    {
        switch (key)
        {
        }
    }
    void game_splkey(unsigned char key, int x, int y)
    {
        switch (key)
        {
             case GLUT_KEY_UP: UP=true; break;
             case GLUT_KEY_F11:glutFullScreenToggle();break;
        }
    }
     void game_splkeyup(unsigned char key, int x, int y)
    {
        switch (key)
        {
             case GLUT_KEY_UP: UP=false; break;
        }
    }
    void key(unsigned char key, int x, int y)
    {
        //key down for normal keys
        switch(R_states::STATE)
        {
            case R_states::MENU:
                            menu_key(key,x,y);break;
            case R_states::GAME:
                            game_key(key,x,y);break;
            case R_states::PAUSE:
                            pause_key(key,x,y);break;
        }

        //glutPostRedisplay();
    }
    void keyup(unsigned char key, int x, int y)
    {
        //key up for normal keys
        switch(R_states::STATE)
        {
            case R_states::MENU:
                            break;
            case R_states::GAME:
                            game_keyup(key,x,y);break;
            case R_states::PAUSE:
                            break;
        }
    }
    void splkey(int key, int x, int y)
    {
        //key down for special keys
        switch(R_states::STATE)
        {
            case R_states::MENU:
                            menu_splkey(key,x,y);break;
            case R_states::GAME:
                            game_splkey(key,x,y);break;
            case R_states::PAUSE:
                            pause_splkey(key,x,y);break;
        }
    }
    void splkeyup(int key, int x, int y)
    {
        //key up for special keys
        switch(R_states::STATE)
        {
            case R_states::MENU:
                            break;
            case R_states::GAME:
                            game_splkeyup(key,x,y);break;
            case R_states::PAUSE:
                            break;
        }
    }


}


