#include "svg.h"

using namespace std;

namespace Svg {

const Color NoneColor = {};

ostream& operator<<(ostream& s, const Rgb& rgb) {
  if (!rgb.alpha) {
    s << "rgb";
  } else {
    s << "rgba";
  }
  s << "(" << static_cast<int>(rgb.red) << "," << static_cast<int>(rgb.green) << "," << static_cast<int>(rgb.blue);
  if (rgb.alpha) {
    s << "," << rgb.alpha.value();
  }
  s << ")";
  return s;
}

void RenderColor(ostream& s, monostate) {
  s << "none";
}

void RenderColor(ostream& s, const string& color) {
  s << color;
}

void RenderColor(ostream& s, const Rgb& rgb) {
  s << rgb;
}

ostream& operator<<(ostream& s, const Color& c) {
  visit([&s](const auto& v) mutable {
    RenderColor(s, v);
  }, c);
  return s;
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
    ss << (!not_first ? (not_first = true, "") : " ") << p.x << "," << p.y;
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

Text& Text::SetFontWeight(std::string font_weight) {
  font_weight_ = move(font_weight);
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
  if (font_weight_) {
    o.SetOption("font-weight", font_weight_.value());
  }
  o.SetData(data_);
  return o;
}

}  // namespace Svg