#include <iostream>
#include <string>
#include <iomanip>
#include <memory>
#include <vector>
#include <exception>
#include <cmath>

using namespace std;

class Figure {
public:
    virtual string Name() const = 0;
    virtual double Perimeter() const = 0;
    virtual double Area() const = 0;
};

class Rect : public Figure {
public:
    Rect(int w, int h) : w_(w), h_(h) {}

    string Name() const override {
        return "RECT";
    }

    double Perimeter() const override {
        return (w_+h_)*2; 
    }

    double Area() const override {
        return w_*h_;
    }

private:
    const int w_, h_;
};

class Triangle : public Figure {
public:
    Triangle(int a, int b, int c) : a_(a), b_(b), c_(c) {}

    string Name() const override {
        return "TRIANGLE";
    }

    double Perimeter() const override {
        return a_ + b_ + c_;        
    }

    double Area() const override {
        double p = Perimeter() / 2;
        double d = p * (p - a_) * (p - b_) * (p - c_);
        return sqrt(d);
    }

private:
    const int a_, b_, c_;
};

class Circle : public Figure {
public:
    Circle(int r) : r_(r) {}

    string Name() const override {
        return "CIRCLE";
    }

    double Perimeter() const override {
        return 2 * PI * r_; 
    }

    double Area() const override {
        return PI * r_ * r_; 
    }

private:
    const int r_;
    const double PI = 3.14;
};

shared_ptr<Figure> CreateFigure(istream& s) {
    string fig;
    s >> fig;
    if (fig == "RECT") {
        int w, h;
        s >> w >> h;
        return make_shared<Rect>(w, h);
    } else if (fig == "TRIANGLE") {
        int a, b, c;
        s >> a >> b >> c;
        return make_shared<Triangle>(a, b, c);
    } else if (fig == "CIRCLE") {
        int r;
        s >> r;
        return make_shared<Circle>(r);
    } else {
        cout << "Unknown fig " << fig << ". skip" << endl;
        return nullptr;
    }
}

int main() {
  vector<shared_ptr<Figure>> figures;
  for (string line; getline(cin, line); ) {
    istringstream is(line);

    string command;
    is >> command;
    if (command == "ADD") {
      is >> ws;
      figures.push_back(CreateFigure(is));
    } else if (command == "PRINT") {
      for (const auto& current_figure : figures) {
        cout << fixed << setprecision(3)
             << current_figure->Name() << " "
             << current_figure->Perimeter() << " "
             << current_figure->Area() << endl;
      }
    }
  }
  return 0;
}