#include <math.h>
#include <stdlib.h>
#include <fstream>		// to write results to ppm file.
using namespace std;

class Vector3
{
public:
	Vector3() {}

	Vector3(float e0, float e1, float e2)
	{
		e[0] = e0;
		e[1] = e1;
		e[2] = e2;
	}

	inline float x() const { return e[0]; }
	inline float y() const { return e[1]; }
	inline float z() const { return e[2]; }

	inline float r() const { return e[0]; }
	inline float g() const { return e[1]; }
	inline float b() const { return e[2]; }

	float e[3];

	inline float length() const
	{
		return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	}
};

inline std::istream& operator>>(std::istream& is, Vector3& t)
{
	is >> t.e[0] >> t.e[1] >> t.e[2];
	return is;
}

inline std::ostream& operator<<(std::ostream &os, const Vector3& t)
{
	os << t.e[0] << " " << t.e[1] << " " << t.e[2];
	return os;
}

inline Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline Vector3 operator*(float t, const Vector3& v)
{
	return Vector3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vector3 operator*(const Vector3& v, float t)
{
	return Vector3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vector3 operator/(const Vector3& v, float t)
{
	return Vector3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline Vector3 unitVector(Vector3 v)
{
	return v / v.length();
}

inline float dot(const Vector3& v1, const Vector3& v2)
{
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

class Ray
{
public:
	Ray() {}
	Ray(const Vector3& o, const Vector3& d) :
		origin(o),
		direction(d)
	{}

	Vector3 pointAtParameter(float t) const
	{
		return origin + t * direction;
	}

	Vector3 origin;
	Vector3 direction;
};

/*

formula for sphere at the origin:
x*x + y*y + z*z == R*R

formula for sphere at any position:
(x-cx)*(x-cx) + (y-cy)*(y-cy) + (z-cz)*(z-cz) == R*R

in vector form (p = point ray hits the sphere, c = sphere center):
dot((p-c),(p-c)) == R*R

adding formula for our ray (p(t) = point on ray at time t:
dot((p(t)-c),(p(t)-c)) == R*R

full formula for using ray (ray formula is A + t*B):
dot((A + t*B -c),(A + t*B -c)) == R*R

FOIL, etc.:
t*t*dot(B,B) + 2*t*dot(B, A-C) + dot(A-C, A-C) - R*R == 0

A, B, and C are constants, so we solve the quadratic in the function below.

*/

bool HitSphere(const Vector3& center, float radius, const Ray& ray)
{
	Vector3 rayOriginToCenter = ray.origin - center;

	float a = dot(ray.direction, ray.direction);
	float b = 2.0f * dot(rayOriginToCenter, ray.direction);
	float c = dot(rayOriginToCenter, rayOriginToCenter) - radius * radius;
	float discriminant = b * b - 4.0f * a * c;

	return discriminant > 0;
}

Vector3 Color(const Ray& ray)
{
	if (HitSphere(Vector3(0, 0, -1), 0.5f, ray))
	{
		return Vector3(1, 0, 0);
	}

	// normalize ray direction
	Vector3 unitDirection = unitVector(ray.direction);

	// map [-1, 1] y value to [0, 1] range
	float t = 0.5f * (unitDirection.y() + 1.0f);

	// linear blend from white to sky blue
	return (1.0f - t) * Vector3(1, 1, 1) + t * Vector3(0.5f, 0.7f, 1);
}

int main()
{
	int width = 200;
	int height = 100;

	ofstream outFile;
	outFile.open("render.ppm");

	outFile << "P3\n" << width << " " << height << "\n255\n";

	Vector3 lowerLeft(-2, -1, -1);
	Vector3 horizontal(4, 0, 0);
	Vector3 vertical(0, 2, 0);
	Vector3 origin(0, 0, 0);

	for (int h = height - 1; h >= 0; --h)
	{
		for (int w = 0; w < width; ++w)
		{
			float u = w / (float)width;
			float v = h / (float)height;
			Ray ray(origin, lowerLeft + u * horizontal + v * vertical);

			Vector3 pixelColor = Color(ray);

			int intRed = int(255.99f * pixelColor.r());
			int intGreen = int(255.99f * pixelColor.g());
			int intBlue = int(255.99f * pixelColor.b());

			outFile << intRed << " " << intGreen << " " << intBlue << endl;
		}
	}

	outFile.close();
}


