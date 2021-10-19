#include "MapToMat.h"

#include <opencv2/core.hpp>

#include <gtest/gtest.h>
#include <functional>

class IdOperation {
public:
  int operator()(int x) const {
    return x;
  }
};

template<class T>
class IdOperationT {
public:
  T operator()(const T &x) const {
    return x;
  }
};

int idOpFunc(int x) {
  return x;
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryIdenticalLambda) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  cv::Mat idM = mapToMat<std::function<int(int)>, int>(m,
                                                       [](int x) -> int { return x; });
  EXPECT_EQ(cv::countNonZero(m != idM), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryIdenticalLambdaShort) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  cv::Mat idM = mapToMat<int>(m, [](int x) -> int { return x; });
  EXPECT_EQ(cv::countNonZero(m != idM), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryIdenticalStdFunc) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  std::function<int(int)> idOp = std::function<int(int)>(
      [](int x) -> int { return x; });
  cv::Mat idM = mapToMat<std::function<int(int)>, int>(m, idOp);
  EXPECT_EQ(cv::countNonZero(m != idM), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryIdenticalStdFunc2) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  cv::Mat idM = mapToMat<std::function<int(int)>, int>(m, idOpFunc);
  EXPECT_EQ(cv::countNonZero(m != idM), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryIdenticalStdFuncShort) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  std::function<int(int)> idOp = std::function<int(int)>(
      [](int x) -> int { return x; });
  cv::Mat idM = mapToMat<int>(m, idOp);
  EXPECT_EQ(cv::countNonZero(m != idM), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryIdenticalStdFuncShort2) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  cv::Mat idM = mapToMat<int>(m, idOpFunc);
  EXPECT_EQ(cv::countNonZero(m != idM), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryIdenticalClass) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  IdOperation idOp;
  cv::Mat idM = mapToMat<IdOperation, int>(m, idOp);
  EXPECT_EQ(cv::countNonZero(m != idM), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryIdenticalClassShort) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  cv::Mat idM = mapToMat<IdOperation, int>(m);
  EXPECT_EQ(cv::countNonZero(m != idM), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryIdenticalTemplateClass) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  IdOperationT<int> idOp;
  cv::Mat idM = mapToMat<IdOperationT<int>, int>(m, idOp);
  EXPECT_EQ(cv::countNonZero(m != idM), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryIdenticalTemplateClassShort) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  cv::Mat idM = mapToMat<IdOperationT<int>, int>(m);
  EXPECT_EQ(cv::countNonZero(m != idM), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatUnaryInc) {
  cv::Mat m(3, 3, CV_32SC1, 1);
  cv::Mat inc(3, 3, CV_32SC1, 2);
  std::function<int(int)> increment = std::function<int(int)>(
      [](int x) -> int { return x + 1; });
  cv::Mat res = mapToMat<std::function<int(int)>, int>(m, increment);
  EXPECT_EQ(cv::countNonZero(res != inc), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatDouble) {
  cv::Mat m(3, 3, CV_64FC1, 1);
  cv::Mat inc(3, 3, CV_64FC1, 2);
  //std::function<int(int)> increment = std::function<double(do)>([](int x)->int{ return x + 1;});
  cv::Mat res = mapToMat<double>(m, [](double x) { return x + 1; });
  EXPECT_EQ(cv::countNonZero(res != inc), 0);
}

TEST(PolarizationAlgorithmsCommon, mapToMatMatExpr) {
  cv::Mat m(3, 3, CV_64FC1, 1);
  cv::Mat inc(3, 3, CV_64FC1, 3);
  //std::function<int(int)> increment = std::function<double(do)>([](int x)->int{ return x + 1;});
  cv::Mat res = mapToMat<double>(m + m, [](double x) { return x + 1; });
  EXPECT_EQ(cv::countNonZero(res != inc), 0);
}

TEST(PolarizationAlgorithmsCommon, mapFunctorToMat) {
  class F : public MapMatFunctorBase<double> {
  public:
    int cols() override {
      return x.cols;
    }

    F(const cv::Mat &x, const cv::Mat &y, cv::Mat *result)
        : MapMatFunctorBase<double>(result), x(x), y(y) {
    }

    double operator()(int j, int i) override {
      return x.at<double>(j, i) + y.at<double>(j, i);
    }

  private:
    const cv::Mat &x;
    const cv::Mat &y;
  };
  cv::Mat m(3, 3, CV_64FC1, 1);
  cv::Mat inc(3, 3, CV_64FC1, 2);
  cv::Mat res(3, 3, CV_64FC1);
  F operation(m, m, &res);
  cv::parallel_for_(cv::Range(0, m.rows), Apply2DFunctor<double>(operation));
  EXPECT_EQ(cv::countNonZero(res != inc), 0);
}

