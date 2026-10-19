// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_CALIBRATION_H
#define SUPERJUPITER_CALIBRATION_H
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#define SUPERJUPITER_CALIBRATION_VERSION "0.1.0"
namespace superjupiter {
namespace calibration {
enum class Bounds { Reject, Clamp, Extrapolate };
enum class Status {
  Ok,
  Clamped,
  Extrapolated,
  InvalidTable,
  InvalidInput,
  OutOfRange,
  NumericError
};
struct Point {
  double input;
  double output;
};
struct Result {
  Status status;
  double raw;
  double value;
  uint32_t revision;
  bool valid() const {
    return status == Status::Ok || status == Status::Clamped || status == Status::Extrapolated;
  }
};
class Calibration {
  const Point *points_;
  size_t count_;
  uint32_t revision_;
  Bounds bounds_;

public:
  Calibration(const Point *points, size_t count, uint32_t revision = 0,
              Bounds bounds = Bounds::Reject)
      : points_(points), count_(count), revision_(revision), bounds_(bounds) {}
  bool valid() const {
    if (!points_ || count_ < 2)
      return false;
    for (size_t i = 0; i < count_; ++i)
      if (!std::isfinite(points_[i].input) || !std::isfinite(points_[i].output) ||
          (i && points_[i].input <= points_[i - 1].input))
        return false;
    return true;
  }
  Result evaluate(double raw) const {
    Result r = {Status::InvalidTable, raw, 0, revision_};
    if (!valid())
      return r;
    if (!std::isfinite(raw)) {
      r.status = Status::InvalidInput;
      return r;
    }
    const bool outside = raw < points_[0].input || raw > points_[count_ - 1].input;
    if (outside && bounds_ == Bounds::Reject) {
      r.status = Status::OutOfRange;
      return r;
    }
    if (outside && bounds_ == Bounds::Clamp) {
      r.value = raw < points_[0].input ? points_[0].output : points_[count_ - 1].output;
      r.status = Status::Clamped;
      return r;
    }
    size_t upper = 1;
    while (upper < count_ - 1 && raw > points_[upper].input)
      ++upper;
    const Point &a = points_[upper - 1];
    const Point &b = points_[upper];
    const double span = b.input - a.input, delta = b.output - a.output;
    const double fraction = (raw - a.input) / span;
    r.value = a.output + fraction * delta;
    if (!std::isfinite(span) || !std::isfinite(delta) || !std::isfinite(fraction) ||
        !std::isfinite(r.value)) {
      r.status = Status::NumericError;
      r.value = 0;
      return r;
    }
    r.status = outside ? Status::Extrapolated : Status::Ok;
    return r;
  }
};
} // namespace calibration
} // namespace superjupiter
#endif
