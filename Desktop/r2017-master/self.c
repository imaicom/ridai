// cc -o self self.c -lwiringPi -lm
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#include <wiringPi.h>
#include <softPwm.h>
#include <softTone.h>

#include "controller.h"
#include <math.h>


struct ps3ctls {
    int fd;
    unsigned char nr_buttons;   // Max number of Buttons
    unsigned char nr_sticks;    // Max number of Sticks
    short *button;          // button[nr_buttons]
    short *stick;           // stick[nr_sticks]
};

int fds;
int fds2;
int mode = 10;
int btn_tri = 0;
int b_btn_tri = 0;
int btn_cir = 0;
int b_btn_cir = 0;
int btn_squ = 0;
int b_btn_squ = 0;
int btn_up = 0;
int b_btn_up = 0;
int btn_down = 0;
int b_btn_down = 0;
int btn_cross = 0;
int b_btn_cross = 0;

int btn_r1 = 0;
int b_btn_r1 = 0;
int btn_r2 = 0;
int b_btn_r2 = 0;

int rotation = 0;
int f = 1;

int automatic() {

	if(digitalRead(15)==0) {
		delay(500);
		if(digitalRead(15)==0) {f = 0;};
		if(digitalRead(15)!=0) {

			softPwmWrite( 1,30);	// １回転
			delay(1000);
			while(digitalRead(4));
			softPwmWrite( 1,0);
			delay(500);

			softPwmWrite( 1,30);	// １回転
			delay(1000);
			while(digitalRead(4));
			softPwmWrite( 1,0);
			delay(500);

			softPwmWrite( 5,50);	// 前進
			softPwmWrite(27,50);
			delay(500);
			softPwmWrite( 5,0);
			softPwmWrite(27,0);
			delay(500);

			softPwmWrite( 6,50);	// 後進
			softPwmWrite(26,50);
			delay(500);
			softPwmWrite( 6,0);
			softPwmWrite(26,0);
			delay(500);
		};

		//system("python /home/pi/r2017/simplebeep.py");
	} else {
		softPwmWrite( 1,0);
	};
}	// automatic()

int ps3c_test(struct ps3ctls *ps3dat) {

    int i;
    unsigned char nr_btn = ps3dat->nr_buttons;
    unsigned char nr_stk = ps3dat->nr_sticks;
    int xx,yy,ph,x,y,z,p,c1,c2,c3,c4,v1,v2,ww;

    printf(" s7=%4d ",digitalRead(7));   // 左床センサ
    printf(" s0=%4d ",digitalRead(0));
    printf(" s2=%4d ",digitalRead(2));
    printf(" s3=%4d ",digitalRead(3));   // 中心床センサ
    printf(" s12=%3d ",digitalRead(12));
    printf(" s13=%3d ",digitalRead(13));
    printf(" s14=%3d ",digitalRead(14));   // 右床センサ
    printf(" s15=%3d ",digitalRead(15));   // タクトスイッチ
    printf(" s16=%3d ",digitalRead(16));   // エンコーダ
    printf(" s1=%4d ",digitalRead(4));   // 大回転センサ
    printf("\n");

    automatic();

//	if((digitalRead(4)!=rotation)&&(digitalRead(4)==0)) system("python /home/pi/r2017/simplebeep.py");
	rotation = digitalRead(4);



    if(ps3dat->button[PAD_KEY_LEFT] ) { digitalWrite(23,1); };
    if(ps3dat->button[PAD_KEY_RIGHT]) { digitalWrite(23,0); };
    if(ps3dat->button[PAD_KEY_UP]   ) { digitalWrite(25,1);delay(2);digitalWrite(25,0);delay(2); };


    if(ps3dat->button[PAD_KEY_START]) {
//        system("mpg123 /home/pi/Music/shuu.mp3 &");
        softPwmWrite( 5,0);
        softPwmWrite( 6,0);
        softPwmWrite(26,0);
        softPwmWrite(27,0);
        softPwmWrite(28,0);
        softPwmWrite(29,0);
        softPwmWrite(24,0);
        softPwmWrite(25,0);
        softPwmWrite(14,0);
        softPwmWrite(23,0);
        delay(3000);
        return -1; // end of program
    };

    return 0;
}


