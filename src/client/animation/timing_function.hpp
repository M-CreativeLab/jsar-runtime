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
    TimingFunction(TimingFunctionType type)
        : type_(type)
    {
    }

  public:
    virtual ~TimingFunction() = default;
    std::unique_ptr<TimingFunction> clone() const;

  public:
    virtual bool isLinear() const
    {
      return false;
    }
    virtual bool isCubicBezier() const
    {
      return false;
    }
    virtual bool isSteps() const
    {
      return false;
    }

    virtual std::string toString() const = 0;
    virtual double evaluate(double fraction) const = 0;
    virtual void range(double *min, double *max) const = 0;

  protected:
    TimingFunctionType type_;
  };

  class LinearTimingFunction : public TimingFunction
  {
  public:
    static std::unique_ptr<LinearTimingFunction> Clone(const TimingFunction *easing)
    {
      auto linear_function = static_cast<const LinearTimingFunction *>(easing);
      return std::unique_ptr<LinearTimingFunction>(new LinearTimingFunction(*linear_function));
    }
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
        : TimingFunction(TimingFunctionType::Linear)
        , points_(std::move(points))
    {
    }
    LinearTimingFunction(const LinearTimingFunction &other)
        : TimingFunction(other.type_)
        , points_(other.points_)
    {
    }

  public:
    ~LinearTimingFunction() override = default;

  public:
    bool isLinear() const override final
    {
      return true;
    }

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
