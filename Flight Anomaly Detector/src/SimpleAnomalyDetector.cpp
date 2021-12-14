
#include "SimpleAnomalyDetector.h"

using namespace std;

SimpleAnomalyDetector::SimpleAnomalyDetector() {
}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
}

void SimpleAnomalyDetector::calculateAllCorrelations(const TimeSeries &ts) {
    allCorrs.clear();
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
        if (corrFeatureIndex >= 0) {
            CorrelatedPair p = {names[i], names[corrFeatureIndex], maxCorr};
            allCorrs.emplace_back(p);
        }
    }
}

void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts) {
    cf.clear();
    calculateAllCorrelations(ts);
    const float MIN_CORR = minCorrelation;
    for (const auto& corrPair : allCorrs) {
        if (corrPair.corrlation >= MIN_CORR) {
            // Data vector of feature at index i
            vector<float> f1Data = ts.getFeatureValues(corrPair.feature1.c_str());
            // Data vector of feature at index corrFeatureIndex
            vector<float> f2Data = ts.getFeatureValues(corrPair.feature2.data());
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
                    corrPair.feature1, corrPair.feature2, corrPair.corrlation,
                    regressionLine, maxDev * 1.2f, {{0, 0}, 0} });

            // Free points array
            for (Point *p : points) {
                delete p;
            }
        }

    }
}

vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts) {
//    cout << "Calling detect" << endl;
    vector<AnomalyReport> reps;
    vector<string> names = ts.getFeatureNames();
    for (const auto& pair : cf) {
        for (int r = 0; r < ts.getDataLength(); r++) {
            float x = ts.getFeatureValues(pair.feature1.c_str())[r];
            float y = ts.getFeatureValues(pair.feature2.c_str())[r];
            bool isAnomalous = false;
            if (pair.corrlation >= minCorrelation)
                isAnomalous = dev({x, y}, pair.lin_reg) > pair.threshold;
            else if (pair.corrlation >= 0.5f)
                isAnomalous = dist({x, y}, pair.minCircle.center) > pair.threshold;
            if (isAnomalous) {
                reps.emplace_back(AnomalyReport{pair.feature1 + "-" + pair.feature2, r + 1});
//                cout << "Found anomaly: " << r + 1 << " , " << pair.feature1 << "-" << pair.feature2 << endl;
            }
        }
    }
    return reps;
}

