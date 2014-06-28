/**
 * Stand alone data recorder  
 */
 
 
#include "DataLogger.h"
#include <sys/stat.h>



DataLogger::DataLogger(void) 
{
   cellnum=1;
   recnum=1;
   path = "DATA/";
   
   // number of columns in the data set
   numcols = 0;
   
   // do not downsample by default
   downsample = 1;
}


DataLogger::~DataLogger(void)
{
//   buffer.~vector< vector<double> >();
}


void DataLogger::newcell(int num)
{
  if (cellnum != num) {
    cellnum = num;
    recnum = 1;
  }
}
void DataLogger::setRecNum(int rn) {
    recnum=rn;
}

void DataLogger::insertdata(vector<double> newdata) 
{
   buffer.push_back(newdata);
   
   // set column number if we haven't yet
   if (numcols == 0) { 
       numcols = newdata.size();
   }
}


void DataLogger::resetbuffer()
{
   buffer.clear();
   numcols = 0;
}

int DataLogger::getNumCols() {
    return numcols;
}

void DataLogger::setDSRate(int ds) {
    downsample = ds;
}


void DataLogger::writebuffer(string prefix, string fileinfo)
{

    create_fns(prefix);

    // check if file exists already
    if(fileExists(path + datafn)) {
        cout << "Specified Data file " << datafn << " already exists.\n\tAppending timestamp..." << endl;
        datafn = datafn + getTimeStamp();
    }

    // open the file as binary
    ofstream data_file ((path + datafn).c_str(), ios::binary | ios::out);
    //ofstream log_file  (logfn.c_str(), ios::out);

    if (data_file.is_open()) {
        
        // increment i by 'downsample' to downsample data and save space
        //cout << " buffer size(int): "<< (int)buffer.size() << endl;
        //cout << " buffer size[1]: "<< buffer[1].size() << endl;
        //cout << " downsample: "<< downsample << endl;
        for (int i = 0; i < (int)buffer.size(); i+=downsample) {
            for (int j = 0; j < (int)buffer[i].size(); j++) {
                // output a data point
                data_file.write(reinterpret_cast<char*>(&buffer[i][j]),sizeof(double));
                //cout << "i: " << i << " j: " << j << endl;
            }
        }
        recnum++;
        cout << "File " << datafn << " written successfully." << endl;
    } else {
        cout << "Unable to open file " << datafn << endl;
    }

    data_file.close();
    update_log(fileinfo);
}


void DataLogger::update_log(string fileinfo)
{
    ofstream log_file ((path + logfn).c_str(), ios::out | ios::app);
    if (log_file.is_open()) {
        log_file << timeofday<< " " << datafn << ": " << fileinfo << endl;
        cout << "Log File " << logfn << " written successfully." << endl;
    }
    log_file.close();
}


/**
 * Test function to output the vector
 */
void DataLogger::dumpData() {
    
    for (int i = 0; i < (int)buffer.size(); i++) {
        for (int j = 0; j < (int)buffer[i].size(); j++) {
            printf("element %d,%d: %f\n", i, j, buffer[i][j]);
        }
    }
}

    
void DataLogger::create_fns(string prefix)
{
    char tstring[80];
    time_t sec;
    time (&sec);
    strftime(tstring, 80,"%b_%d_%y", localtime(&sec));

    // convert doubles to ints for printing, change cellnum to ANSI char
    char trial[80];
    sprintf(trial, "_%c%d.dat", ((int)cellnum)+64, (int)recnum);
    
    char t_o_d[80];
    strftime(t_o_d, 80, "%T", localtime(&sec));
    timeofday = string(t_o_d);
    
    datafn = prefix + "_" + string(tstring) + string(trial);
    logfn = string(tstring) + ".log";
}

string DataLogger::getTimeStamp() {
    char tstring[80];
    time_t sec;
    time (&sec);
    strftime(tstring, 80,"%H%M%S", localtime(&sec));
    return string(tstring);
}

bool DataLogger::fileExists(string fn) {
    struct stat stf;
    int is;

    is = stat(fn.c_str(), &stf);
    if(is == 0) {
        return true;
    } else {
        return false;
    }
}
