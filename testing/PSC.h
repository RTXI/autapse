#ifndef PSC_H
#define PSC_H

#include <vector>

using namespace std;

class PSC {
    
public:
    PSC(double d);
    PSC(double gm, double es, double rs, double fl, double d, double del);
    ~PSC(void);

    double update(double, double);
    int isSpiking();
    void setState(int);
    void setGMax(double);
    void setESyn(double);
    void resetTimeConst();
    void setDelay(double);

private:

    double exp1;
    double exp2;

    double gmax;
    double esyn;
    double PSCrise;
    double PSCfall;

    double peakval;
    double fact1;
    double fact2;

    double delay;
    vector <double> timers;
    
    int state;
    double dt;
};

#endif  // PSC_H
