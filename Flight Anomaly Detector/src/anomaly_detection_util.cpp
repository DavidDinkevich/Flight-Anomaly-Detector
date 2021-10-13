/*
 * anomaly_detection_util.cpp
 *
 * Author: 584698174 David Dinkevich
 */

#include <cmath>
#include "anomaly_detection_util.h"

float avg(float *x, int size) {
    // So as not to divide by zero
    if (size == 0)
    	return 0.0f;
    float sum = 0.0f;
    for (int i = 0; i < size; i++) {
        sum += x[i];
    }
    return sum / (float) size;
}

// returns the variance of X
float var(float *x, int size){
    // Don't divide by zero!
    if (size == 0)
        return 0.0f;
    // Calculate the sum of the array, and sum of the squares of the array
    float sum = 0.0f;
    float sumSquares = 0.0f;
    for (int i = 0; i < size; i++) {
        sum += x[i];
        sumSquares += (x[i] * x[i]);
    }
    // Calculate variance
    float mu = sum / (float) size;
    return (sumSquares / (float) size) - (mu * mu);
//    float mu = (1 / (float) size) * sum;
//    return (1 / (float) size) * sumSquares - mu * mu;
}

// returns the covariance of X and Y
float cov(float *x, float *y, int size) {
    // Don't divide by 0!
    if (size == 0)
        return 0.0f;
    // Calculate covariance
    float avgX = avg(x, size);
    float avgY = avg(y, size);
    float res = 0.0f;
    for (int i = 0; i < size; i++) {
        res += ((x[i] - avgX) * (y[i] - avgY));
    }
	return res / (float)size;
}

// returns the Pearson correlation coefficient of X and Y
float pearson(float *x, float *y, int size) {
    if (size == 0) // Don't div by zero
        return 0.0f;
    float xStandardDev = std::sqrt(var(x, size));
    float yStandardDev = std::sqrt(var(y, size));
    return cov(x, y, size) / (xStandardDev * yStandardDev);
}

// performs a linear regression and returns the line equation
Line linear_reg(Point **points, int size) {
    // Don't div by zero
    if (size == 0)
        return Line(0.0f, 0.0f);
    // Separate points into x and y arrays
    float *xVals = new float[size];
    float *yVals = new float[size];
    for (int i = 0; i < size; i++) {
        xVals[i] = points[i]->x;
        yVals[i] = points[i]->y;
    }
    // Calculate slope
    float slope = cov(xVals, yVals, size) / var(xVals, size);
    // Calculate y-intercept
    float yInt = avg(yVals, size) - (slope * avg(xVals, size));
    // Free memory
    delete[] xVals;
    delete[] yVals;
    return Line(slope, yInt);
}

// returns the deviation between point p and the line equation of the points
float dev(Point p, Point **points, int size) {
	return dev(p, linear_reg(points, size));
}

// returns the deviation between point p and the line
float dev(Point p, Line l) {
	return std::abs(l.f(p.x) - p.y);
}




