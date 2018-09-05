#include "mbed.h"
#include "uLCD_4DGL.h"
#include "SDFileSystem.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
//buttons and music
#include "mbed.h"
#include "Speaker.h"
#include "PinDetect.h"
PinDetect pb1(p16); //you can use different pins
PinDetect pb2(p17); //you can use different pins
PinDetect pb3(p18); // you can use different pins
Speaker mySpeaker(p21);

using namespace std;

uLCD_4DGL uLCD(p28, p27, p29); // serial tx, serial rx, reset pin;

// defiitions for aliens
#define ALIEN_HEIGHT 8
#define ALIEN_WIDTH 11
#define SHIP_HEIGHT 8
#define SHIP_WIDTH 16
#define BULLET_HEIGHT 4
#define BULLET_WIDTH 2
#define _ 0x000000 //BLACK
#define X 0xFFFFFF //WHITE

int ship_sprite[SHIP_HEIGHT * SHIP_WIDTH] = {
    _,_,_,_,_,_,_,X,X,_,_,_,_,_,_,_,
    X,_,_,_,_,_,_,X,X,_,_,_,_,_,_,X,
    X,X,_,_,_,_,_,X,X,_,_,_,_,_,X,X,
    X,X,_,_,_,_,X,X,X,X,_,_,_,_,X,X,
    X,X,_,_,_,X,X,X,X,X,X,_,_,_,X,X,
    X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,
    X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,
    X,_,_,_,_,_,X,X,X,X,_,_,_,_,_,X
};

int bullet_sprite[BULLET_HEIGHT * BULLET_WIDTH] = {
    X,X,
    X,X,
    X,X,
    X,X,
};

int bullet_clear[BULLET_HEIGHT * BULLET_WIDTH] = {
    _,_,
    _,_,
    _,_,
    _,_,
};
int alienBobUp_sprite[ALIEN_HEIGHT * ALIEN_WIDTH] = {
    _,_,X,_,_,_,_,_,X,_,_,
    X,_,_,X,_,_,_,X,_,_,X,
    X,_,X,X,X,X,X,X,X,_,X,
    X,X,X,_,X,X,X,_,X,X,X,
    X,X,X,X,X,X,X,X,X,X,X,
    _,_,X,X,X,X,X,X,X,_,_,
    _,_,X,_,_,_,_,_,X,_,_,
    _,X,_,_,_,_,_,_,_,X,_,
};

int alienBobDown_sprite[ALIEN_HEIGHT * ALIEN_WIDTH] = {
    _,_,X,_,_,_,_,_,X,_,_,
    _,_,_,X,_,_,_,X,_,_,_,
    _,_,X,X,X,X,X,X,X,_,_,
    _,X,X,_,X,X,X,_,X,X,_,
    X,X,X,X,X,X,X,X,X,X,X,
    X,_,X,X,X,X,X,X,X,_,X,
    X,_,X,_,_,_,_,_,X,_,X,
    _,_,_,X,X,_,X,X,_,_,_,

};


int alienAlice_sprite[ALIEN_HEIGHT * ALIEN_WIDTH] = {
    _,_,_,_,X,X,X,_,_,_,_,
    _,X,X,X,X,X,X,X,X,X,_,
    X,X,X,X,X,X,X,X,X,X,X,
    X,X,X,_,_,X,_,_,X,X,X,
    X,X,X,X,X,X,X,X,X,X,X,
    _,_,_,X,X,_,X,X,_,_,_,
    _,_,X,X,_,_,_,X,X,_,_,
    X,X,_,_,_,X,_,_,_,X,X,
};

int alienSkull_sprite[ALIEN_HEIGHT * ALIEN_WIDTH] = {
    _,_,_,X,X,X,X,X,_,_,_,
    _,_,X,X,X,X,X,X,X,X,_,
    _,X,X,_,_,X,_,_,X,X,_,
    X,X,X,_,_,X,_,_,X,X,X,
    X,X,X,X,X,X,X,X,X,X,X,
    X,X,X,X,X,X,X,X,X,X,X,
    _,_,_,X,X,X,X,X,_,_,_,
    _,_,_,X,X,X,X,X,_,_,_,
};

