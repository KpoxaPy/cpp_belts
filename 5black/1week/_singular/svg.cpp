#include <deque>
#include <map>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
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
  int red = 0;
  int green = 0;
  int blue = 0;

  Rgb() = default;
  Rgb(int r, int g, int b)
      : red(r), green(g), blue(b) {}
};

std::ostream& operator<<(std::ostream&, const Rgb&);

class Color : std::variant<std::string, Rgb> {
  friend std::ostream& operator<<(std::ostream&, const Color&);

 public:
  using variant::variant;

  Color();
};

std::ostream& operator<<(std::ostream&, const Color&);

const Color NoneColor;

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
  Text& SetData(std::string);

 protected:
  Object MakeObject() const override;

 private:
  Point point_ = {0, 0};
  Point offset_ = {0, 0};
  uint32_t font_size_ = 1;
  std::optional<std::string> font_family_;
  std::string data_;
};

}  // namespace Svg

// IMPLEMENTATION

namespace Svg {

using namespace std;

ostream& operator<<(ostream& s, const Rgb& rgb) {
  return s << "rgb(" << rgb.red << "," << rgb.green << "," << rgb.blue << ")";
}

Color::Color()
    : variant("none") {}

ostream& operator<<(ostream& s, const Color& c) {
  if (holds_alternative<string>(c)) {
    return s << get<string>(c);
  } else if (holds_alternative<Rgb>(c)) {
    return s << get<Rgb>(c);
  }
  throw runtime_error("bad Color variant in operator<<");
}

Object::Object(string type)
    : type_(move(type)) {}

void Object::Render(std::ostream& s) const {
  s << "<" << type_;
  for (const auto& [type, value] : options_) {
    s << " " << type << "=\"" << value << "\"";
  }
  if (data_) {
    s << ">" << data_.value() << "</" << type_ << ">";
  } else {
    s << "/>";
  }
}

void Object::SetData(std::string data) {
  data_ = move(data);
}

void Object::SetOptionImpl(std::string type, std::string value) {
  options_[move(type)] = move(value);
}

std::ostream& operator<<(std::ostream& s, const Object& o) {
  o.Render(s);
  return s;
}

void Document::Add(Object o) {
  objects_.push_back(move(o));
}

void Document::Render(std::ostream& s) const {
  s << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
  s << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";
  for (const auto& o : objects_) {
    s << o;
  }
  s << "</svg>";
}

Circle::Circle()
    : BaseObject("circle") {}

Circle& Circle::SetCenter(Point p) {
  center_ = p;
  return *this;
}

Circle& Circle::SetRadius(double v) {
  radius_ = v;
  return *this;
}

Object Circle::MakeObject() const {
  auto o = BaseObject::MakeObject();
  o.SetOption("cx", center_.x);
  o.SetOption("cy", center_.y);
  o.SetOption("r", radius_);
  return o;
}

Polyline::Polyline()
    : BaseObject("polyline") {}

Polyline& Polyline::AddPoint(Point p) {
  points_.push_back(p);
  return *this;
}

Object Polyline::MakeObject() const {
  auto o = BaseObject::MakeObject();
  ostringstream ss;
  bool not_first = false;
  for (const auto& p : points_) {
    ss << (not_first ? (not_first = true, "") : " ") << p.x << "," << p.y;
  }
  o.SetOption("points", ss.str());
  return o;
}

Text::Text()
    : BaseObject("text") {}

Text& Text::SetPoint(Point p) {
  point_ = p;
  return *this;
}

Text& Text::SetOffset(Point p) {
  offset_ = p;
  return *this;
}

Text& Text::SetFontSize(uint32_t size) {
  font_size_ = size;
  return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
  font_family_ = move(font_family);
  return *this;
}

Text& Text::SetData(std::string data) {
  data_ = move(data);
  return *this;
}

Object Text::MakeObject() const {
  auto o = BaseObject::MakeObject();
  o.SetOption("x", point_.x);
  o.SetOption("y", point_.y);
  o.SetOption("dx", offset_.x);
  o.SetOption("dy", offset_.y);
  o.SetOption("font-size", font_size_);
  if (font_family_) {
    o.SetOption("font-family", font_family_.value());
  }
  o.SetData(data_);
  return o;
}

}  // namespace Svg