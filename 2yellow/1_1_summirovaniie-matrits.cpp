#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>

using namespace std;

class Matrix {
    friend istream& operator>>(istream& s, Matrix& m);
    friend ostream& operator<<(ostream& s, const Matrix& m);
    friend bool operator==(const Matrix& lhs, const Matrix& rhs);
    friend Matrix operator+(const Matrix& lhs, const Matrix& rhs);

public:
    Matrix() {
        Reset(0, 0);
    }

    Matrix(int rows_num, int cols_num) {
        Reset(rows_num, cols_num);
    }

    void Reset(int rows_num, int cols_num) {
        if (rows_num < 0 || cols_num < 0) {
            throw out_of_range("");
        }
        rows_num_ = rows_num;
        cols_num_ = cols_num;

        m_ = vector<vector<int>>(rows_num, vector<int>(cols_num, 0));
    }

    int& At(int row, int col) {
        if (row < 0 || rows_num_ <= row
            || col < 0 || cols_num_ <= col) {
            throw out_of_range("");
        }
        return m_[row][col];
    }

    int At(int row, int col) const {
        if (row < 0 || rows_num_ <= row
            || col < 0 || cols_num_ <= col) {
            throw out_of_range("");
        }
        return m_[row][col];
    }

    int GetNumRows() const {
        return rows_num_;
    }

    int GetNumColumns() const {
        return cols_num_;
    }

    bool IsEmpty() const {
        return rows_num_ == 0 || cols_num_ == 0;
    }

private:
    int rows_num_;
    int cols_num_;
    vector<vector<int>> m_;
};

istream& operator>>(istream& s, Matrix& m) {
    int rows, cols;
    s >> rows >> cols;
    m.Reset(rows, cols);
    for (int i = 0; i < m.rows_num_; ++i) {
        for (int j = 0; j < m.cols_num_; ++j) {
            s >> m.m_[i][j];
        }
    }
    return s;
}

ostream& operator<<(ostream& s, const Matrix& m) {
    s << m.rows_num_ << ' ' << m.cols_num_ << endl;
    for (int i = 0; i < m.rows_num_; ++i) {
        for (int j = 0; j < m.cols_num_; ++j) {
            s << (j == 0 ? "" : " ") << m.m_[i][j];
        }
        s << endl;
    }
    return s;
}

bool operator==(const Matrix& lhs, const Matrix& rhs) {
    return lhs.IsEmpty() && rhs.IsEmpty() || lhs.m_ == rhs.m_;
}

Matrix operator+(const Matrix& lhs, const Matrix& rhs) {
    if (lhs.IsEmpty() && rhs.IsEmpty()) {
        return {};
    }

    if (lhs.rows_num_ != rhs.rows_num_
        || lhs.cols_num_ != rhs.cols_num_) {
        throw invalid_argument("cannot add m(" + to_string(lhs.rows_num_) + ", " + to_string(lhs.cols_num_)
            + ") and m(" + to_string(rhs.rows_num_) + ", " + to_string(rhs.cols_num_) + ")");
    }

    Matrix m(lhs.rows_num_, rhs.cols_num_);
    for (int i = 0; i < lhs.rows_num_; ++i) {
        for (int j = 0; j < lhs.cols_num_; ++j) {
            m.m_[i][j] = lhs.m_[i][j] + rhs.m_[i][j];
        }
    }
    return m;
}

int main() {
  Matrix one;
  Matrix two;

  cin >> one >> two;
  cout << one + two << endl;
  return 0;
}
