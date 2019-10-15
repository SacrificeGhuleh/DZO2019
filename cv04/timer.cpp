//
// Created by zvone on 03-Oct-19.
//
#include "timer.h"
#include <cmath>
#include <iostream>

void Timer::printElapsed(double elapsed) {
  int eli = static_cast<int>(elapsed);
  int hours = static_cast<int>(std::floor(eli / 3600));
  int minutes = std::floor((eli - (hours * 3600)) / 60);
  int seconds = eli - (minutes * 60 + hours * 3600);
  std::cout << "Elapsed: " << hours << "h " << minutes << "m " << seconds << "s" << std::endl;
}