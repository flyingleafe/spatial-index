#ifndef FLF_CW_BASE
#define FLF_CW_BASE

#include <cstddef>
#include <cmath>
#include <gmpxx.h>
#include <type_traits>
#include <limits>

template <typename T>
inline bool between(T a, T b, T c) {
    return a <= b && b <= c;
}

template <typename T>
class Point {
public:
    T x, y;

    Point<T>(T _x, T _y) : x(_x), y(_y) {}

    const Point<T> operator+(const Point<T> &p) const {
        Point<T> r;
        r.x = x + p.x;
        r.y = y + p.y;
        return r;
    };

    const Point<T> operator-(const Point<T> &p) const {
        Point<T> r;
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
int mpqTurn(const Point<T> &a, const Point<T> &b, const Point<T> &c) {
    mpq_class ax(a.x), ay(a.y), bx(b.x), by(b.y), cx(c.x), cy(c.y);
    return sgn((bx - ax) * (cy - ay) - (by - ay) * (cx - ax));
}

template <typename T>
int turn(const Point<T> &a, const Point<T> &b, const Point<T> &c) {
    long double a1 = ((long double)(b.x - a.x))*(c.y - a.y);
    long double a2 = ((long double)(b.y - a.y))*(c.x - a.x);
    long double val = a1 - a2;
    long double eps = 8 * std::abs(a1 + a2) * std::numeric_limits<double>::epsilon();

    if (std::abs(val) < eps) {
        return mpqTurn(a, b, c);
    }
    return sgn(val);
}

int turn(const Point<int> &a, const Point<int> &b, const Point<int> &c) {
    return sgn((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x));
}

template <typename T>
class BoundingBox {
public:
    T x1, y1, x2, y2;

    BoundingBox<T>(T _x1, T _y1, T _x2, T _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

    bool intersectsWith(const BoundingBox<T> &other) const {
        return (between(x1, other.x1, x2)       || between(x1, other.x2, x2) ||
                between(other.x1, x1, other.x2) || between(other.x1, x2, other.x2))
            && (between(y1, other.y1, y2)       || between(y1, other.y2, y2) ||
                between(other.y1, y1, other.y2) || between(other.y1, y2, other.y2));
    }
};

template <typename T>
class Segment {
public:
    Point<T> begin, end;

    Segment<T>(Point<T> &a, Point<T> &b) : begin(a), end(b) {}

    BoundingBox<T> boundingBox() const {
        return BoundingBox<T>(std::min(begin.x, end.x), std::min(begin.y, end.y),
                        std::max(begin.x, end.x), std::max(begin.y, end.y));
    }

    bool intersectsWith(const Segment<T> &other) const {
        if (!boundingBox().intersectsWith(other.boundingBox())) {
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
