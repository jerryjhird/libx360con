#pragma once

struct xbox360_msg {
    // data[0-1]
    unsigned int type :8; // always 0
    unsigned int length :8; // always 0x14

    // data[2]
    unsigned int dpad_up :1;
    unsigned int dpad_down :1;
    unsigned int dpad_left  :1;
    unsigned int dpad_right :1;
    unsigned int start :1;
    unsigned int back  :1;
    unsigned int thumb_l :1;
    unsigned int thumb_r :1;

    // data[3]
    unsigned int lb :1;
    unsigned int rb :1;
    unsigned int guide :1;
    unsigned int dummy1 :1; // always 0
    unsigned int a :1; // green
    unsigned int b :1; // red
    unsigned int x :1; // blue
    unsigned int y :1; // yellow

    // data[4-5] 
    unsigned int lt :8;
    unsigned int rt :8;

    // data[6-9] 
    int x1 :16;
    int y1 :16;

    // data[10-13]
    int x2 :16;
    int y2 :16;

    // data[14-19]
    unsigned int dummy2 :32; // always 0
    unsigned int dummy3 :16; // always 0
} __attribute__((__packed__));

static inline int is_xbox360_msg(const unsigned char *data, int size) {
    if (size < (int)sizeof(struct xbox360_msg)) return 0;
    return (data[0] == 0x00 && data[1] == 0x14); // type 0, length 0x14
}
