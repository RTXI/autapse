
/**
* Autapse -- Couple a cell to itself
* I took the PSC and SpikeDetecting from Autapse
* and the DataLogger stuff from Istep
*
* [0:0.1:1] runs a ramp from gmax = 0 to gmax = 1 @ 0.1 per second
* [2:10:2] runs a contant gmax @ 2 for 10 seconds
*/

#include <default_gui_model.h>

#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <QtGui>

#include "SpikeDetect.h"
#include "PSC.h"
#include "DataLogger.cpp"

// epsilon, the fudge factor used to compare doubles
#define EPS 1e-9
using namespace std;

class Autapse : public DefaultGUIModel
{
    public:
    
        Autapse(void);
        virtual ~Autapse(void);
        
        void execute(void);
    
    protected:
    
        void update(DefaultGUIModel::update_flags_t);
    
    private:
    
        double dt;
        double gstart, gend, grate, active, gout, V;
        double delay, onset_delay;
        double onset_cnt, const_cnt;
        int isconst;
        
        // synaptic rev. potentials and kinetics
        const double esyn = 0;
        const double psgrise = 0.3;
        const double psgfall = 5.6;
        
        // spike detector
        SpikeDetect *detect;
        double minint;
        double spikeState;
        
        // psg
        PSC *psgSelf;
        
        // autapse conductance (units?)
        //double gmaxSelf; 
        
        // DataLogger
        DataLogger data;
        double acquire, maxt, tcnt, cellnum;
        string prefix, info;
        vector<double> newdata;
};


