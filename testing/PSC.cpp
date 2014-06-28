/*
* Syn. conductance
* intended to be included into many RTXI models
*/

#include <math.h>
#include <iostream>

#include "PSC.h"

using namespace std;


PSC::PSC(double d) {

    exp1 = 0;
    exp2 = 0;

    // default values for the parameters
    gmax = 0.1;
    esyn = -90.0; 

    // values from Destexhe et. al. 1994
    // adpated from Dayan and Abbot
    PSCrise = 0.3;
    PSCrise = 5.6;
    //PSCrise = 1.68; 
    //PSCfall = 6.21;
    
    state = 0;
    dt = d;

    resetTimeConst();
}

PSC::PSC(double gm, double es, double rs, double fl, double d, double del) {

    exp1 = 0;
    exp2 = 0;

    gmax = gm;
    esyn = es; 
    PSCrise = rs; 
    PSCfall = fl;

    state = 0;
    dt = d;

    delay = del;

    resetTimeConst();
}
    

PSC::~PSC(void) {
}

/**
 * v - post synaptic membrane voltage in volts
 * dt - RTXI system period:  RT::System::getInstance()->getPeriod()*1e-6
 */
double PSC::update(double v, double d) {
    
    // convert to mV
    double vpost = v;
    
    //Assuming 10 mV junction potential
    //#define Vpost (input(0) * 1e3 - 10)
    
    double Iout;
    double sum;
    
    // when model period changes, update the synaptic waveform to compensate
    if (dt != d) {
        dt = d;
        resetTimeConst();
    }
   
    // detect spike
    if (isSpiking()) {

        // add a new timer to the list
        timers.push_back(delay);
    }

    //vector<double>::const_iterator iter;

    //for (iter = timers.begin(); iter != timers.end(); iter++) {
      //  if ((*iter) > 1e-9) {
       //     break;
       // }


    if (timers.size() > 0) {

        if (timers[0] < 1e-9) {
            // increment G function if timer is up
            exp1 += 1/peakval;
            exp2 += 1/peakval;

            // remove timer
            timers.erase(timers.begin());
        }
        for (unsigned int i = 0; i < timers.size(); i++) {
            timers[i] -= dt;
            //cout << "timer[i]: " << timers[i] << endl;
        }
        //cout << endl;
    }

    // compute exponentials
    exp1 = exp1 * fact1;
    exp2 = exp2 * fact2;
    sum = exp2 - exp1;

    Iout = sum * gmax * (esyn - vpost);

    // Iout is in  uA/cm^2 if gmax is in mS/cm^2.
    // converting uA -> A = 1e-6,
    // multiplying by area of typical cell (1e-6 cm^2) gives conversion factor of 1e-12
    // Iout is pA if gmax is nS (for output to a real cell)
    return Iout * 1e-12;
}

/*
 * Called when dt changes
 */
void PSC::resetTimeConst() {

    double rs = PSCrise;
    double fl = PSCfall;

    fact1 = exp (-dt / fl);
    fact2 = exp (-dt / rs);
    peakval = exp((1/(1/fl-1/rs) * log(fl/rs))/rs) - exp ((1/(1/fl-1/rs)*log(fl/rs))/fl);
}


int PSC::isSpiking() {
    if (state == 1) {
        return 1; 
    } else {
        return 0;
    }
}

// set the state of the cell to determine if it is spiking
void PSC::setState(int s) {
    state = s;
}

void PSC::setGMax(double gm) {
    gmax = gm;
}

void PSC::setESyn(double es) {
    esyn = es;
}

void PSC::setDelay(double d) {
    delay = d;
}
