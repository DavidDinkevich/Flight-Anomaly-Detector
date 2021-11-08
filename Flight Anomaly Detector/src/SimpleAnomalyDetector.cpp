
#include "SimpleAnomalyDetector.h"

using namespace std;

SimpleAnomalyDetector::SimpleAnomalyDetector() {
}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
}


void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts) {
    vector<string> names = ts.getFeatureNames();
    for (int i = 0; i < ts.getNumFeatures(); i++) {
        float maxCorr = 0;
        int corrFeatureIndex = -1;
        // Data vector of feature at index i
        vector<float> f1Data = ts.getFeatureValues(names[i].c_str());
        for (int j = i + 1; j < ts.getNumFeatures(); j++) {
            // Data vector of feature at index j
            vector<float> f2Data = ts.getFeatureValues(names[j].c_str());
            float corr = fabs(pearson(f1Data.data(), f2Data.data(), ts.getDataLength()));
            if (corr > maxCorr && corr <= 1.0f) {
                maxCorr = corr;
                corrFeatureIndex = j;
            }
        }
        const float MIN_CORR = 0.9f;
        if (corrFeatureIndex >= 0 && maxCorr >= MIN_CORR) {
            // Data vector of feature at index corrFeatureIndex
            vector<float> f2Data = ts.getFeatureValues(names[corrFeatureIndex].data());
            // Create points array
            vector<Point*> points(ts.getDataLength());
            for (int j = 0; j < ts.getDataLength(); j++) {
                points[j] = new Point(f1Data[j], f2Data[j]);
            }
            Line regressionLine = linear_reg(points.data(), ts.getDataLength());
            // Calculate threshold--max distance of point from line.
            float maxDev = 0.0f;
            for (int j = 0; j < ts.getDataLength(); j++) {
                maxDev = max(maxDev, dev(*points[j], regressionLine));
            }
            // Add correlation
            cf.emplace_back(correlatedFeatures{
                names[i], names[corrFeatureIndex], maxCorr, regressionLine, maxDev * 1.2f });

            // Free points array
            for (Point *p : points) {
                delete p;
            }
        }
    }
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts) {
    vector<AnomalyReport> reps;
    vector<string> names = ts.getFeatureNames();
    for (int r = 0; r < ts.getDataLength(); r++) {
        for (auto& pair : cf) {
            float x = ts.getFeatureValues(pair.feature1.c_str())[r];
            float y = ts.getFeatureValues(pair.feature2.c_str())[r];
            // If we found an anomalous point
            if (dev({x, y}, pair.lin_reg) > pair.threshold) {
                reps.emplace_back(AnomalyReport { pair.feature1 + "-" + pair.feature2, r+1 });
            }
        }
    }
    return reps;
}

