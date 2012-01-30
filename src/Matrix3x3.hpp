/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_MATRIX3X3_HPP_
#define WC_MATRIX3X3_HPP_

#include <ostream>

#include <Wt/WTransform>
#include <Wt/WPointF>

#include "global.hpp"

namespace Wt {

namespace Wc {

/** Row for Matrix3x3.

\see Matrix3x3
\ingroup util
*/
struct Row3 {
    /** Values of row */
    double m[3];

    /** Default constructor.
    \note Values are initialized with 0.
    */
    Row3();

    /** Constructor */
    Row3(double m1, double m2, double m3);

    /** Multiply to the matrix */
    Row3 operator*(const Matrix3x3& matrix) const;

    /** Multiply to the column of matrix */
    double mul(const Matrix3x3& matrix, int column) const;
};

/** Structure of 3 points.

\see Matrix3x3
\ingroup util
*/
struct ThreeWPoints {
    /** Constructor */
    ThreeWPoints(const WPointF& aa, const WPointF& bb, const WPointF& cc);

    /** Point A */
    WPointF a;

    /** Point B */
    WPointF b;

    /** Point C */
    WPointF c;
};

/** Util structure for custom 3x3 matrix of \c double.
This structure was created since WTransform class does not allow
specify m13 and m23.

\ingroup util
*/
struct Matrix3x3 {
    /** Values of the matrix.
    To access value at \c row=row and \c column=col (first is 0),
    use \c m[row][col].
    \f[ \left(\begin{array}{c c c}
    m[0][0] & m[0][1] & m[0][2] \\
    m[1][0] & m[1][1] & m[1][2] \\
    m[2][0] & m[2][1] & m[2][2] \\
    \end{array}\right) \f]
    */
    double m[3][3];

    /** Default constructor.
    \note Values of matrix are initialized with 0
    */
    Matrix3x3();

    /** Construct a custom matrix by specifying the parameters. */
    Matrix3x3(double m11, double m12, double m13,
              double m21, double m22, double m23,
              double m31, double m32, double m33);

    /** Return the transformation matrix.
    The matrix transforms point coordinates (from 2 to).

    Let us prove the way of calculating transformation matrix.
    WTransform will apply this to (a, b, c):
    \f[
    \left(\begin{array}{c c}
    a_x & a_y \\
    b_x & b_y \\
    c_x & c_y
    \end{array}\right)

    \left(\begin{array}{c c}
    m_{11} & m_{12} \\
    m_{21} & m_{22}
    \end{array}\right) +

    \left(\begin{array}{c c}
    d_x & d_y \\
    d_x & d_y \\
    d_x & d_y \end{array}\right) =

    \left(\begin{array}{c c}
    A_x & A_y \\
    B_x & B_y \\
    C_x & C_y \end{array}\right).
    \f]

    ... which is equal to ...

    \f[
    \left(\begin{array}{c c c}
    a_x & a_y & 1 \\
    b_x & b_y & 1 \\
    c_x & c_y & 1 \end{array}\right)

    \left(\begin{array}{c c c}
    m_{11} & m_{12} & 0 \\
    m_{21} & m_{22} & 0 \\
    d_x & d_y & 1 \end{array}\right) =

    \left(\begin{array}{c c c}
    A_x & A_y & 1 \\
    B_x & B_y & 1 \\
    C_x & C_y & 1 \end{array}\right).
    \f]

    ... which is equal to ...

    \f[
    \left(\begin{array}{c c c}
    m_{11} & m_{12} & 0 \\
    m_{21} & m_{22} & 0 \\
    d_x & d_y & 1 \end{array}\right) =

    \left(\begin{array}{c c c}
    a_x & a_y & 1 \\
    b_x & b_y & 1 \\
    c_x & c_y & 1 \end{array}\right)^{-1}

    \left(\begin{array}{c c c}
    A_x & A_y & 1 \\
    B_x & B_y & 1 \\
    C_x & C_y & 1 \end{array}\right).
    \f]
    */
    Matrix3x3(const ThreeWPoints& from, const ThreeWPoints& to);

    /** Calculate the determinant */
    double det() const;

    /** Calculate the adjunct */
    double adjunct(int row, int col) const;

    /** Calculate the inverted matrix */
    Matrix3x3 inv() const;

    /** Multiply to other matrix */
    Matrix3x3 operator*(const Matrix3x3& other) const;

    /** Convert to WTransform matrix */
    operator WTransform() const;

    /** print WTransform */
    static std::ostream& print(std::ostream& o, const WTransform& matrix);
};

/** streaming operator */
std::ostream& operator<<(std::ostream& o, const Matrix3x3& matrix);

}

}

#endif

