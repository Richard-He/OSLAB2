#include "Random.h"

void RandomGen::readfile(string &path) {
    ifstream infile(path);
    string line;
    getline(infile,line);
    n = stoi(line);
    for(int i=1 ;i<n;i++){
        getline(infile, line);
        randomnumb.push_back(stoi(line));
    }
}


RandomGen::RandomGen(string &path) {
    readfile(path);
    size = randomnumb.size();
    offset = 0;
}

int RandomGen::random(int burst){
    int retval = 1 + (randomnumb[offset] % burst);
    offset = ( offset + 1 ) % size;
    return retval;
}

