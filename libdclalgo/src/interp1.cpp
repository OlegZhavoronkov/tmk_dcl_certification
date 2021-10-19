#include "interp1.h"

#include <algorithm>

#include "MapToMat.h"

struct Interp1Engine {
    Interp1Engine(const std::vector<double> &x, const std::vector<double> &y) : x(x), y(y) {
        std::vector<double> dx;
        std::vector<double> dy;
        dx.reserve( x.size() );
        dy.reserve( x.size() );
        slope.reserve( x.size() );
        intercept.reserve( x.size() );
        for( size_t i = 0; i < x.size(); ++i ){
            if( i < x.size()-1 )
            {
                dx.push_back( x[i+1] - x[i] );
                dy.push_back( y[i+1] - y[i] );
                slope.push_back( dy[i] / dx[i] );
                intercept.push_back( y[i] - x[i] * slope[i] );
            }
            else
            {
                dx.push_back( dx[i-1] );
                dy.push_back( dy[i-1] );
                slope.push_back( slope[i-1] );
                intercept.push_back( intercept[i-1] );
            }
        }
    }

    double operator()(double newX) {
        auto bound = std::upper_bound(x.begin(), x.end(), newX);
        int idx = bound - x.begin() - 1;
        if(abs(*bound - newX) < abs(*(--bound) - newX))
            --idx;
        return slope[idx] * newX + intercept[idx];
    }

    const std::vector<double> &x;
    const std::vector<double> &y;
    std::vector<double> slope;
    std::vector<double> intercept;
};

std::vector<double> interp1(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &x_new ) {
    std::vector<double> newY;
    newY.reserve(x_new.size());
    Interp1Engine engine(x, y);
    for(double xPoint : x_new) {
        newY.push_back(engine(xPoint));
    }
    return newY;
}

cv::Mat interp1(const std::vector<double> &x, const std::vector<double> &y, const cv::Mat &xNew) {
    Interp1Engine engine(x, y);
    return mapToMat<double>(xNew, [&](double x) {
        return engine(x);
    });
}