int alienBow_sprite[ALIEN_HEIGHT * ALIEN_WIDTH] = {

    X,X,_,_,_,_,_,_,_,X,X,
    X,X,_,_,_,_,_,_,_,X,X,
    _,_,X,_,_,_,_,_,X,_,_,
    _,_,_,X,X,X,X,X,_,_,_,
    _,_,_,X,X,X,X,X,_,_,_,
    _,_,X,_,_,_,_,_,X,_,_,
    X,X,_,_,_,_,_,_,_,X,X,
    X,X,_,_,_,_,_,_,_,X,X,
};
//******************************************************************************
//base class -- Abstract so can't be instantiated
class ScreenAliens
{
protected:
    float xpos, ypos;
    float dx; //change in position each frame
    bool sprite;
    bool dir; //true means moves left, false means move right
    bool dead; //to erase aliens when they die

public:

    void setPosition(float x, float y) {
        xpos = x;
        ypos = y;
    } //end set position

    float getX() {
        return xpos;
    }
    float getY() {
        return ypos;
    }

    void setDead(bool de) {
        dead = de;
    }

    bool getDead() {
        return dead;
    }

    virtual void setsprite(bool sp) = 0;
    virtual void draw() = 0;
    virtual void update()=0;

    virtual void erase() { //delete current position of alien
        int x2,y2;
        x2= xpos+11;
        y2 = ypos + 8;
        uLCD.filled_rectangle(xpos, ypos,x2 ,y2, BLACK);
    }

};
//****************************************************************************
// Derived classes
class AlienBob: public ScreenAliens
{
public:
//Default Constructors
    AlienBob() {
        xpos = 63;
        ypos = 63;
        dx = 1;
        dir = true;
        sprite = false;
        dead = false;
    }

//    virtual void setPosition(float x, float y) {
//        xpos = x;
//        ypos = y;
//    }
    virtual void setsprite(bool sp) {
        sprite = sp;
    }// end setsprite()

    virtual void draw() {
        if (sprite ==false)
            uLCD.BLIT(xpos, ypos, ALIEN_WIDTH, ALIEN_HEIGHT, alienBobDown_sprite);
        else
            uLCD.BLIT(xpos, ypos, ALIEN_WIDTH, ALIEN_HEIGHT, alienBobUp_sprite);
    }

    virtual void update() {
        erase();
        if(dir == false && xpos >=110) {
            xpos = xpos - dx;
            dir = true;;
        } else if(dir == true && xpos > 8 && xpos <110) {
            xpos = xpos-dx;
        } else if(xpos <= 8) {
            xpos = xpos + dx;
            dir = false;
        } else if (dir == false && xpos <110 && xpos >8) {
            xpos = xpos + dx;
        }
        if(xpos >=110) {
            xpos = 110-dx;
            dir = true;
        }
//        erase();
        setsprite(!sprite);
        draw();
        //wait(.2);
    }//end update()
};
//************************************************
// ALICE Derived class

class alienAlice : public ScreenAliens
{

public:
//Default Constructors
    alienAlice() {
        xpos = 22;
        ypos = 43;
        dx = 6.5;
        dir = true;
        sprite = true;
        dead = false;
    }

//    virtual void setPosition(float x, float y) {
//        xpos = x;
//        ypos = y;
//    }
    virtual void setsprite(bool sp) {
        sprite = sp;
    }// end setsprite()

    virtual void draw() {
        uLCD.BLIT(xpos, ypos, ALIEN_WIDTH, ALIEN_HEIGHT, alienAlice_sprite);
    }

    virtual void update() {
        erase();
        if(dir == false && xpos >=110) {
            xpos = xpos - dx;
            dir = true;;
        } else if(dir == true && xpos > 8 && xpos <110) {
            xpos = xpos-dx;
        } else if(xpos <= 8) {
            xpos = xpos + dx;
            dir = false;
        } else if (dir == false && xpos <110 && xpos >8) {
            xpos = xpos + dx;
        }
        if(xpos >=110) {
            xpos = 110-dx;
            dir = true;
        }
//        erase();
        draw();
        //wait(.2);
    }//end update()
}; // end alienAlice

