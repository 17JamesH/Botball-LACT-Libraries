#include "generic.h"
#include <math.h>

// Utility
float bound (float num, float max) {
  if (num > max) return max;
  else if (num < -max) return -max;
  else return num;
}

int in_range (int input, int wanted, int fudge) {
  if (wanted + fudge >= input && wanted - fudge <= input) return 1;
  else return 0;
}

float ET_distance (int x) {
  return sqrt(3674186.332167 / (x - 9.384503) + 26294.871673) - 176.997129;
}

int et_avg (int port) {
  int totalcount = 100;
  int run = 0;
  int i;
  
  for (i = 0; i < totalcount; i++) {
    run += analog_et(port);
    msleep(2);
  }
  
  return run / totalcount;
}

// Light Start
/*
#define LIGHT_S() analog10(sensor)
void light_start (int sensor) {
  WAIT(!(b_button() || c_button()));
  int max = 0,
    min = 9999,
    curr = 0,
    avg = 0;
  
  display_clear();
  display_printf(0,0,"Max:");
  display_printf(0,1,"Min:");
  display_printf(0,2,"Curr:");
  display_printf(0,3,"avg:");
  
  while (!b_button() && !c_button()) {
    curr = LIGHT_S();
    
    if (curr > max) max = curr;
    if (curr < min) min = curr;
    
    avg = (max + min) / 2;
    
    display_printf(5, 0, "%d   ", max);
    display_printf(5, 1, "%d   ", min);
    display_printf(6, 2, "%d   ", curr);
    display_printf(5,3,"%d   ",avg);
    
    if (curr > avg) display_printf(10,5,"XX");
    else display_printf(10,5,"OO");
    
    msleep(50);
  }
  
  display_clear();
  display_printf(0,4,"Prepared to begin: left to skip");
  
  while (LIGHT_S() > avg && !(c_button())) msleep(50);
}
*/
// Timing
long _start_time;
void start() {
  _start_time = systime();
}

float curr_time() {
  return (systime()-_start_time)/1000.0;
}

void now() {
  printf("now %f\n",curr_time());
}

void nowstr(char *s) {
  printf("%s %f\n",s,curr_time());
}

void wait_till(float t) {
  now();
  msleep((long)((t - curr_time()) * 1000));
}

// Servos

// servo_off(int) is replaced with the KIPR library function disable_servo(int)
void servo_set(int port, int end, float time/*, float increment*/) { // position is from 0-2047
  float increment = .01;
  float curr,start = get_servo_position(port);
  float i = ((end-start)/time)*increment;
  
  curr = start;
  
  if (start > end) {
    while (curr > end) {
      set_servo_position(port, curr);
      
      curr += i;
      msleep((long)(increment * 1000));
    }
  } else if (start < end) {
    while(curr < end) {
      set_servo_position(port,curr);
	  
      curr += i;
      msleep((long)(increment * 1000));
    }
  }
  
  set_servo_position(port,end);
}

// Camera stuff
void multicamupdate (int reps) {
  int i;
  for(i = 0; i < reps; i++) camera_update();
}

int cam_area (int channel) { // returns largest blob in channel, or 0 if none
  if (get_object_count(channel) > 0) {
    return get_object_area(channel, 0);
  }
  
  return 0;
}

void update_wait(){ // updates the camera, and waits until success
  while(!camera_update()) msleep(1);
}

#ifndef _NEWMENU
void next(int state) {
  currstate = state;
}
#else

// If menu code is in use, print the time of each state switch
extern struct menuitem menu[];

void next (int state) {
  int i = -1;
  currstate = state;
  
  while (!strcmp(menu[++i].name, "FIN")) {
    if (menu[i].snum == state) {
      nowstr(menu[i].name);
      return;
    }
  }
  
  now();
}
#endif


int getabbutton () { // returns (0, 1) on (a, b)
  WAIT(!(a_button() || b_button()));
  WAIT(a_button() || b_button());
  
  int x;
  if (a_button()) x=0;
  if (b_button()) x=1;
  
  printf("\n%d",x);
  WAIT(!(a_button() || b_button()));
  return x;
}

