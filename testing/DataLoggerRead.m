function DataLoggerRead()

myfile = 'test.dat';

% you must know how many channels you recorded
NumChan = 3;

fid = fopen(myfile);
a = fread(fid,[NumChan,inf], 'double');
fclose(fid);
a=a';


