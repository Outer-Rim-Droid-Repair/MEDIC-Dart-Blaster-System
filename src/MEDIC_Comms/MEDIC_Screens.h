#ifndef MEDIC_Screens_h
#define MEDIC_Screens_h

#include "Arduino.h"
#include <Adafruit_SSD1306.h>
#include "MEDIC_Comms.h"


class Screen {
    public:
        Screen() {};
        Screen(Adafruit_SSD1306 screen_obj, MEDIC_CONNTROLLER *controller);
        virtual void drawBackgrond() {}
        virtual void drawInfo() {}

        void drawTestPattern(void);
        void invertSection(int x1, int y1, int x2, int y2);
        void drawQuestionBox(char *question);

    protected:
        Adafruit_SSD1306 _screen_obj;
        int _width;
        int _height;
        MEDIC_CONNTROLLER _controller;

    private:

};

class Version_Screen: public Screen {
    public:
        Version_Screen(): Screen() {}
        Version_Screen(Adafruit_SSD1306 screen_obj, MEDIC_CONNTROLLER *controller): Screen(screen_obj, controller) {}
        void drawBackgrond() override;
        void drawInfo(char *ControllerVersion, char *powerBoardVersion, char *FireControlVersion, char *ChronoVersion);

    };

class Chrono_Screen: public Screen {
    public: 
        Chrono_Screen(): Screen() {}
        Chrono_Screen(Adafruit_SSD1306 screen_obj, MEDIC_CONNTROLLER *controller): Screen(screen_obj, controller) {}
        void drawBackgrond() override;
        void drawInfo() override;

};

class Fire_Control_Screen: public Screen {
    public:
        Fire_Control_Screen(): Screen() {}
        Fire_Control_Screen(Adafruit_SSD1306 screen_obj, MEDIC_CONNTROLLER *controller): Screen(screen_obj, controller) {}
        void drawBackgrond() override;
        void drawInfo() override;

};

#endif