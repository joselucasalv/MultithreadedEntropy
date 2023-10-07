#include <iostream>
#include <iomanip> //set precision
#include <vector> 
#include <pthread.h> //pthread inclusion
#include <sstream> //stringstream
#include <math.h> //log
using namespace std;

struct entropy { //stores my vectors of tasks, times, and entropy results for each input line
    vector<char> task; 
    vector<int> time; 
    vector<double> result;
};

void* entropyCalc (void* pos_void_ptr) { //entropy calculation
    struct entropy* entr = (struct entropy*) pos_void_ptr; //cast
    
    int NFreq; //variables needed for calculation
    int currFreq = 0;
    double H;
    double currH;
    double currentTerm;
    double newTerm;
    
    for(int i=0; i<entr->time.size(); i++) { //implementing Rincon's algorithm by parsing through tasks and time vectors
        NFreq = currFreq + entr->time.at(i);
        if(NFreq == entr->time.at(i)) {
            H=0;
        } else {
            int freqSelectedTask = 0;
            for(int j=0; j<i; j++) {
                if(entr->task.at(j) == entr->task.at(i))
                    freqSelectedTask += entr->time.at(j); //adds all previous frequencies
            }
            if (freqSelectedTask == 0)
                currentTerm = 0;
            else
                currentTerm = freqSelectedTask * log2(freqSelectedTask);
                
            newTerm = (freqSelectedTask + entr->time.at(i))*
                        log2(freqSelectedTask + entr->time.at(i));

            H = log2(NFreq) - ((log2(currFreq)-currH) *
                (currFreq)-currentTerm + newTerm)/NFreq; //H calculation
        }
        currH = H;
        currFreq = NFreq;
        entr->result.push_back(H); //stores H in result vector
    }
 
    return nullptr;
}

int main() {
    
    vector<entropy> input; //vector that holds entropy() vectors
    string inputline;
    int NENT = 0; //will store number of entropy calculations/size of pthread
    
    while(getline(cin, inputline)) {
        stringstream iss(inputline); //stream input into variables tasks and times
        char tasks;
        int times;
        
        input.push_back(entropy()); //have to push input back with empty entropy struct so .at(NENT) exists
        
        while(iss >> tasks >> times) { //read tasks and times until newline
            input.at(NENT).task.push_back(tasks); //push values into the vectors
            input.at(NENT).time.push_back(times);
        }
        
        NENT++;
    }
    
    pthread_t entr[NENT]; //threads will be of size NENT (number of lines in input)
    
    for(int i=0; i<NENT; i++){
        if(pthread_create(&entr[i], nullptr, entropyCalc, (void*) &input.at(i))) //create my threads and pass by reference my input vector 
		{
			fprintf(stderr, "Error creating thread\n"); //will return if threads were unable to be created
			return 1;
		}
    }
    
    for(int i=0; i<NENT; i++)
        pthread_join(entr[i], nullptr); //have to join all threads created in different loop
        
        
    for(int i=0; i<NENT; i++){ //will print NENT CPUs
        cout << "CPU " << i+1 << endl;
        cout << "Task scheduling information: ";
        for (int j=0; j<input.at(i).task.size(); j++) {
            cout << input.at(i).task.at(j) << "(" << input.at(i).time.at(j) << ")";
            if (j+1 == input.at(i).task.size()) //required to determine if at the end of the tasks
                cout << endl;
            else
                cout << ", ";
        }
        cout << "Entropy for CPU " << i+1 << endl;
        for(int j=0; j<input.at(i).result.size(); j++)
            cout << setprecision(2) << fixed << input.at(i).result.at(j) << " "; //set decimal places to 2 and print results from entropy vector
        cout << endl << endl;
    }
    
    return 0;
}