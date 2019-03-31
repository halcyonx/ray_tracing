#pragma once
#include <cmath>

template <typename T>
class Vec3
{
public:
    Vec3() {}
    Vec3(T e)
        : v{e, e, e}
    {}
    Vec3(T e0, T e1, T e2)
        : v{e0, e1, e2}
    {}

    inline T x() const { return v[0]; }
    inline T y() const { return v[1]; }
    inline T z() const { return v[2]; }

    inline T r() const { return v[0]; }
    inline T g() const { return v[1]; }
    inline T b() const { return v[2]; }

    inline Vec3& operator+() { return *this; }
    inline Vec3 operator-() const { return Vec3{ -v[0], -v[1], -v[2] }; }

    inline T operator[](int i) const { return v[i]; }
    inline T& operator[](int i) { return v[i]; }

    inline Vec3& operator+=(const Vec3 & rhs);
    inline Vec3& operator-=(const Vec3 & rhs);
    inline Vec3& operator*=(const Vec3 & rhs);
    inline Vec3& operator/=(const Vec3 & rhs);
    inline Vec3& operator*=(T m);
    inline Vec3& operator/=(T m);

    inline T length() const
    {
        return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    }
    inline T squared_length() const
    {
        return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    }

    inline void normalize()
    {
        float k = 1.0 / sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
        v[0] *= k;
        v[1] *= k;
        v[2] *= k;
    }

private:
    T v[3];
};

// definitions
template <typename T> 
inline Vec3<T> & Vec3<T>::operator+=(const Vec3<T> & rhs)
{
    v[0] += rhs.v[0];
    v[1] += rhs.v[1];
    v[2] += rhs.v[2];
    return *this;
}

template <typename T> 
inline Vec3<T> & Vec3<T>::operator-=(const Vec3<T> & rhs)
{
    v[0] -= rhs.v[0];
    v[1] -= rhs.v[1];
    v[2] -= rhs.v[2];
    return *this;
}

template <typename T> 
inline Vec3<T> & Vec3<T>::operator*=(const Vec3<T> & rhs)
{
    v[0] *= rhs.v[0];
    v[1] *= rhs.v[1];
    v[2] *= rhs.v[2];
    return *this;
}

template <typename T> 
inline Vec3<T> & Vec3<T>::operator/=(const Vec3<T> & rhs)
{
    v[0] /= rhs.v[0];
    v[1] /= rhs.v[1];
    v[2] /= rhs.v[2];
    return *this;
}

template <typename T> 
inline Vec3<T> & Vec3<T>::operator*=(T m)
{
    v[0] *= m;
    v[1] *= m;
    v[2] *= m;
    return *this;
}

template <typename T> 
inline Vec3<T> & Vec3<T>::operator/=(T m)
{
    T k = 1.0 / m;
    v[0] *= k;
    v[1] *= k;
    v[2] *= k;
    return *this;
}


template <typename T>
inline Vec3<T> operator+(const Vec3<T> & v1, const Vec3<T> & v2)
{
    return Vec3<T> {v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]};
}

template <typename T>
inline Vec3<T> operator-(const Vec3<T> & v1, const Vec3<T> & v2)
{
    return Vec3<T> {v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]};
}

template <typename T>
inline Vec3<T> operator*(const Vec3<T> & v1, const Vec3<T> & v2)
{
    return Vec3<T> {v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]};
}

template <typename T>
inline Vec3<T> operator/(const Vec3<T> & v1, const Vec3<T> & v2)
{
    return Vec3<T> {v1[0] / v2[0], v1[1] / v2[1], v1[2] / v2[2]};
}

template <typename T>
inline Vec3<T> operator*(T k, const Vec3<T> & v)
{
    return Vec3<T> {k * v[0], k * v[1], k * v[2]};
}

template <typename T>
inline Vec3<T> operator*(const Vec3<T> & v, T k)
{
    return Vec3<T> {k * v[0], k * v[1], k * v[2]};
}

template <typename T>
inline Vec3<T> operator/(const Vec3<T> & v, T k)
{
    return Vec3<T> {v[0] / k, v[1] / k, v[2] / k};
}

template <typename T>
inline T dot(const Vec3<T> & v1, const Vec3<T> & v2)
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

template <typename T>
inline Vec3<T> cross(const Vec3<T> & v1, const Vec3<T> & v2)
{
    return Vec3<T>
    {
        (v1[1] * v2[2] - v1[2] * v2[1]),
        (-(v1[0] * v2[2] - v1[2] * v2[0])),
        (v1[0] * v2[1] - v1[1] * v2[0])
    };
}

template <typename T>
inline Vec3<T> unit_vec(const Vec3<T> & v)
{
    return v / v.length();
}

using vec3 = Vec3<float>;