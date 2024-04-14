#include <wx/dc.h>


#include "ChartClass.h"
#include "vecmat.h"

ChartClass::ChartClass(std::shared_ptr<ConfigClass> c)
{
	cfg = std::move(c);
	x_step = 1000;
}

void ChartClass::Set_Range()
{
	double xmin = 9999.9, xmax = -9999.9, ymin = 9999.9, ymax = -9999.9;
	double x, y, step;
	int i;

	xmin = cfg->Get_x_start();
	xmax = cfg->Get_x_stop();

	step = (cfg->Get_x_stop() - cfg->Get_x_start()) / x_step;
	x = cfg->Get_x_start();

	for (i = 0; i <= x_step; i++)
	{
		y = GetFunctionValue(x);
		if (y > ymax) ymax = y;
		if (y < ymin) ymin = y;
		x = x + step;
	}

	y_min = ymin;
	y_max = ymax;
	x_min = xmin;
	x_max = xmax;
}

double ChartClass::GetFunctionValue(double x)
{
	if (cfg->Get_F_type() == 1) return x * x;
	if (cfg->Get_F_type() == 2) return 0.5 * exp(4 * x - 3 * x * x);
	return x + sin(x * 4.0);
}

void ChartClass::Draw(wxDC* dc, int w, int h)
{
	dc->SetBackground(wxBrush(RGB(255, 255, 255)));
	dc->Clear();
	dc->SetPen(wxPen(RGB(255, 0, 0)));
	dc->DrawRectangle(10, 10, w - 20, h - 20);
	dc->SetClippingRegion(10, 10, w - 20, h - 20);
	Set_Range();

	Matrix M;

	/* Zczytuje wartoœci z panelu */

	const double x0 = cfg->Get_x0(), x1 = cfg->Get_x1(), y0 = cfg->Get_y0(), y1 = cfg->Get_y1();
	const double x_start = cfg->Get_x_start(), x_stop = cfg->Get_x_stop();
	const double dx = cfg->Get_dX(), dy = cfg->Get_dY();
	const double krok = (x_max - x_min) / x_step; // uwzglednienie krokow podczas rysowania funkcji
	const double alpha = cfg->Get_Alpha();
	const double disX = (x1 + x0) / 2;
	const double disY = (y1 + y0) / 2;

	if (cfg->RotateScreenCenter())
		M = GetTranslationMatrix(w / 2, h / 2) * GetRotationMatrix(-alpha) * GetTranslationMatrix(-disX / (x1 - x0) * w, disY / (y1 - y0) * h) *
		GetTranslationMatrix(-dx / (x1 - x0) * w, dy / (y1 - y0) * h) * GetScaleMatrix(w - 20, h - 20, x0, x1, y0, y1);
	else
		M = GetTranslationMatrix(-disX / (x1 - x0) * w, disY / (y1 - y0) * h) * GetTranslationMatrix(-dx / (x1 - x0) * w, dy / (y1 - y0) * h) * GetTranslationMatrix(w / 2, h / 2) *
		GetRotationMatrix(-alpha) * GetScaleMatrix(w - 20, h - 20, x0, x1, y0, y1);

	dc->SetPen(wxPen(RGB(0, 0, 255))); // rysuje ukl wsp

	// OY cala
	DrawLine2d(dc, M, 0, y_min - 0.5, 0, y_max);
	DrawText(dc, M, alpha, -0.1, y_max + 0.05, "OY");
	DrawLine2d(dc, M, 0.02, y_max - 0.06, 0, y_max);
	DrawLine2d(dc, M, -0.02, y_max - 0.06, 0, y_max);
	// OX cala
	DrawLine2d(dc, M, x_start, 0, x_stop, 0);
	DrawText(dc, M, alpha, x_stop - 0.07, 0.2, "OX");
	DrawLine2d(dc, M, x_stop - 0.05, -0.05, x_stop, 0);
	DrawLine2d(dc, M, x_stop - 0.05, 0.05, x_stop, 0);

	for (double i = x_start; i <= x_stop; i += (x_stop - x_start) / 7) {
		DrawLine2d(dc, M, i, -0.05, i, 0.05);
		DrawText(dc, M, alpha, i - 0.05, -0.07, i);
		DrawLine2d(dc, M, -0.02, GetFunctionValue(i), 0.02, GetFunctionValue(i));
		DrawText(dc, M, alpha, 0.04, GetFunctionValue(i) + 0.06, GetFunctionValue(i));
	}

	// rysujemy wykres (zloty :D)
	dc->SetPen(wxPen(RGB(255, 215, 0)));
	for (double x = x_min; x <= x_max; x += krok)
		DrawLine2d(dc, M, x, GetFunctionValue(x), x + krok, GetFunctionValue(x + krok));

	// zaznaczamy punkty 
	dc->SetPen(wxPen(RGB(0, 0, 0)));
	for (double i = x_start; i <= x_stop; i += (x_stop - x_start) / 7)
		DrawLine2d(dc, M, i, GetFunctionValue(i) - 0.02, i, GetFunctionValue(i) + 0.02);

}

double ChartClass::Get_Y_min()
{
	Set_Range();
	return y_min;
}

double ChartClass::Get_Y_max()
{
	Set_Range();
	return y_max;
}

void ChartClass::DrawLine2d(wxDC* dc, Matrix M, double x1, double y1, double x2, double y2)
{
	Vector u, v;

	u.Set(x1, y1);// [x1,y1,1]
	v.Set(x2, y2);
	// transformacja punktu
	u = M * u;
	v = M * v;

	dc->DrawLine(u.GetX(), u.GetY(), v.GetX(), v.GetY());
}

void ChartClass::DrawText(wxDC* dc, Matrix M, double alpha, double x, double y, double value)
{
	Vector u;

	u.Set(x, y);
	u = M * u;

	dc->DrawRotatedText(wxString::Format(wxT("%.2f"), value), u.GetX(), u.GetY(), alpha);
}

void ChartClass::DrawText(wxDC* dc, Matrix M, double alpha, double x, double y, std::string value)
{
	Vector u;

	u.Set(x, y);
	u = M * u;

	dc->DrawRotatedText(wxString(value), u.GetX(), u.GetY(), alpha);
}

Matrix GetScaleMatrix(double w, double h, double x0, double x1, double y0, double y1)
{
	Matrix M;

	M.data[0][0] = w / (x1 - x0);
	M.data[1][1] = -h / (y1 - y0); // minus bo y1 jest mniejszy bo zaczynamy "od gory"

	return M;
}

Matrix GetRotationMatrix(double alpha)
{
	Matrix M;

	const double rad = alpha * M_PI / 180.0;

	M.data[0][0] = cos(rad);
	M.data[0][1] = -sin(rad);
	M.data[1][0] = sin(rad);
	M.data[1][1] = cos(rad);

	return M;
}

Matrix GetTranslationMatrix(double dx, double dy)
{
	Matrix M;

	// przesuniecie o dx w kierunku X i dy w kierunku Y
	M.data[0][0] = 1.0;
	M.data[1][1] = 1.0;
	M.data[0][2] = dx;
	M.data[1][2] = dy;

	// [2][2] zawsze 1

	return M;
}
