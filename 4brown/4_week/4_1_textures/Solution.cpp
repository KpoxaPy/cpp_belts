#include "Common.h"

#include <functional>
#include <stdexcept>

using namespace std;

template <typename DerivedShape>
class ClonableShape : public virtual IShape {
public:
  std::unique_ptr<IShape> Clone() const override {
    return make_unique<DerivedShape>(static_cast<const DerivedShape &>(*this));
  }
};

class DrawableShape : public virtual IShape {
public:
  void Draw(Image& i) const override {
    Size i_size = {0, static_cast<int>(i.size())};
    if (i_size.height > 0) {
      i_size.width = i[0].size();
    }

    Point p = position_;
    while (CheckPosition(p)) {
      if (0 <= p.x && p.x < i_size.width && 0 <= p.y && p.y < i_size.height) {
        if (CheckByMask(p)) {
          i[p.y][p.x] = GetSymbol(p);
        }
      }
      NextPosition(p);
    }
  }

  void SetPosition(Point position) override {
    position_ = move(position);
  }

  Point GetPosition() const override {
    return position_;
  }

  virtual char GetSymbol(Point) const {
    return '.';
  }

  virtual bool CheckPosition(Point) const {
    return false;
  }

  virtual void NextPosition(Point&) const {}

  virtual bool CheckByMask(Point) const {
    return true;
  }

private:
  Point position_;
};

class BoundedShape : public virtual DrawableShape {
public:
  void SetSize(Size size) override {
    size_ = move(size);
  }

  Size GetSize() const override {
    return size_;
  }

  virtual bool CheckPosition(Point p) const {
    Point s = GetPosition();
    Point e{s.x + size_.width, s.y + size_.height};
    return s.x <= p.x && p.x < e.x
      && s.y <= p.y && p.y < e.y;
  }

  void NextPosition(Point& p) const override {
    Point s = GetPosition();
    Point e{s.x + size_.width, s.y + size_.height};
    if (p.x < e.x - 1) {
      ++p.x;
    } else {
      p.x = s.x;
      ++p.y;
    }
  }

private:
  Size size_;
};

class TexturedShape : public virtual DrawableShape {
public:
  void SetTexture(std::shared_ptr<ITexture> tex) override {
    tex_ = move(tex);
  }

  ITexture* GetTexture() const override {
    return tex_.get();
  }

  char GetSymbol(Point p) const override {
    if (tex_) {
      Size tex_size = tex_->GetSize();
      Point start = GetPosition();
      p.x -= start.x;
      p.y -= start.y;
      if (0 <= p.x && p.x < tex_size.width && 0 <= p.y && p.y < tex_size.height) {
        return tex_->GetImage()[p.y][p.x];
      }
    }
    return DrawableShape::GetSymbol(p);
  }

private:
  shared_ptr<ITexture> tex_;
};

class Rectangle : public TexturedShape, public BoundedShape, public ClonableShape<Rectangle> {};

class Ellipse : public TexturedShape, public BoundedShape, public ClonableShape<Ellipse> {
public:
  bool CheckByMask(Point p) const override {
    Point d = GetPosition();
    p.x -= d.x;
    p.y -= d.y;
    return IsPointInEllipse(p, GetSize());
  }
};

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
  switch (shape_type) {
    case ShapeType::Ellipse:
      return make_unique<Ellipse>();
    case ShapeType::Rectangle:
      return make_unique<Rectangle>();
    default:
      throw runtime_error("");
  }
}