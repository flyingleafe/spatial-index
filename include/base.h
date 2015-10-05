#ifndef FLF_CW_BASE
#define FLF_CW_BASE

#include <cstddef>
#include <cmath>
#include <gmpxx.h>
#include <type_traits>
#include <limits>

#define between(a, b, c) ((a) <= (b) && (b) <= (c))

template <typename T>
struct point {
    T x, y;

    point<T>(T _x, T _y) : x(_x), y(_y) {}

    point<T> operator+(const point<T> &p) const {
        point<T> r;
        r.x = x + p.x;
        r.y = y + p.y;
        return r;
    };

    point<T> operator-(const point<T> &p) const {
        point<T> r;
        r.x = x - p.x;
        r.y = y - p.y;
        return r;
    };
};

template <typename T>
int sgn(T val) {
    return (val > T(0)) - (val < T(0));
}

template <typename T>
int mpq_turn(const point<T> &A, const point<T> &B, const point<T> &C) {
    mpq_class Ax(A.x), Ay(A.y), Bx(B.x), By(B.y), Cx(C.x), Cy(C.y);
    return sgn((Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax));
}

template <typename T>
int turn(const point<T> &A, const point<T> &B, const point<T> &C) {
    long double a1 = ((long double)(B.x - A.x))*(C.y - A.y);
    long double a2 = ((long double)(B.y - A.y))*(C.x - A.x);
    long double val = a1 - a2;
    long double eps = 8 * std::abs(a1 + a2) * std::numeric_limits<double>::epsilon();

    if (std::abs(val) < eps) {
        return mpq_turn(A, B, C);
    }
    return sgn(val);
}

template <typename T>
struct b_box {
    T x1, y1, x2, y2;

    b_box<T>(T _x1, T _y1, T _x2, T _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

    bool intersects_with(const b_box<T> &other) const {
        return (between(x1, other.x1, x2)       || between(x1, other.x2, x2) ||
                between(other.x1, x1, other.x2) || between(other.x1, x2, other.x2))
            && (between(y1, other.y1, y2)       || between(y1, other.y2, y2) ||
                between(other.y1, y1, other.y2) || between(other.y1, y2, other.y2));
    }
};

template <typename T>
struct segment {
    point<T> begin, end;

    segment<T>(point<T> &a, point<T> &b) : begin(a), end(b) {}

    b_box<T> bounding_box() const {
        return b_box<T>(std::min(begin.x, end.x), std::min(begin.y, end.y),
                        std::max(begin.x, end.x), std::max(begin.y, end.y));
    }

    bool intersects_with(const segment<T> &other) const {
        if (!bounding_box().intersects_with(other.bounding_box())) {
            return false;
        }

        int t1 = turn(begin, end, other.begin);
        int t2 = turn(begin, end, other.end);
        int t3 = turn(other.begin, other.end, begin);
        int t4 = turn(other.begin, other.end, end);

        return (t1 == 0 && t2 == 0 && t3 == 0 && t4 == 0)
            || (t1 != t2 && t3 != t4);
    }
};

#endif
