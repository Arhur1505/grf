#ifndef __ChartClass__
#define __ChartClass__

#include <memory>
#include "ConfigClass.h"
class Matrix;

class ChartClass
{
public:
	void Set_Range();   // ustala wartosci zmiennych x_min,y_min,x_max,y_max
	double Get_Y_min(); // zwraca y_min
	double Get_Y_max(); // zwraca y_max
	void Draw(wxDC* dc, int w, int h);  // rysuje wykres
	ChartClass(std::shared_ptr<ConfigClass> c);

private:
	static void DrawLine2d(wxDC* dc, Matrix t, double x1, double y1, double x2, double y2); // transformuje odcinki
	static void DrawText(wxDC* dc, Matrix t, double alpha, double x, double y, double str);
	static void DrawText(wxDC* dc, Matrix t, double alpha, double x, double y, std::string str);

	std::shared_ptr<ConfigClass> cfg;
	int x_step;         // liczba odcinkow z jakich bedzie sie skladal wykres
	double x_min, x_max; // zakres zmiennej x
	double y_min, y_max; // zakres wartosci przyjmowanych przez funkcje
	double GetFunctionValue(double x); // zwraca wartosci rysowanej funkcji

};

Matrix GetScaleMatrix(double w, double h, double x0, double x1, double y0, double y1);
Matrix GetRotationMatrix(double alpha);
Matrix GetTranslationMatrix(double dx, double dy);

#endif