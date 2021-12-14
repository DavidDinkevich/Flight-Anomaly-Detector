
#include "HybridAnomalyDetector.h"


HybridAnomalyDetector::HybridAnomalyDetector() {

}

HybridAnomalyDetector::~HybridAnomalyDetector() {

}

void HybridAnomalyDetector::learnNormal(const TimeSeries &ts) {
    SimpleAnomalyDetector::learnNormal(ts);
    for (const auto &corrPair: getAllCorrelations()) {
        if (corrPair.corrlation >= 0.5f && corrPair.corrlation < minCorrelation) {
            // Data vector of feature at index i
            vector<float> f1Data = ts.getFeatureValues(corrPair.feature1.c_str());
            // Data vector of feature at index corrFeatureIndex
            vector<float> f2Data = ts.getFeatureValues(corrPair.feature2.data());
            // Create points array
            vector<Point*> points(ts.getDataLength());
            for (int j = 0; j < ts.getDataLength(); j++) {
                points[j] = new Point(f1Data[j], f2Data[j]);
            }
            // Compute minimum enclosing circle
            Circle minCircle = findMinCircle(points.data(), points.size());
            // Add correlation
            cf.emplace_back(correlatedFeatures{
                    corrPair.feature1, corrPair.feature2, corrPair.corrlation,
                    {}, minCircle.radius * 1.1f, minCircle});
            // Free points array
            for (Point *p : points) {
                delete p;
            }
        }
    }
}