#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>
#include <fstream>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  std::ofstream myfile;
  myfile.open ("parameters.txt");
  
  uWS::Hub h;
  PID steerPID;
  PID gasPID;
  
  // Comfort mode
  //double pSteer[3] = {0.12,0.00001,1.5};
  //double pSpeed[3] = {0.5,0.0,4}; 
  
  // Sports mode
  double pSteer[3] = {0.06,0.00001,1.1};
  double pSpeed[3] = {1.0,0.0,4.0}; 
  
  steerPID.Init(pSteer);
  gasPID.Init(pSpeed);
  // TODO: Initialize the steerPID variable.

  h.onMessage([&steerPID, &gasPID, &myfile](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          double gas;
//////////////////////////////////////////////////////////////////////////////////
          //std::cout << "Speed: " << speed << std::endl;
          
          steer_value = steerPID.Control(steerPID.p, cte);
          
          // If the car turns at 100% to either left or right, the cars brakes at 100%, aso

          double errorSpeed = speed/100.0 - (1.0-2*fabs(steer_value));
          gas = speed/100.0 + gasPID.Control(gasPID.p,errorSpeed);
          if (gas > 1.0) gas = 1.0;
          if(gas < -1.0) gas =-1.0;

          //steer_value = steerPID.Tuning(steerPID.p, cte);
          
          // The simulator resets, but only if the Tuning function is called
          if(steerPID.restartFlag){
              // Write the parameters in the datalog file
            myfile << steerPID.p[0] << ", " << steerPID.p[1] << ", " << steerPID.p[2] << ", " << std::endl;
            std::string msg = "42[\"reset\",{}]";
            ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
            steerPID.restartFlag = false;
          }
 ///////////////////////////////////////////////////////////////////////////////         
          else{
            // DEBUG
            std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;
            json msgJson;
            msgJson["steering_angle"] = steer_value;
            //msgJson["throttle"] = 0.4;
            msgJson["throttle"] = gas;
            auto msg = "42[\"steer\"," + msgJson.dump() + "]";
            std::cout << msg << std::endl;
            ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
          }
          
          if(gasPID.tryout > gasPID.tryoutMax*gasPID.nP){myfile.close(); }
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}


