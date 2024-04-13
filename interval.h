#pragma once

#include "math_utils.h"

struct Interval {
public:
    Interval() = default;
    Interval(double min, double max) : _min(min), _max(max) {}

    double getMin() const {
        return _min;
    }
    double getMax() const {
        return _max;
    }
    void setMin(double min) {
        _min = min;
    }
    void setMax(double max) {
        _max = max;
    }

    double size() const {
        return _max - _min;
    }

    bool containsInclusive(double x) const {
        return _min <= x && x <= _max;
    }

    bool containsExclusive(double x) const {
        return _min < x && x < _max;
    }

    static Interval createEmpty() {
        return Interval{INF, -INF};
    }
    static Interval createInfinite() {
        return Interval{-INF, INF};
    }
private:
    double _min;
    double _max;
};