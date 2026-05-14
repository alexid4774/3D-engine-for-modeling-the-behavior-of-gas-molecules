#pragma once

#include <stdexcept>
#include <iostream>
#include <cmath>


using Real = float;


class Math {
public:
    static constexpr Real EPSILON = 1e-9f;
    static constexpr Real SMALL_VALUE = 1e-12f;
    static constexpr Real PI = 3.141592653589793f;

    static Real square(Real x);
    static Real clamp(Real value, Real minVal, Real maxVal);
    static bool isNearlyZero(Real x);
    static bool isNearlyEqual(Real a, Real b);
};


class Vec3 {
public:
    Real x;
    Real y;
    Real z;

    Vec3();
    Vec3(Real x_, Real y_, Real z_);

    Real getX() const;
    Real getY() const;
    Real getZ() const;

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(Real scalar) const;
    Vec3 operator/(Real scalar) const;

    Vec3& operator+=(const Vec3& other);
    Vec3& operator-=(const Vec3& other);

    Real length() const;
    Real lengthSq() const;

    bool isZero() const;

    Vec3 normalized() const;
    void normalize();

    static Real dot(const Vec3& a, const Vec3& b);
    static Vec3 cross(const Vec3& a, const Vec3& b);

    static Real distance(const Vec3& a, const Vec3& b);
    static Real distanceSq(const Vec3& a, const Vec3& b);

    static Vec3 zero();

    void print() const;
};


class Displacement {
private:
    Vec3 delta;
    Real distanceSq;

public:
    Displacement();
    Displacement(const Vec3& delta_);

    const Vec3& getDelta() const;
    Real getDistanceSquared() const;
    Real getDistance() const;

    static Displacement between(const Vec3& from, const Vec3& to);
};


class PeriodicBox {
private:
    Vec3 size;

public:
    PeriodicBox();
    PeriodicBox(const Vec3& boxSize);

    const Vec3& getSize() const;
    void setSize(const Vec3& newSize);

    Real minimalImage1D(Real dx, Real boxLength) const;
    Vec3 applyMinimalImage(const Vec3& delta) const;

    Real wrapCoordinate(Real value, Real boxLength) const;
    Vec3 wrapPosition(const Vec3& position) const;

    Displacement periodicDisplacement(const Vec3& from, const Vec3& to) const;
};


class Mat4 {
private:
    Real m[16];

public:
    Mat4();

    Real& at(int row, int col);
    const Real& at(int row, int col) const;

    const Real* data() const;
    Real* data();

    static Mat4 identity();
    static Mat4 translation(const Vec3& t);
    static Mat4 scale(const Vec3& s);

    static Mat4 rotationX(Real angle);
    static Mat4 rotationY(Real angle);
    static Mat4 rotationZ(Real angle);

    static Mat4 perspective(Real fov, Real aspect, Real nearPlane, Real farPlane);
    static Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up);

    Mat4 operator*(const Mat4& other) const;

    static Mat4 makeModelMatrix(
        const Vec3& position,
        const Vec3& rotation,
        const Vec3& scaleVec
    );

    void print() const;
};


class AABB {
private:
    Vec3 minPoint;
    Vec3 maxPoint;

public:
    AABB();
    AABB(const Vec3& minP, const Vec3& maxP);

    Vec3 getMin() const;
    Vec3 getMax() const;

    Vec3 size() const;
    Vec3 center() const;

    bool contains(const Vec3& p) const;
};