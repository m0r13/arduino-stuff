#ifndef ir44key_h
#define ir44key_h

class IR44Key {
public:
    // special key that is sent when a key is repeating
    static const long SPECIAL_REPEAT = 0xffffffff;

    // all the buttons
    // from top to bottom and left to right
    static const long BRIGHTNESS_UP = 0xff3ac5;
    static const long BRIGHTNESS_DOWN = 0xffba45;
    static const long PLAY_PAUSE = 0xff827d;
    static const long POWER_ON_OFF = 0xff02fd;

    static const long RED = 0x4ab0f7b5;
    static const long GREEN = 0x10755f41;
    static const long BLUE = 0xf87ab5a;
    static const long WHITE = 0xc107b660;

    static const long C01 = 0xff2ad5;
    static const long C02 = 0xffaa55;
    static const long C03 = 0xff926d;
    static const long C04 = 0xff12ed;

    static const long C05 = 0xff0af5;
    static const long C06 = 0xff8a75;
    static const long C07 = 0xffb24d;
    static const long C08 = 0xff32cd;

    static const long C09 = 0xff38c7;
    static const long C10 = 0xffb847;
    static const long C11 = 0xff7887;
    static const long C12 = 0xfff807;

    static const long C13 = 0xff18e7;
    static const long C14 = 0xff9867;
    static const long C15 = 0xff58a7;
    static const long C16 = 0xffd827;

    static const long RED_UP = 0xff28d7;
    static const long GREEN_UP = 0xffa857;
    static const long BLUE_UP = 0xff6897;
    static const long QUICK = 0xffe817;

    static const long RED_DOWN = 0xff08f7;
    static const long GREEN_DOWN = 0xff8877;
    static const long BLUE_DOWN = 0xff48b7;
    static const long SLOW = 0xffc837;

    static const long DIY1 = 0xff30cf;
    static const long DIY2 = 0xffb04f;
    static const long DIY3 = 0xff708f;
    static const long MODE_AUTO = 0xfff00f;

    static const long DIY4 = 0xff10ef;
    static const long DIY5 = 0xff906f;
    static const long DIY6 = 0xff50af;
    static const long MODE_FLASH = 0xffd02f;

    static const long MODE_JUMP3 = 0xff20df;
    static const long MODE_JUMP7 = 0xffa05f;
    static const long MODE_FADE3 = 0xff609f;
    static const long MODE_FADE7 = 0xffe01f;
};

#endif
