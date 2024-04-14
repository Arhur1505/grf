//Extremely simple vector and matrix classes by Janusz Malinowski. 
#include <stdio.h>
#pragma once
class Vector
{
 public:
 double data[3];
 Vector();
 Vector(double x, double y);
 void Print(void);
 void Set(double d1,double d2);
 double GetX() const;
 double GetY() const;
};

class Matrix
{
 public:
 double data[3][3];
 Matrix();
 Matrix(double a1, double a2, double a3, double b1, double b2, double b3, double c1, double c2, double c3);
 void Print(void);
 Matrix operator*(const Matrix&) const;
 friend Vector operator*(const Matrix&, const Vector&);
};

