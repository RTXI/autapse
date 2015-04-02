#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "autapse.h"

extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new Autapse();
}

static DefaultGUIModel::variable_t vars[] = {
    {
        "VpostSelf",
        "Cells own voltage",
        DefaultGUIModel::INPUT,  //0
    },
    {
        "IsynSelf",
        "Synaptic Output Current to the E Cell in Amps",
        DefaultGUIModel::OUTPUT,  //0
    },
    {
        "G Out",
        "an output to test this thing",
        DefaultGUIModel::OUTPUT,  //1
    },
    {
        "Start G (nS)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "End G (nS)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Rate (nS/sec)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Delay (ms)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Onset Delay (s)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Active?",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER,
    },
    {
        "Acquire?",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER,
    },
    {
        "Cell (#)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER,
    },
    {
        "File Prefix",
        "",
        DefaultGUIModel::COMMENT //| DefaultGUIModel::DOUBLE,
    },
    {
        "File Info",
        "",
        DefaultGUIModel::COMMENT //| DefaultGUIModel::DOUBLE,
    },
    {
        "onset_cnt",
        "",
        DefaultGUIModel::STATE
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

Autapse::Autapse(void)
: DefaultGUIModel("Autapse",::vars,::num_vars), dt(RT::System::getInstance()->getPeriod()*1e-6) {
    
    grate = 1.0;
    gout = gstart = 0.0;
    gend = 2.0;  
    active = 0.0;
    acquire = 0.0; 
    cellnum = 1.0;
    prefix = "autapse";
    info = "n/a";
    delay = 3.0;
    onset_delay = 10;
    onset_cnt = 0;
    const_cnt = 0;
    isconst = 0;

    
    // make this smaller to account for doublets that you often get
    minint = 3;
    spikeState = 0;
    detect = new SpikeDetect(-20.0, minint);
    
    psgSelf = new PSC(gstart, esyn, psgrise, psgfall, dt, delay);

    newdata.push_back(0);
    newdata.push_back(0);
    newdata.push_back(0);
    newdata.push_back(0);
    
    update(INIT);
    refresh();
}

Autapse::~Autapse(void) {
    delete(detect);
    delete(psgSelf);
}

void Autapse::execute(void) {
    
    int myacquire = (int)acquire;
    int myactive = (int)active;
    double iout = 0.0;
    
    // get cell's voltage (convert to mv)
    V = input(0) * 1e3;

    // run conductance ramp
    if (myactive) {

        // if we have passed the onset delay
        if (onset_cnt > (onset_delay - EPS)) {

            // update the spike detector's state, and output
            spikeState = detect->update(V, dt);
            psgSelf->setState((int)spikeState);

            // set the PSC to use the updated gmax
            psgSelf->setGMax(gout);

            // The Current returned by the PSC class is in amps
            iout = psgSelf->update(V, dt);
            output(0) = iout;
            output(1) = gout;

            if (gout <= (gend - EPS) && !isconst) {
                // run a ramp
                gout += grate * dt / 1000;

            } else if (const_cnt < grate && isconst) {
                // run a constant protocol, don't change gout
                // run as long as grate in seconds
                const_cnt += dt / 1000;

            } else {
                // this run is done, set variables back to 0
                gout = 0;
                myactive = 0;
                output(0) = 0.0;
                output(1) = 0.0;

                active = 0;
                const_cnt = 0;
                setParameter("Active?", active);
                refresh();
            }
        } else {
            // increment the delay before the protocol starts
            onset_cnt += dt / 1000;
        }
    }
    
    // log data
    if (myacquire && myactive) {
        newdata[0] = tcnt;
        newdata[1] = V;

        // fudge this so gout is output as 0 until protocol has started
        if (onset_cnt > onset_delay) {
            newdata[2] = gout;
        } else {
            newdata[2] = 0.0;
        }
        
        newdata[3] = iout;
        data.insertdata(newdata);
        tcnt += dt / 1000;
        
    } else if (myacquire && !myactive) {
        // make sure to set aquire to 0 before we write data
        // to stop collecting more while realtime is broken
        tcnt = 0;
        acquire = 0;
        setParameter("Acquire?", 0.0);
        refresh();

        data.writebuffer(prefix, info);
        data.resetbuffer();
    }
}

void Autapse::update(DefaultGUIModel::update_flags_t flag) {

    std::ostringstream o;
    unsigned int pos = 0;

    switch(flag) {
    case PAUSE:
        output(0) = 0.0;
        output(1) = 0.0;
        break;

    case INIT:
        
        setState("spikeState", spikeState);
        
        setParameter("Start G (nS)", gstart);
        setParameter("End G (nS)", gend);
        setParameter("Rate (nS/sec)", grate);
        
        setParameter("Active?",active);
        setParameter("Acquire?",acquire);
        setParameter("Cell (#)",cellnum);
        setComment("File Prefix", QString::fromStdString(prefix));
        setParameter("Delay (ms)", delay);
        setParameter("Onset Delay (s)", onset_delay);

        setState("onset_cnt", onset_cnt);
        
        // set info string 
        o << "[" << onset_delay << " -- " << gstart << ":" << grate 
          << ":" << gend << " -- " << delay << "]";
        info = o.str() + info;
        //cout << info << endl;
        setParameter("File Info", info);

        // check to see if we are running a constant protocol
        if ((gend - gstart) < 0.001) {
            isconst = 1;
        }
        break;
    case MODIFY:
        
        grate   = getParameter("Rate (nS/sec)").toDouble();
        gstart = getParameter("Start G (nS)").toDouble();
        gend   = getParameter("End G (nS)").toDouble();
        delay = getParameter("Delay (ms)").toDouble();
        onset_delay = getParameter("Onset Delay (s)").toDouble();
        
        active = getParameter("Active?").toInt();
        acquire = getParameter("Acquire?").toInt();
        cellnum = getParameter("Cell (#)").toInt();
        prefix = getParameter("File Prefix").toStdSting();
        info = getParameter("File Info").toStdString();
        
        // find the end of the info string (2 characters) if any
        pos = info.find("]");
        if (pos != string::npos) {
            // remove last runs parameter string
            info = info.substr(pos + 1);
            o << "[" << onset_delay << " -- " << gstart << ":" 
              << grate << ":" << gend << " -- " << delay << "]";
            info = o.str() + info;
            setParameter("File Info", QString::fromStdString(info));
        } else {
            // just set a new string
            o << "[" << onset_delay << " -- " << gstart << ":" 
              << grate << ":" << gend << " -- " << delay << "]";
            info = o.str() + info;
            setParameter("File Info", QString::fromStdString(info));
        }
        
        // set gout to be the starting value, if active
        if (active != 0) {
            gout = gstart;
        } else {
            gout = 0;
        }

        tcnt = 0;
        onset_cnt = 0;

        // set the time to run the const protocol to the grate if gstart and end are the same
        if ((gend - gstart) < 0.001) {
            const_cnt = 0;
            isconst = 1;
        } else {
            isconst = 0;
        }

        psgSelf->setGMax(gout);
        psgSelf->setDelay(delay);
        
        // reset data saving
        data.newcell((int)cellnum);
        data.resetbuffer();
        
        break;
    case PERIOD:
        // dt is in
        dt = RT::System::getInstance()->getPeriod()*1e-6;
    default:
        break;
    }
}
