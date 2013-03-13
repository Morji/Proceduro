/*************************************************************
ValMath header: Supplies math functions 

Author: Valentin Hinov
Date: 07/03/2013
Version: 1.0

Exposes: map function
**************************************************************/

#ifndef _H_VALMATH_H
#define _H_VALMATH_H

inline long map(long x, long in_min, long in_max, long out_min, long out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif