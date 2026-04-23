#include <cmath>
#include <cassert>
#include <iostream>

using Real = double;

const Real EPSILON = 1e-9;
const Real SMALL_VALUE = 1e-12;
const Real PI = 3.141592653589793;


Real square(Real x) {
    return x * x;
}

Real clamp(Real value, Real minVal, Real maxVal) {
    if (value < minVal) {
        return minVal;
    }
    if (value > maxVal) {
        return maxVal;
    }
    return value;
}

bool isNearlyZero(Real x) {
    return std::abs(x) <= EPSILON;
}

bool isNearlyEqual(Real a, Real b) {
    return std::abs(a - b) <= EPSILON;
}


struct Vec3 {
    Real x;
    Real y;
    Real z;


    Vec3();
    Vec3(Real x_, Real y_, Real z_);
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(Real scalar) const;
    Vec3 operator/(Real scalar) const;

    Vec3& operator+=(const Vec3& other);
    Vec3& operator-=(const Vec3& other);

    Real length() const;
    Real lengthSq() const;

    Vec3 normalized() const;
    void normalize();

    bool isZero() const;
    bool isFinite() const;

    static Real dot(const Vec3& a, const Vec3& b);
    static Vec3 cross(const Vec3& a, const Vec3& b);
    static Real distance(const Vec3& a, const Vec3& b);
    static Real distanceSq(const Vec3& a, const Vec3& b);
};


// результат смещения
struct Displacement {
    Vec3 delta;
    Real distanceSq;
};


Displacement computeDisplacement(const Vec3& a, const Vec3& b) {
    Displacement result;
    result.delta = b - a;
    result.distanceSq = result.delta.lengthSq();
    return result;
}


// минимальное расстояние с учётом "замкнутого мира"
Real minimalImage(Real dx, Real boxSize);

// применить к вектору
Vec3 applyMinimalImage(const Vec3& delta, const Vec3& boxSize);

// вернуть координату в диапазон [0, boxSize)
Real wrapCoordinate(Real value, Real boxSize);

// вернуть позицию внутрь коробки
Vec3 wrapPosition(const Vec3& pos, const Vec3& boxSize);


struct Mat4 {
    Real m[16]; // можно column-major или row-major

    // --- создание ---
    static Mat4 identity();

    // --- преобразования ---
    static Mat4 translation(const Vec3& t);
    static Mat4 scale(const Vec3& s);

    static Mat4 rotationX(Real angle);
    static Mat4 rotationY(Real angle);
    static Mat4 rotationZ(Real angle);

    static Mat4 perspective(Real fov, Real aspect, Real near, Real far);
    static Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up);

    // --- операции ---
    Mat4 operator*(const Mat4& other) const;
};


// translation * rotation * scale
Mat4 makeModelMatrix(
    const Vec3& position,
    const Vec3& rotation,
    const Vec3& scale
);


struct AABB {
    Vec3 min;
    Vec3 max;

    Vec3 size() const;
    Vec3 center() const;
    bool contains(const Vec3& p) const;
};


void printVec3(const Vec3& v) {
    std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
}



int main() {
    // протестировать:
    // - Vec3
    // - distance
    // - normalization
    // - periodic boundary

    return 0;
}