int ps3c_input(struct ps3ctls *ps3dat) {

    int rp;
    struct js_event ev;

    do {
        rp = read(ps3dat->fd, &ev, sizeof(struct js_event));
        if (rp != sizeof(struct js_event)) {
            return -1;
        }
    } while (ev.type & JS_EVENT_INIT);

    switch (ev.type) {
        case JS_EVENT_BUTTON:
            if (ev.number < ps3dat->nr_buttons) {
                ps3dat->button[ev.number] = ev.value;
            }
            break;
        case JS_EVENT_AXIS:
            if (ev.number < ps3dat->nr_sticks) {
                ps3dat->stick[ev.number] = ev.value / 200; // 327 range -32767 ~ +32768 -> -100 ~ +100
            }
            break;
        default:
            break;
    }

    return 0;
}


int ps3c_getinfo(struct ps3ctls *ps3dat) {

    if(ioctl(ps3dat->fd , JSIOCGBUTTONS , &ps3dat->nr_buttons) < 0) return -1;
    if(ioctl(ps3dat->fd , JSIOCGAXES    , &ps3dat->nr_sticks ) < 0) return -2;

    return 0;
}


int ps3c_init(struct ps3ctls *ps3dat, const char *df) {

    unsigned char nr_btn;
    unsigned char nr_stk;
    unsigned char *p;
    int i;

    ps3dat->fd = open(df, O_RDONLY);
    if (ps3dat->fd < 0) return -1;

    if (ps3c_getinfo(ps3dat) < 0) {
        close(ps3dat->fd);
        return -2;
    }

    nr_btn = ps3dat->nr_buttons;
    nr_stk = ps3dat->nr_sticks;
//  printf("%d %d\n",nr_btn,nr_stk);

    p = calloc(nr_btn + nr_stk , sizeof(short));
    if (p == NULL) {
        close(ps3dat->fd);
        return -3;
    }
    ps3dat->button = (short *)p;
    ps3dat->stick  = (short *)&p[nr_btn * sizeof(short)];

    return 0;
}

void ps3c_exit   (struct ps3ctls *ps3dat) {

    free (ps3dat->button);
    close(ps3dat->fd);
}


void main() {

    char *df = "/dev/input/js0";
    struct ps3ctls ps3dat;
    long int i,j;

    wiringPiSetup();
    softPwmCreate( 5,0,20); // motor-1 20ms   // 左？モータ
    softPwmCreate( 6,0,20); // motor-1 20ms
    softPwmCreate(26,0,20); // motor-2 20ms   // 右？モータ
    softPwmCreate(27,0,20); // motor-2 20ms
    softPwmCreate( 1,0,20); // motor-3 20ms   // 大回転モータ

    pinMode(23,OUTPUT);	// センササーボ 0:収納 1:出し
    pinMode(25,OUTPUT);	// ブザー

    pinMode(7,INPUT);	// センサ
    pinMode(0,INPUT);
    pinMode(2,INPUT);
    pinMode(3,INPUT);
    pinMode(12,INPUT);
    pinMode(13,INPUT);
    pinMode(14,INPUT);

    pinMode(15,INPUT);	// タクトスイッチ
	pinMode(16,INPUT);	// エンコーダ
	pinMode(4,INPUT);	// 大回転センサ


 //   system("mpg123 /home/pi/Music/move_it.mp3 &");
	system("python /home/pi/r2017/simplebeep.py");

    if(!(ps3c_init(&ps3dat, df))) {

        do {
            if (ps3c_test(&ps3dat) < 0) break;
        } while (!(ps3c_input(&ps3dat)));

        ps3c_exit(&ps3dat);
    } else while(f) automatic();
}