//**************************************************************************
// Additional Aliens
class alienSkull : public ScreenAliens
{
public:
//Default Constructors
    alienSkull() {
        xpos = 42;
        ypos = 43;
        dx = 10.5;
        dir = true;
        sprite = true;
        dead = false;
    }
    virtual void setsprite(bool sp) {
        sprite = sp;
    }// end setsprite()

    virtual void draw() {
        uLCD.BLIT(xpos, ypos, ALIEN_WIDTH, ALIEN_HEIGHT, alienSkull_sprite);
    }

    virtual void update() {
        erase();
        if(dir == false && xpos >=110) {
            xpos = xpos - dx;
            dir = true;;
        } else if(dir == true && xpos > 8 && xpos <110) {
            xpos = xpos-dx;
        } else if(xpos <= 8) {
            xpos = xpos + dx;
            dir = false;
        } else if (dir == false && xpos <110 && xpos >8) {
            xpos = xpos + dx;
        }
        if(xpos >=110) {
            xpos = 110-dx;
            dir = true;
        }
        draw();
        //wait(.2);
    }
};
//
class alienBow : public ScreenAliens
{

public:
//Default Constructors
    alienBow() {
        xpos = 22;
        ypos = 43;
        dx = 5.5;
        dir = true;
        sprite = true;
        dead = false;
    }

//    virtual void setPosition(float x, float y) {
//        xpos = x;
//        ypos = y;
//    }
    virtual void setsprite(bool sp) {
        sprite = sp;
    }// end setsprite()

    virtual void draw() {
        uLCD.BLIT(xpos, ypos, ALIEN_WIDTH, ALIEN_HEIGHT, alienBow_sprite);
    }

    virtual void update() {
        erase();
        if(dir == false && xpos >=110) {
            xpos = xpos - dx;
            dir = true;;
        } else if(dir == true && xpos > 8 && xpos <110) {
            xpos = xpos-dx;
        } else if(xpos <= 8) {
            xpos = xpos + dx;
            dir = false;
        } else if (dir == false && xpos <110 && xpos >8) {
            xpos = xpos + dx;
        }
        if(xpos >=110) {
            xpos = 110-dx;
            dir = true;
        }
//        erase();
        draw();
        //wait(.2);
    }// end update
};
//**************************************************************************
//Push Button setup

class ship
{


public:
    float xpos, ypos,dx;
    ship():xpos(60),ypos(110),dx(4) {}
    virtual void draw() {
        uLCD.BLIT(xpos, ypos, SHIP_WIDTH, SHIP_HEIGHT, ship_sprite);
    }
    virtual void erase() { //delete current position of alien
        float x2,y2;
        x2= xpos+18;
        y2 = ypos + 8;
        uLCD.filled_rectangle(xpos, ypos,x2 ,y2, BLACK);
    }
    void moveRight() {
        erase();
        xpos = xpos + dx;
        draw();
        //wait(.1);
    }//end moveRight()
    void moveLeft() {
        erase();
        xpos = xpos - dx;
        draw();
        //wait(.1);
    }

};

class bullet
{
public:
    float xbull, ybull;
    bullet():xbull(0),ybull(0) {};

    void appear(float x,float y) {
        xbull = x;
        ybull = y;
        uLCD.BLIT(xbull, ybull, BULLET_WIDTH, BULLET_HEIGHT, bullet_sprite);
    }

    void collision() {
        uLCD.BLIT(xbull, ybull, BULLET_WIDTH, BULLET_HEIGHT, bullet_clear);
    }
    void moveUp() {
        collision();
        ybull = ybull - 1;
        uLCD.BLIT(xbull, ybull, BULLET_WIDTH, BULLET_HEIGHT, bullet_sprite);
    }

};

//******************************************************
int shipUpdate = 0;

void pb1_hit_callback (void) //push button 1 = ship moves left
{
    shipUpdate = 1;
}
void pb2_hit_callback (void)
{
    shipUpdate = 2;
}
void pb3_hit_callback (void)
{
    shipUpdate = 3;
}

//**************************************************************************

