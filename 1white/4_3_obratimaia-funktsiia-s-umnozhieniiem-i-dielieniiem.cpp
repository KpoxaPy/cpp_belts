#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class FunctionPart {
public:
    FunctionPart(char op, double value) {
        op_ = op;
        value_ = value;
    }

    double Apply(double source_value) const {
        if (op_ == '+') {
            return source_value + value_;
        } else if (op_ == '-') {
            return source_value - value_;
        } else if (op_ == '*') {
            return source_value * value_;
        } else if (op_ == '/') {
            return source_value / value_;
        }
        return source_value;
    }

    void Invert() {
        if (op_ == '+') {
            op_ = '-';
        } else if (op_ == '-') {
            op_ = '+';
        } else if (op_ == '*') {
            op_ = '/';
        } else if (op_ == '/') {
            op_ = '*';
        }
    }

private:
    char op_;
    double value_;
};

class Function {
public:
    void AddPart(char op, double value) {
        parts_.push_back({op, value});
    }

    double Apply(double source_value) const {
        for (const auto& part : parts_) {
            source_value = part.Apply(source_value);
        }
        return source_value;
    }

    void Invert() {
        for (auto& part : parts_) {
            part.Invert();
        }
        reverse(begin(parts_), end(parts_));
    }

private:
    vector<FunctionPart> parts_;
};

// struct Image {
//   double quality;
//   double freshness;
//   double rating;
// };

// struct Params {
//   double a;
//   double b;
//   double c;
// };

// Function MakeWeightFunction(const Params& params,
//                             const Image& image) {
//   Function function;
//   function.AddPart('*', params.a);
//   function.AddPart('-', image.freshness * params.b);
//   function.AddPart('+', image.rating * params.c);
//   return function;
// }

// double ComputeImageWeight(const Params& params, const Image& image) {
//   Function function = MakeWeightFunction(params, image);
//   return function.Apply(image.quality);
// }

// double ComputeQualityByWeight(const Params& params,
//                               const Image& image,
//                               double weight) {
//   Function function = MakeWeightFunction(params, image);
//   function.Invert();
//   return function.Apply(weight);
// }

// int main() {
//   Image image = {10, 2, 6};
//   Params params = {4, 2, 6};
//   cout << ComputeImageWeight(params, image) << endl;
//   cout << ComputeQualityByWeight(params, image, 52) << endl;
//   return 0;
// }
