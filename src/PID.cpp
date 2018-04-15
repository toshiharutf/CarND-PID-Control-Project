#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double parameters[3]) {
    p[0] = parameters[0];  // Kp
    p[1] = parameters[1];  // Ki
    p[2] = parameters[2];  // Kd
    nP = sizeof(p)/sizeof(double);
    
    lastError = 0;
    integralError = 0;
    counter = 0;
    counterMax = 1000;
    tryout = 0;
    tryoutMax = 15;
    
    // tuning parameters
    bestError = 1E10;
    totalError = 0;
    tuningCounter = 0; // 0 -< Kp tuning, 1 -> Ki tuning
    restartFlag = false;
    
    dp[0] = 0.01;
    dp[1] = 0.00005;
    dp[2] = 0.01;
 
    /*
    dp[0] = 0.1;
    dp[1] = 0.00005;
    dp[2] = 0.01;
    */
}

double PID::Control(double params[3], double error) {
    double steering = -params[0]*error - (error-lastError)*params[2] - integralError*params[1];
    integralError += error;
    lastError = error;
    totalError += error*error;
    
    // steering limits
    if(steering >1) steering = 1;
    if(steering < -1) steering = -1;
    
    return steering;
}

// A simplified version of the Twiddle algorithm was implemented here
double PID::Tuning(double params[3], double error){
    
    double steering = -p[0]*error - (error-lastError)*p[2] - integralError*p[1];
    integralError += error;
    lastError = error;
    totalError += error*error;
    
    
    if(tuningCounter > (nP-1)){ tuningCounter = 0;}
     
    if(counter>counterMax){
        counter = 0;
        if(totalError/counterMax < bestError){
            bestError = totalError/counterMax;
            p[tuningCounter] += dp[tuningCounter];
            cout << "Increasing p " << p[tuningCounter] << endl;
        }
        else{
            p[tuningCounter] -= dp[tuningCounter];
            cout << "Decreasing p " << p[tuningCounter] << endl;
        }
        tuningCounter++;
        p[tuningCounter] += dp[tuningCounter];
        restartFlag = true;
        tryout++;
        lastError = 0;
        integralError = 0;
        
            
    }
    counter++;
    
    if(steering >1) steering = 1;
    if(steering < -1) steering = -1;
    return steering;
}

/*
    for(int j = 0; j<1; ++j){
        
        for(int i = 0; i <3; ++i){
            pid.p[i] += pid.dp[i];
            int status = run(pid, p);
            
            if(pid.totalError < pid.bestError){
                pid.bestError = pid.totalError;
                pid.dp[i] *=1.1;
            }
            else{
                pid.p[i] -= 2*pid.dp[i];
                int status = run(pid, p);
                if( (pid.p[i]>0) && (pid.totalError < pid.bestError) ){
                    pid.bestError = pid.totalError;
                    pid.dp[i] *=1.1;
                }
                else{
                    pid.dp[i]*=0.9;
                    pid.p[i] += pid.dp[i];
                }
            }
            std::cout << "Parameters: " << pid.p << std::endl;
            myFile<< pid.p[0] << ", " << pid.p[1] << ", " << pid.p[2] << std::endl;
                  
        }
    }
*/

double PID::TotalError() {
}