int main()
{

    double freq = 90;
    for(int m = 0; m<10; m++) {
        mySpeaker.PlayNote(freq,0.25,1);
        freq = freq +100;
    }
//setup push buttons
    pb1.mode(PullUp);
    pb2.mode(PullUp);
    pb3.mode(PullUp);
// Delay for initial pullup to take effect
    wait(.01);

// Setup Interrupt callback functions for a pb hit
    pb1.attach_deasserted(&pb1_hit_callback);
    pb2.attach_deasserted(&pb2_hit_callback);
    pb3.attach_deasserted(&pb3_hit_callback);
// Start sampling pb inputs using interrupts
    pb1.setSampleFrequency();
    pb2.setSampleFrequency();
    pb3.setSampleFrequency();

    uLCD.baudrate(3000000);
    wait(0.2);

    uLCD.printf("LAME ATTACK");
    wait(2.0);
    uLCD.cls();
//    uLCD.baudrate(BAUD_3000000); //jack up baud rate to max for fast display
//    wait(.02);

    ScreenAliens *aliens[6];
    int xaxis,yaxis;
    int y; //multiplier that will break up screen into rows
    //vector that keeps track of the availability of each of the 7 available rows
    // open rows are denoted by true
    bool openVec[7] = {true, true, true, true, true, true, true};
    for (int i=0; i<6; i++) {
        bool open = false;
        //use rand until a row is available
        while(!open) {
            unsigned seed = time(0);
            srand(seed);
            y = rand()%7 + 1;
            if (openVec[y-1] == true) {
                openVec[y-1] = false;
                open = true;
            }
        } //while loop
        xaxis = rand()% 110+8;
        yaxis = 11*y;

// Randomly select an alien type
        int type;
        type = rand()% 4;
        if(type == 0) {
            aliens[i] = new AlienBob();
            bool randbool = rand() % 2;
            aliens[i]->setsprite(randbool);
        } else if(type == 1)
            aliens[i] = new alienAlice();
        else if(type == 2)
            aliens[i] = new alienSkull();
        else
            aliens[i] = new alienBow();


        aliens[i]->setPosition(xaxis,yaxis);
        aliens[i]->draw();
    } // for loop maybe
//********************************************

    ship shippy;
    bullet shooter;
    shippy.draw();
// bullet parameters
    bool screenBullet = false;
    float x_bull,y_bull;
    int alienkill=0;
    while(alienkill <6) {
//        if(screenBullet == true) {
//            if(y_bull >= 110) {
//                uLCD.BLIT(x_bull, y_bull, BULLET_WIDTH, BULLET_HEIGHT, bullet_clear);
//                screenBullet = false;
//            }
//        }
//************************************************

        for(int k=0; k<6; k++) {
            if(aliens[k]->getDead() == false)
                aliens[k] ->update();
        } // for loop
        if(shooter.ybull <= 10) {
            shooter.collision();
            screenBullet = false;
        }
        if(screenBullet == true) {

            shooter.moveUp();
            for(int k =0; k<6; k++) {
                if(aliens[k] ->getDead() == false) {
                    float xmin = aliens[k]->getX();
                    float ymax = aliens[k]->getY();
                    float xmax = aliens[k]->getX() + 11;
                    float ymin = aliens[k]->getY() - 9;
                    if(shooter.xbull >=xmin && shooter.xbull <= xmax) {
                        if(shooter.ybull >= ymin && shooter.ybull <= ymax) {
                            aliens[k]->erase();
                            shooter.collision();
                            screenBullet = false;
                            aliens[k]->setDead(true);
                            alienkill++;


                        }
                    }
                }
            }// for loop
        } // screenBullet(check)
//            else {
        if (shipUpdate == 1) {
            shippy.moveLeft();
//                    uLCD.printf("button %i\n",shipUpdate);
            shipUpdate = 0;
        }
        if(shipUpdate ==2 && screenBullet == false) {
            x_bull = shippy.xpos + 7;
            y_bull = shippy.ypos - 7;
            screenBullet = true;
            shooter.appear(x_bull,y_bull);
            shipUpdate = 0;

        }
        if(shipUpdate == 3) {
            shippy.moveRight();
            shipUpdate = 0;
        }

//************************************************
//            }// else


    } //while loop

    for(int m=0; m<6; m++) {
        delete aliens[m];
    }

    uLCD.printf("You Saved EARTH!, %i/n");

} // End Main
