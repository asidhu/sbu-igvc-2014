#ifndef _MOTORCTRL_H
#define _MOTORCTRL_H

// simply disable safety and adjust left and right power to control motors
// powers range from [1,-1]
class motorctrl{
	public:
	double left_power, right_power;
	bool safety;	
	double throttle;
};

#endif
