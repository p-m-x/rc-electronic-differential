
#ifndef BTS7960_H
#define BTS7960_H

#include "Arduino.h"

class BTS7960
{
public:
    BTS7960(int INH_R, int IN_R, int IS_R, int INH_L, int IN_L, int IS_L, bool DEBUG);
    BTS7960(int INH_R, int IN_R, int INH_L, int IN_L, bool DEBUG);
    void rotate(int value, int dir);
    void begin();
    void stop();   
	
private:

    int toPWM(int v);
    int _inh_R;
	int _in_R;
	int _is_R;
    int _inh_L;
	int _in_L;
	int _is_L;
	bool _debug;
};

#endif
