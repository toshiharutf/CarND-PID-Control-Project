#ifndef PID_H
#define PID_H

#include <iostream>
class PID {
public:
  /*
  * Errors
  */
  double lastError;
  double integralError;


  /*
  * Coefficients
  */ 
   
  double p[3];
  double dp[3];
  int nP;
  
  // Total error
  double bestError;
  double totalError;
  int counter;
  int counterMax;
  int tryout;
  int tryoutMax;
  int tuningCounter;
  bool restartFlag;

  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init(double parameters[3]);

  /*
  * Update the PID error variables given cross track error.
  */
  double Control(double params[], double cte);

  /*
  * Calculate the total PID error.
  */
  double TotalError();
  
  double Tuning(double params[3], double error);
};

#endif /* PID_H */
