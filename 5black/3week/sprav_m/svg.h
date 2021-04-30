#pragma once

#include <deque>
#include <map>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace Svg {

template <typename T>
std::string ToString(const T& v) {
  std::ostringstream ss;
  ss << v;
  return ss.str();
}

struct Rgb {
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  std::optional<double> alpha;

  Rgb() = default;
  Rgb(uint8_t r, uint8_t g, uint8_t b)
      : red(r), green(g), blue(b) {}
  Rgb(uint8_t r, uint8_t g, uint8_t b, double a)
      : red(r), green(g), blue(b), alpha(a) {}
};

std::ostream& operator<<(std::ostream&, const Rgb&);

using Color = std::variant<std::monostate, std::string, Rgb>;
extern const Color NoneColor;

std::ostream& operator<<(std::ostream&, const Color&);

struct Point {
  double x = 0;
  double y = 0;

  Point() = default;
  Point(double x, double y)
      : x(x), y(y) {}
};

class Object {
 public:
  Object(std::string type);

  void SetData(std::string);

  template <typename T>
  void SetOption(std::string type, const T& value) {
    SetOptionImpl(move(type), ToString(value));
  }

  void Render(std::ostream&) const;

 private:
  std::string type_;
  std::optional<std::string> data_;
  std::map<std::string, std::string> options_;

  void SetOptionImpl(std::string, std::string);
};

std::ostream& operator<<(std::ostream&, const Object&);

class Document {
 public:
  Document() = default;
  void Add(Object);
  void Render(std::ostream&) const;

 private:
  std::deque<Object> objects_;
};

template <typename C>
class BaseObject {
  using Current_ = C;

 public:
  BaseObject(std::string type)
      : type_(move(type)) {}
  virtual ~BaseObject() = default;

  operator Object() const {
    return MakeObject();
  }

  Current_& SetFillColor(const Color& c) {
    fill_ = c;
    return static_cast<Current_&>(*this);
  }

  Current_& SetStrokeColor(const Color& c) {
    stroke_ = c;
    return static_cast<Current_&>(*this);
  }

  Current_& SetStrokeWidth(double v) {
    stroke_width_ = v;
    return static_cast<Current_&>(*this);
  }

  Current_& SetStrokeLineCap(const std::string& v) {
    stroke_linecap_ = v;
    return static_cast<Current_&>(*this);
  }

  Current_& SetStrokeLineJoin(const std::string& v) {
    stroke_linejoin_ = v;
    return static_cast<Current_&>(*this);
  }

 protected:
  virtual Object MakeObject() const {
    Object o(type_);
    o.SetOption("fill", fill_);
    o.SetOption("stroke", stroke_);
    o.SetOption("stroke-width", stroke_width_);
    if (stroke_linecap_) {
      o.SetOption("stroke-linecap", stroke_linecap_.value());
    }
    if (stroke_linejoin_) {
      o.SetOption("stroke-linejoin", stroke_linejoin_.value());
    }
    return o;
  }

 private:
  std::string type_;

  Color fill_ = NoneColor;
  Color stroke_ = NoneColor;
  double stroke_width_ = 1;
  std::optional<std::string> stroke_linecap_;
  std::optional<std::string> stroke_linejoin_;
};

class Circle : public BaseObject<Circle> {
 public:
  Circle();
  Circle& SetCenter(Point);
  Circle& SetRadius(double);

 protected:
  Object MakeObject() const override;

 private:
  Point center_ = {0, 0};
  double radius_ = 1;
};

class Polyline : public BaseObject<Polyline> {
 public:
  Polyline();
  Polyline& AddPoint(Point);

 protected:
  Object MakeObject() const override;

 private:
  std::vector<Point> points_;
};

class Text : public BaseObject<Text> {
 public:
  Text();
  Text& SetPoint(Point);
  Text& SetOffset(Point);
  Text& SetFontSize(uint32_t);
  Text& SetFontFamily(std::string);
  Text& SetFontWeight(std::string);
  Text& SetData(std::string);

 protected:
  Object MakeObject() const override;

 private:
  Point point_ = {0, 0};
  Point offset_ = {0, 0};
  uint32_t font_size_ = 1;
  std::optional<std::string> font_family_;
  std::optional<std::string> font_weight_;
  std::string data_;
};

} // namespace Svg