#pragma once

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace dom
{
  enum class TimingFunctionType
  {
    Linear,
    CubicBezier,
    Steps
  };

  class TimingFunction
  {
  protected:
    TimingFunction(TimingFunctionType type) : type_(type)
    {
    }

  public:
    virtual ~TimingFunction() = default;
    std::unique_ptr<TimingFunction> clone() const;

  public:
    virtual std::string toString() const = 0;
    virtual double evaluate(double fraction) const = 0;
    virtual void range(double *min, double *max) const = 0;

  private:
    TimingFunctionType type_;
  };

  class LinearTimingFunction : public TimingFunction
  {
  public:
    static std::unique_ptr<LinearTimingFunction> Create(std::vector<glm::vec3> points)
    {
      return std::unique_ptr<LinearTimingFunction>(new LinearTimingFunction(std::move(points)));
    }

  private:
    LinearTimingFunction()
        : TimingFunction(TimingFunctionType::Linear)
    {
    }
    explicit LinearTimingFunction(std::vector<glm::vec3> points)
        : TimingFunction(TimingFunctionType::Linear), points_(std::move(points))
    {
    }

  public:
    ~LinearTimingFunction() override = default;

  public:
    std::string toString() const override;
    double evaluate(double fraction) const override;
    void range(double *min, double *max) const override;

  private:
    std::vector<glm::vec3> points_;
  };

  // class CubicBezierTimingFunction : public TimingFunction
  // {
  // };

  // class StepsTimingFunction : public TimingFunction
  // {
  // };
}
