/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "Matrix3x3.hpp"

namespace Wt {

namespace Wc {

Row3::Row3() {
    m[0] = 0;
    m[1] = 0;
    m[2] = 0;
}

Row3::Row3(double m1, double m2, double m3) {
    m[0] = m1;
    m[1] = m2;
    m[2] = m3;
}

Row3 Row3::operator*(const Matrix3x3& matrix) const {
    return Row3(mul(matrix, 0), mul(matrix, 1), mul(matrix, 2));
}

double Row3::mul(const Matrix3x3& matrix, int column) const {
    double result = 0;
    for (int i = 0; i < 3; i++) {
        result += m[i] * matrix.m[i][column];
    }
    return result;
}

ThreeWPoints::ThreeWPoints(const WPointF& aa, const WPointF& bb,
                           const WPointF& cc):
    a(aa), b(bb), c(cc)
{ }

Matrix3x3::Matrix3x3() {
    m[0][0] = 0;
    m[0][1] = 0;
    m[0][2] = 0;
    m[1][0] = 0;
    m[1][1] = 0;
    m[1][2] = 0;
    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 0;
}

Matrix3x3::Matrix3x3(double m11, double m12, double m13,
                     double m21, double m22, double m23,
                     double m31, double m32, double m33) {
    m[0][0] = m11;
    m[0][1] = m12;
    m[0][2] = m13;
    m[1][0] = m21;
    m[1][1] = m22;
    m[1][2] = m23;
    m[2][0] = m31;
    m[2][1] = m32;
    m[2][2] = m33;
}

Matrix3x3::Matrix3x3(const ThreeWPoints& from, const ThreeWPoints& to) {
    const WPointF& a = from.a;
    const WPointF& b = from.b;
    const WPointF& c = from.c;
    const WPointF& A = to.a;
    const WPointF& B = to.b;
    const WPointF& C = to.c;
    Matrix3x3 m(a.x(), a.y(), 1,
                b.x(), b.y(), 1,
                c.x(), c.y(), 1);
    Matrix3x3 M(A.x(), A.y(), 1,
                B.x(), B.y(), 1,
                C.x(), C.y(), 1);
    *this = m.inv() * M;
}

double Matrix3x3::det() const {
    return m[0][0] * (m[2][2] * m[1][1] - m[2][1] * m[1][2])
           - m[1][0] * (m[2][2] * m[0][1] - m[2][1] * m[0][2])
           + m[2][0] * (m[1][2] * m[0][1] - m[1][1] * m[0][2]);
}

double Matrix3x3::adjunct(int row, int col) const {
    int sign = ((row + col) % 2) ? -1 : 1;
    int c1 = col == 0 ? 1 : 0;
    int r1 = row == 0 ? 1 : 0;
    int c2 = col == 2 ? 1 : 2;
    int r2 = row == 2 ? 1 : 2;
    return sign * (m[r1][c1] * m[r2][c2] - m[r1][c2] * m[r2][c1]);
}

Matrix3x3 Matrix3x3::inv() const {
    Matrix3x3 result;
    double d = det();
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            result.m[row][col] = adjunct(col, row) / d; // transpose
        }
    }
    return result;
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const {
    Matrix3x3 result;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            result.m[row][col] = 0;
            for (int i = 0; i < 3; i++) {
                result.m[row][col] += m[row][i] * other.m[i][col];
            }
        }
    }
    return result;
}

Matrix3x3::operator WTransform() const {
    return WTransform(m[0][0], m[0][1],
                      m[1][0], m[1][1],
                      m[2][0], m[2][1]);
}

std::ostream& operator<<(std::ostream& o, const Matrix3x3& v) {
    o << '(';
    o << v.m[0][0] << ',' << v.m[0][1] << ',' << v.m[0][2];
    o << '|';
    o << v.m[1][0] << ',' << v.m[1][1] << ',' << v.m[1][2];
    o << '|';
    o << v.m[2][0] << ',' << v.m[2][1] << ',' << v.m[2][2];
    o << ')';
    return o;
}

std::ostream& Matrix3x3::print(std::ostream& o, const WTransform& v) {
    o << '(';
    o << v.m11() << ',' << v.m12() << ',' << 0;
    o << '|';
    o << v.m21() << ',' << v.m22() << ',' << 0;
    o << '|';
    o << v.dx() << ',' << v.dy() << ',' << 1;
    o << ')';
    return o;
}

}

}

