#include "minCircle.h"
#include <cmath>
#include <stdexcept>

// ==========================
// ====  HELPER METHODS  ====
// ==========================

inline float distSq(const Point& a, const Point& b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

inline float dist(const Point& a, const Point& b) {
    return sqrtf(distSq(a, b));
}

Circle getTrivialCircle(const vector<Point> &points, size_t size) {
    switch (size) {
        case 0:
            return {{0, 0}, 0};
        case 1:
            return { points[0], 0 };
        case 2: {
            Point avg((points[0].x + points[1].x) / 2.f, (points[0].y + points[1].y) / 2.f);
            float rad = dist(points[0], points[1]) / 2.f;
            return { avg, rad };
        }
        case 3: {
            float x1 = points[0].x, y1 = points[0].y;
            float x2 = points[1].x, y2 = points[1].y;
            float x3 = points[2].x, y3 = points[2].y;
            float a = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
            float b = (x1 * x1 + y1 * y1) * (y3 - y2)
                    + (x2 * x2 + y2 * y2) * (y1 - y3)
                    + (x3 * x3 + y3 * y3) * (y2 - y1);
            float c = (x1 * x1 + y1 * y1) * (x2 - x3)
                    + (x2 * x2 + y2 * y2) * (x3 - x1)
                    + (x3 * x3 + y3 * y3) * (x1 - x2);
            float d = (x1 * x1 + y1 * y1) * (x3 * y2 - x2 * y3)
                    + (x2 * x2 + y2 * y2) * (x1 * y3 - x3 * y1)
                    + (x3 * x3 + y3 * y3) * (x2 * y1 - x1 * y2);
            float cx = -b/(2.f * a);
            float cy = -c/(2.f * a);
            float r = sqrtf((b * b + c * c - 4 * a * d) / (4 * a * a));
            return {{cx, cy}, r};
        }
        default:
            throw std::runtime_error("Can only construct trivial circle from <= 3 points");
    }

}

Circle welzlHelper(Point **points, vector<Point> &R, size_t size) {
    if (size == 0 || R.size() == 3) {
        return getTrivialCircle(R, R.size());
    }
    Point& p = *points[size - 1];
    vector<Point> copy(R);
    Circle without = welzlHelper(points, copy, size - 1);
    if (distSq(without.center, p) <= without.radius * without.radius)
        return without;
    R.push_back(p);
    return welzlHelper(points, R, size - 1);
}

Circle welzl(Point **points, size_t size) {
    vector<Point> R;
    R.reserve(3);
    return welzlHelper(points, R, size);
}

// ========================
// ====  MAIN METHODS  ====
// ========================

Circle findMinCircle(Point **points, size_t size) {
    return welzl(points, size);
}
