#ifndef MAP_TO_MAT_H
#define MAP_TO_MAT_H

#include <functional>
#include <opencv2/core/utility.hpp>

template<typename T>
class MapMatFunctorBase {
public:
  MapMatFunctorBase(cv::Mat *_result) : result(_result) {
  }

  virtual T operator()(int y, int x) = 0;

  void evaluate(int y, int x) {
    if (result->rows > y && result->cols > x)
      result->at<T>(y, x) = this->operator()(y, x);
  }

  virtual int cols() = 0;

protected:
  cv::Mat *result;
};

template<typename T>
class Apply2DFunctorToRow : public cv::ParallelLoopBody {
private:
  int row;
  MapMatFunctorBase<T> &operation;
public:
  Apply2DFunctorToRow(int _row,
                      MapMatFunctorBase<T> &_operation) : row(_row),
                                                          operation(
                                                              _operation) {
  }

  virtual void operator()(const cv::Range &r) const {
    for (int i = r.start; i != r.end; ++i) {
      operation.evaluate(row, i);
    }
  }
};

template<typename T>
class Apply2DFunctor : public cv::ParallelLoopBody {
private:
  MapMatFunctorBase<T> &operation;
public:
  Apply2DFunctor(MapMatFunctorBase<T> &_operation) : operation(_operation) {
  }

  virtual void operator()(const cv::Range &r) const {
    for (int i = r.start; i != r.end; ++i) {
      cv::parallel_for_(cv::Range(0, operation.cols()),
                        Apply2DFunctorToRow<T>(i, operation));
    }
  }
};

template<class F, typename T>
class OperationToRowApplicatorUnary : public cv::ParallelLoopBody {
private:
  const cv::Mat *m;
  cv::Mat *result;
  int row;
  F operation;
public:
  OperationToRowApplicatorUnary(const cv::Mat *_m, cv::Mat *_result, int _row,
                                F _operation) : m(_m), result(_result),
                                                row(_row),
                                                operation(_operation) {
  }

  virtual void operator()(const cv::Range &r) const {
    for (int i = r.start; i != r.end; ++i) {
      result->at<T>(row, i) = operation.operator()(m->at<T>(row, i));
    }
  }
};

template<class F, typename T>
class OperationApplicatorUnary : public cv::ParallelLoopBody {
private:
  const cv::Mat *m;
  cv::Mat *result;
  F operation;
public:
  OperationApplicatorUnary(const cv::Mat *_m, cv::Mat *_res, F _operation) : m(
      _m), result(_res), operation(_operation) {
  }

  virtual void operator()(const cv::Range &r) const {
    for (int i = r.start; i != r.end; ++i) {
      cv::parallel_for_(cv::Range(0, m->cols),
                        OperationToRowApplicatorUnary<F, T>(m, result, i,
                                                            operation));
    }
  }
};

template<typename T>
cv::Mat mapToMat(const cv::Mat &m, std::function<T(T)> operation) {
  cv::Mat result(m.size(), m.type());
  cv::parallel_for_(cv::Range(0, m.rows),
                    OperationApplicatorUnary<std::function<T(T)>, T>(&m,
                                                                     &result,
                                                                     operation));
  return result;
}

template<class F, typename T>
cv::Mat mapToMat(const cv::Mat &m) {
  F operation;
  cv::Mat result(m.size(), m.type());
  cv::parallel_for_(cv::Range(0, m.rows),
                    OperationApplicatorUnary<F, T>(&m, &result, operation));
  return result;
}

template<class F, typename T>
cv::Mat mapToMat(const cv::Mat &m, F operation) {
  cv::Mat result(m.size(), m.type());
  cv::parallel_for_(cv::Range(0, m.rows),
                    OperationApplicatorUnary<F, T>(&m, &result, operation));
  return result;
}

#endif //MAP_TO_MAT_H