int getabcbutton () { // returns (0, 1, 2) on (a, b, c)
  printf("\n\nWAITING FOR INPUT\n");
  
  WAIT(!(a_button() || b_button() || c_button()));
  WAIT(a_button() || b_button() || c_button());
  
  int x;
  if (a_button()) x=0;
  if (b_button()) x=1;
  if (c_button()) x=2;
  
  printf("\n%d",x);
  WAIT(!(a_button() || b_button() || c_button()));
  
  return x;
}

//Setup Tophat

int leftWhite, rightWhite;
int leftBlack, rightBlack;
#define LEFT_LINE $
#define RIGHT_LINE $
void setupTophatDouble() 
{
    int accept = 0;
    while (!accept) 
    {
        printf("\n\n------------------------\n\nMove tophat sensors over white area of board.\nPress right button to set.\n");
        while (!right_button()) {}
        leftWhite = analog(LEFT_LINE);
        rightWhite = analog(RIGHT_LINE);
        printf("Left tophat value: %d, Right tophat value: %d\n", leftWhite, rightWhite);
        msleep(1000);
        printf("Press right button to accept, left button to reject.\n");
        while(!right_button() && !left_button()) {}
        if (right_button()) 
        {
            accept = 1;
        }
    }

    msleep(1000);
    accept = 0;
    while (!accept) 
    {
        printf("\n\n------------------------\n\nMove tophat sensors over black area of board.\nPress right button to set.\n");
        while (!right_button()) {}
        leftBlack = analog(LEFT_LINE);
        rightBlack = analog(RIGHT_LINE);
        printf("Left tophat value: %d, Right tophat value: %d\n", leftBlack, rightBlack);
        msleep(1000);
        printf("Press right button to accept, left button to reject.\n");
        while(!right_button() && !left_button()) {}
        if (right_button()) 
        {
            accept = 1;
        }
    }
}

int white, black;
#define TOPHAT $
void setupTophatSingle() 
{
    int accept = 0;
    while (!accept) 
    {
        printf("\n\n------------------------\n\nMove tophat sensor over white area of board.\nPress right button to set.\n");
        while (!right_button()) {}
        white = analog(TOPHAT);
        printf("Tophat value: %d\n", white);
        msleep(1000);
        printf("Press right button to accept, left button to reject.\n");
        while(!right_button() && !left_button()) {}
        if (right_button()) 
        {
            accept = 1;
        }
    }

    msleep(1000);
    accept = 0;
    while (!accept) 
    {
        printf("\n\n------------------------\n\nMove tophat sensor over black area of board.\nPress right button to set.\n");
        while (!right_button()) {}
        black = analog(TOPHAT);
        printf("Tophat value: %d\n", black);
        msleep(1000);
        printf("Press right button to accept, left button to reject.\n");
        while(!right_button() && !left_button()) {}
        if (right_button()) 
        {
            accept = 1;
        }
    }
} 

//servo_slow

void servo_slow(int port, int dest)
{
    int pos = get_servo_position(port);
    if (dest > pos) 
    {
        while (pos < dest) 
        {
            pos += 1;
            set_servo_position(port, pos);
            msleep(1);
        }
    }
    else
    {
        while (pos > dest) 
        {
            pos -= 1;
            set_servo_position(port, pos);
            msleep(1);
        }
    }
}

void servo_slow_2(int port1, int dest1, int port2, int dest2)
{
    //slow servo with 2 servos
    int pos1 = get_servo_position(port1);
    int dir1 = dest1 > pos1 ? 1 : -1;
    int pos2 = get_servo_position(port2);
    int dir2 = dest2 > pos2 ? 1 : -1;

    void move_servo1() {
        while (dir1*pos1 < dir1*dest1) {
            pos1 += dir1*1;
            set_servo_position(port1, pos1);
            msleep(1);
        }
    }

    void move_servo2() {
        while (dir2*pos2 < dir2*dest2) {
            pos2 += dir2*1;
            set_servo_position(port2, pos2);
            msleep(1);
        }
    }

    thread move_servo1_thread = thread_create(move_servo1);
    thread_start(move_servo1_thread);

    move_servo2();

    thread_wait(move_servo1_thread);
    thread_destroy(move_servo1_thread);

}
