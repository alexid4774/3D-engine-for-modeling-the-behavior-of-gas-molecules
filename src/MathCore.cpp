#include "MathCore.hpp"

// Math

Real Math::square(Real x) {
    return x * x;
}

Real Math::clamp(Real value, Real minVal, Real maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

bool Math::isNearlyZero(Real x) {
    return std::abs(x) <= EPSILON;
}

bool Math::isNearlyEqual(Real a, Real b) {
    return std::abs(a - b) <= EPSILON;
}

// Vec3

Vec3::Vec3() : x(0), y(0), z(0) {}

Vec3::Vec3(Real x_, Real y_, Real z_)
    : x(x_), y(y_), z(z_) {}

Real Vec3::getX() const { return x; }
Real Vec3::getY() const { return y; }
Real Vec3::getZ() const { return z; }

Vec3 Vec3::operator+(const Vec3& other) const {
    return Vec3(x + other.x, y + other.y, z + other.z);
}

Vec3 Vec3::operator-(const Vec3& other) const {
    return Vec3(x - other.x, y - other.y, z - other.z);
}

Vec3 Vec3::operator*(Real scalar) const {
    return Vec3(x * scalar, y * scalar, z * scalar);
}

Vec3 Vec3::operator/(Real scalar) const {
    if (Math::isNearlyZero(scalar)) {
        throw std::runtime_error("ZeroDivision");
    }

    return Vec3(x / scalar, y / scalar, z / scalar);
}

Vec3& Vec3::operator+=(const Vec3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Real Vec3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

Real Vec3::lengthSq() const {
    return x * x + y * y + z * z;
}

bool Vec3::isZero() const {
    return Math::isNearlyZero(length());
}

Vec3 Vec3::normalized() const {
    if (isZero()) {
        return *this;
    }

    return *this / length();
}

void Vec3::normalize() {
    if (!isZero()) {
        *this = *this / length();
    }
}

Real Vec3::dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 Vec3::cross(const Vec3& a, const Vec3& b) {
    return Vec3(
        a.y * b.z - a.z * b.y,
        -a.x * b.z + a.z * b.x,
        a.x * b.y - a.y * b.x
    );
}

Real Vec3::distance(const Vec3& a, const Vec3& b) {
    return (a - b).length();
}

Real Vec3::distanceSq(const Vec3& a, const Vec3& b) {
    return (a - b).lengthSq();
}

Vec3 Vec3::zero() {
    return Vec3(0.0f, 0.0f, 0.0f);
}

void Vec3::print() const {
    std::cout << "(" << x << ", " << y << ", " << z << ")";
}

// Displacement

Displacement::Displacement()
    : delta(Vec3::zero()), distanceSq(0.0f) {}

Displacement::Displacement(const Vec3& delta_)
    : delta(delta_), distanceSq(delta_.lengthSq()) {}

const Vec3& Displacement::getDelta() const {
    return delta;
}

Real Displacement::getDistanceSquared() const {
    return distanceSq;
}

Real Displacement::getDistance() const {
    return std::sqrt(distanceSq);
}

Displacement Displacement::between(const Vec3& from, const Vec3& to) {
    return Displacement(to - from);
}

// PeriodicBox

PeriodicBox::PeriodicBox()
    : size(0.0f, 0.0f, 0.0f) {}

PeriodicBox::PeriodicBox(const Vec3& boxSize)
    : size(boxSize) {}

const Vec3& PeriodicBox::getSize() const {
    return size;
}

void PeriodicBox::setSize(const Vec3& newSize) {
    size = newSize;
}

Real PeriodicBox::minimalImage1D(Real dx, Real boxLength) const {
    if (boxLength <= 0.0f) {
        return dx;
    }

    Real half = 0.5f * boxLength;

    if (dx > half) {
        dx -= boxLength;
    } else if (dx < -half) {
        dx += boxLength;
    }

    return dx;
}

Vec3 PeriodicBox::applyMinimalImage(const Vec3& delta) const {
    return Vec3(
        minimalImage1D(delta.getX(), size.getX()),
        minimalImage1D(delta.getY(), size.getY()),
        minimalImage1D(delta.getZ(), size.getZ())
    );
}

Real PeriodicBox::wrapCoordinate(Real value, Real boxLength) const {
    if (boxLength <= 0.0f) {
        return value;
    }

    value = std::fmod(value, boxLength);

    if (value < 0.0f) {
        value += boxLength;
    }

    return value;
}

Vec3 PeriodicBox::wrapPosition(const Vec3& position) const {
    return Vec3(
        wrapCoordinate(position.getX(), size.getX()),
        wrapCoordinate(position.getY(), size.getY()),
        wrapCoordinate(position.getZ(), size.getZ())
    );
}

Displacement PeriodicBox::periodicDisplacement(const Vec3& from, const Vec3& to) const {
    Vec3 rawDelta = to - from;
    Vec3 correctedDelta = applyMinimalImage(rawDelta);
    return Displacement(correctedDelta);
}

// Mat4

Mat4::Mat4() {
    for (int i = 0; i < 16; i++) {
        m[i] = 0.0f;
    }
}

Real& Mat4::at(int row, int col) {
    return m[col * 4 + row];
}

const Real& Mat4::at(int row, int col) const {
    return m[col * 4 + row];
}

const Real* Mat4::data() const {
    return m;
}

Real* Mat4::data() {
    return m;
}

Mat4 Mat4::identity() {
    Mat4 result;

    result.at(0, 0) = 1.0f;
    result.at(1, 1) = 1.0f;
    result.at(2, 2) = 1.0f;
    result.at(3, 3) = 1.0f;

    return result;
}

Mat4 Mat4::translation(const Vec3& t) {
    Mat4 result = identity();

    result.at(0, 3) = t.x;
    result.at(1, 3) = t.y;
    result.at(2, 3) = t.z;

    return result;
}

Mat4 Mat4::scale(const Vec3& s) {
    Mat4 result = identity();

    result.at(0, 0) = s.x;
    result.at(1, 1) = s.y;
    result.at(2, 2) = s.z;

    return result;
}

Mat4 Mat4::rotationX(Real angle) {
    Mat4 result = identity();

    Real c = std::cos(angle);
    Real s = std::sin(angle);

    result.at(1, 1) = c;
    result.at(1, 2) = -s;
    result.at(2, 1) = s;
    result.at(2, 2) = c;

    return result;
}

Mat4 Mat4::rotationY(Real angle) {
    Mat4 result = identity();

    Real c = std::cos(angle);
    Real s = std::sin(angle);

    result.at(0, 0) = c;
    result.at(0, 2) = s;
    result.at(2, 0) = -s;
    result.at(2, 2) = c;

    return result;
}

Mat4 Mat4::rotationZ(Real angle) {
    Mat4 result = identity();

    Real c = std::cos(angle);
    Real s = std::sin(angle);

    result.at(0, 0) = c;
    result.at(0, 1) = -s;
    result.at(1, 0) = s;
    result.at(1, 1) = c;

    return result;
}

Mat4 Mat4::perspective(Real fov, Real aspect, Real nearPlane, Real farPlane) {
    if (Math::isNearlyZero(aspect)) {
        throw std::runtime_error("ZeroDivision (aspect)!");
    }

    if (Math::isNearlyZero(std::tan(fov * 0.5f))) {
        throw std::runtime_error("ZeroDivision (fov)!");
    }

    if (Math::isNearlyEqual(nearPlane, farPlane)) {
        throw std::runtime_error("ZeroDivision (near = far)!");
    }

    Real tanHalfFov = std::tan(fov * 0.5f);

    Mat4 result;

    result.at(0, 0) = 1.0f / (aspect * tanHalfFov);
    result.at(1, 1) = 1.0f / tanHalfFov;
    result.at(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
    result.at(2, 3) = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
    result.at(3, 2) = -1.0f;

    return result;
}

Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 forward = (target - eye).normalized();
    Vec3 right = Vec3::cross(forward, up).normalized();
    Vec3 trueUp = Vec3::cross(right, forward);

    Mat4 result = identity();

    result.at(0, 0) = right.x;
    result.at(1, 0) = right.y;
    result.at(2, 0) = right.z;

    result.at(0, 1) = trueUp.x;
    result.at(1, 1) = trueUp.y;
    result.at(2, 1) = trueUp.z;

    result.at(0, 2) = -forward.x;
    result.at(1, 2) = -forward.y;
    result.at(2, 2) = -forward.z;

    result.at(0, 3) = -Vec3::dot(right, eye);
    result.at(1, 3) = -Vec3::dot(trueUp, eye);
    result.at(2, 3) = Vec3::dot(forward, eye);

    return result;
}

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 result;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            Real sum = 0.0f;

            for (int k = 0; k < 4; k++) {
                sum += at(row, k) * other.at(k, col);
            }

            result.at(row, col) = sum;
        }
    }

    return result;
}

