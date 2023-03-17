#pragma once
#include <chrono>

/**
 * \brief basic timer class
 */
struct Timer
{
  Timer() { reset(); }

  /**
   * \brief get time since last reset call
   * \return elapsed time
   */
  float elapsed() const
  {
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - start);
    return static_cast<float>(time_span.count());
  };
  /**
   * \brief reset clock
   */
  void reset() { start = std::chrono::high_resolution_clock::now(); };
  std::chrono::high_resolution_clock::time_point start;
};

