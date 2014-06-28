#ifndef DATALOGGER_H
#define DATALOGGER_H


#include <vector>
#include <string>
#include <iostream>
#include <fstream>


using namespace std;


class DataLogger 
{ 
public:
    DataLogger(void);
    virtual ~DataLogger(void);

    virtual void insertdata(vector<double>);
    virtual void writebuffer(string, string);
    virtual void newcell(int);
    virtual void resetbuffer();
    virtual void setRecNum(int);

    bool fileExists(string);
    string getTimeStamp();
    
    int getNumCols();
    void setDSRate(int ds);

    void dumpData();

private:

    void create_fns(string);
    void update_log(string);
    string create_path(void);
	
    vector< vector<double> > buffer; 

    int cellnum;
    int recnum;
    string datafn;
    string logfn;
    string timeofday; 

    string path;
    
    int downsample;
    int numcols;
};




#endif //DATALOGGER_H