Mat4 Mat4::makeModelMatrix(
    const Vec3& position,
    const Vec3& rotation,
    const Vec3& scaleVec
) {
    return Mat4::translation(position)
         * Mat4::rotationZ(rotation.z)
         * Mat4::rotationY(rotation.y)
         * Mat4::rotationX(rotation.x)
         * Mat4::scale(scaleVec);
}

void Mat4::print() const {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            std::cout << at(row, col) << " ";
        }

        std::cout << "\n";
    }
}

// AABB

AABB::AABB()
    : minPoint(Vec3::zero()), maxPoint(Vec3::zero()) {}

AABB::AABB(const Vec3& minP, const Vec3& maxP)
    : minPoint(minP), maxPoint(maxP) {}

Vec3 AABB::getMin() const {
    return minPoint;
}

Vec3 AABB::getMax() const {
    return maxPoint;
}

Vec3 AABB::size() const {
    return maxPoint - minPoint;
}

Vec3 AABB::center() const {
    return (minPoint + maxPoint) * 0.5f;
}

bool AABB::contains(const Vec3& p) const {
    return p.getX() >= minPoint.getX() && p.getX() <= maxPoint.getX() &&
           p.getY() >= minPoint.getY() && p.getY() <= maxPoint.getY() &&
           p.getZ() >= minPoint.getZ() && p.getZ() <= maxPoint.getZ();
}