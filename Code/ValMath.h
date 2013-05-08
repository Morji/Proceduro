/*************************************************************
ValMath header: Supplies math functions 

Author: Valentin Hinov
Date: 07/03/2013
Version: 1.0

Exposes: map function, clamp function
**************************************************************/

#ifndef _H_VALMATH_H
#define _H_VALMATH_H

#include <Windows.h>

template <typename T, typename U> 
inline T mapValue(T  x, T in_min, T in_max, U out_min, U out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline long clamp(long x, long minVal, long maxVal){
  return min(max(x,minVal),maxVal);
}

#endif