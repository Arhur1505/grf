#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbmp.h> 
#include <wx/filedlg.h>
#include <wx/file.h>
#include <wx/gauge.h>
#include <wx/colordlg.h>
#include <string>
#include <wx/colour.h> 

using namespace std;
float promien = sqrt(40 * 40 + 50 * 50), up;

class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow* parent) : wxPanel(parent)
    {
        Bind(wxEVT_PAINT, &MyPanel::OnPaint, this);
        Bind(wxEVT_SIZE, &MyPanel::OnSize, this);

        wxImage image;
        if (image.LoadFile("banan.png", wxBITMAP_TYPE_PNG)) {
            bitmapa = new wxBitmap(image);
        }
        else {
            wxLogError("Nie można wczytać obrazu PNG.");
            return;
        }
    }
    int current_shape = 0;
    int smile = 0; 
    wxColour color_star = *wxBLACK; 
    wxString tekst = "tekst"; 
    int banan_pozycja = 0;

    wxBitmap* bitmapa;
private:
    wxPoint center;

    void OnSize(wxSizeEvent& event) {
        wxSize size = GetSize();
        center.x = size.GetWidth() / 2;
        center.y = size.GetHeight() / 2;
        Refresh();
    }

    void OnPaint(wxPaintEvent& event)
    {
        wxPen transparentPen(*wxTRANSPARENT_PEN);
        wxBrush brush(color_star, wxBRUSHSTYLE_SOLID);
        wxPen pen(*wxBLACK, 1);
        wxPaintDC dc(this);
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
        dc.Clear();
        wxString textToDraw = tekst;
        wxString textToDraw_2 = tekst;
        double maxAngle = M_PI / 3.0;
        double currentAngle = maxAngle * banan_pozycja / 100;
        int x = center.x - 100;
        int y = center.y - 100;
        int size = 50;
        switch (current_shape) {
        case 0: {
            dc.SetBrush(brush);
            wxPoint points[5];
            points[0] = wxPoint(x, y - size);
            points[1] = wxPoint(x + size * 0.5878, y + size * 0.8090);
            points[2] = wxPoint(x - size * 0.9511, y - size * 0.3090);
            points[3] = wxPoint(x + size * 0.9511, y - size * 0.3090);
            points[4] = wxPoint(x - size * 0.5878, y + size * 0.8090);

            dc.DrawPolygon(5, points);
        }
              break;
        case 1: {
            dc.SetBrush(*wxGREY_BRUSH);

            dc.DrawCircle(x, y, size);
            dc.SetBrush(*wxWHITE_BRUSH);
            dc.SetPen(*wxWHITE_PEN);
            dc.DrawCircle(x - 20, y - 20, size);

            wxPen normalPen(*wxBLACK, 1);

            dc.SetPen(normalPen);
            dc.SetBrush(*wxBLACK_BRUSH);

            dc.DrawCircle(x + 19, y - 10, 5);
            dc.DrawLine(x - 15, y + 20, x - 5, y + 25);
        }
              break;
        case 2: {
            dc.SetBrush(*wxYELLOW_BRUSH);
            dc.DrawCircle(x, y, size - 10);
        }
              break;
        }
        dc.SetPen(pen);
        dc.SetBrush(*wxWHITE_BRUSH);
        
        float pointx = center.x - promien * sin(currentAngle + M_PI / 4);
        float pointy = center.y + promien * cos(currentAngle + M_PI / 4);

        dc.DrawLine(center.x, center.y, center.x + promien * sin(M_PI / 4), center.y + promien * cos(M_PI / 4));
        dc.DrawLine(pointx, pointy, center.x, center.y);
        dc.DrawLine(center.x, center.y - 20, center.x, center.y + 100);
        dc.DrawLine(center.x, center.y + 100, center.x + 50, center.y + 160);
        dc.DrawLine(center.x, center.y + 100, center.x - 50, center.y + 160);
        dc.DrawCircle(center.x, center.y - 40, 20);
        dc.DrawEllipse(center.x - 15, center.y - 45, 10, 5);
        if (!smile) {
            dc.DrawEllipticArc(center.x - 12, center.y - 30, 25, 20, 40, 140);
            dc.DrawEllipse(center.x + 5, center.y - 50, 5, 10);
        }
        else {
            dc.DrawEllipticArc(center.x - 12, center.y - 45, 25, 20, 220, 320);
            dc.DrawEllipse(center.x + 5, center.y - 45, 10, 5);
            dc.DrawBitmap(*bitmapa, pointx - 20, pointy - 30, true);
        }
        dc.DrawText(textToDraw, center.x - 100, center.y + 150);

        dc.SetFont(wxFont(25, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD, false));
        dc.DrawRotatedText(textToDraw_2, center.x + 60, center.y + 170, 90);
    }
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
    {
        choices.Add(wxT("Gwiazdka"));
        choices.Add(wxT("Księżyc"));
        choices.Add(wxT("Słonko"));
        myPanel = new MyPanel(this);
        myPanel->Refresh();
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        banan = new wxCheckBox(this, ID_banana, wxT("Banan"));
        scrollBar = new wxScrollBar(this, ID_angle);
        scrollBar->SetScrollbar(0, 1, 100, 1);
        gauge = new wxGauge(this, ID_gauge, 100);
        star_color = new wxButton(this, ID_star, wxT("Kolor gwiazdki"), wxDefaultPosition, wxDefaultSize);
        text = new wxTextCtrl(this, ID_text, "tekst", wxDefaultPosition, wxDefaultSize, 0);
        choice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
        choice->SetSelection(0);
        sizer->Add(new wxButton(this, ID_save_file, wxT("Zapisz plik")), 1, wxALIGN_CENTER_HORIZONTAL | wxTOP, 10);
        sizer->Add(banan, 1, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 10);
        sizer->Add(gauge, 1, wxEXPAND | wxTOP | wxBOTTOM, 10);
        sizer->Add(scrollBar, 1, wxEXPAND | wxTOP | wxBOTTOM, 10);
        sizer->Add(star_color, 1, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 10);
        sizer->Add(text, 1, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 10);
        sizer->Add(choice, 1, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 10);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

        mainSizer->Add(myPanel, 7, wxEXPAND | wxALL, 5);
        mainSizer->Add(sizer, 3, wxALIGN_TOP | wxALL, 5);

        SetSizerAndFit(mainSizer);
        scrollBar->Disable();
        Bind(wxEVT_BUTTON, &MyFrame::save_file, this, ID_save_file);
        banan->Bind(wxEVT_CHECKBOX, &MyFrame::onBananaCheck, this);
        scrollBar->Bind(wxEVT_SCROLL_THUMBTRACK, &MyFrame::OnScroll, this);
        scrollBar->Bind(wxEVT_SCROLL_CHANGED, &MyFrame::OnScroll, this);
        star_color->Bind(wxEVT_BUTTON, &MyFrame::OnColorChoose, this);
        text->Bind(wxEVT_TEXT, &MyFrame::OnTextEnter, this);
        choice->Bind(wxEVT_CHOICE, &MyFrame::OnChoiceMade, this);
        SetIcon(wxNullIcon);

        Centre();
        Show(true);
    }
    void save_file(wxCommandEvent& e) {
        wxClientDC dc(myPanel);
        wxSize panelSize = myPanel->GetSize();
        wxBitmap to_save(panelSize);

        wxMemoryDC memDC;
        memDC.SelectObject(to_save);

        memDC.Blit(0, 0, panelSize.GetWidth(), panelSize.GetHeight(), &dc, 0, 0);

        wxString filename = wxFileSelector("Save As", "", "", "png", "PNG files (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);
        if (!filename.empty()) {
            wxBitmapType type = wxBITMAP_TYPE_BMP; // Format obrazu (PNG)
            to_save.SaveFile(filename, type);
        }

    }

    void onBananaCheck(wxCommandEvent& e) {
        if (e.IsChecked()) {
            scrollBar->Enable();
            myPanel->smile = 1;
        }
        else {
            scrollBar->Disable();
            myPanel->smile = 0;
        }
        myPanel->Refresh();
    }

    void OnTextEnter(wxCommandEvent& event) {
        myPanel->tekst = event.GetString();
        myPanel->Refresh();
    }

    void OnChoiceMade(wxCommandEvent& event) {
        myPanel->current_shape = event.GetSelection();
        myPanel->Refresh();
    }

    void OnScroll(wxScrollEvent& event) {
        int position = event.GetPosition();
        gauge->SetValue(position);
        myPanel->banan_pozycja = position;
        myPanel->Refresh();
    }

    void OnColorChoose(wxCommandEvent& event)
    {
        wxColourDialog dialog(this);
        if (dialog.ShowModal() == wxID_OK)
        {
            wxColour colour = dialog.GetColourData().GetColour();
            myPanel->color_star = colour;
            myPanel->Refresh();
        }

    }

private:
    wxGauge* gauge;
    wxButton* star_color;
    wxTextCtrl* text;
    wxArrayString choices;
    wxChoice* choice;
    MyPanel* myPanel;
    wxCheckBox* banan;
    wxScrollBar* scrollBar;
    enum
    {
        ID_save_file = 1,
        ID_banana = 2,
        ID_angle = 3,
        ID_gauge = 4,
        ID_star = 5,
        ID_text = 6,
        ID_choice = 7
    };

};

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};
bool MyApp::OnInit() {
    wxImage::AddHandler(new wxPNGHandler);

    SetProcessDPIAware();
    MyFrame* frame = new MyFrame("Gra");
    frame->Show(true);
    return true;
}
wxIMPLEMENT_APP(MyApp);
