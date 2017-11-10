//genBTC 2017  Picture 10/14 final draft @ https://puu.sh/xYa55/cddf6c0c11.png                    

/*
(NOTE: The Cluster Map is the parent of all this. Cluster map is a wxWindow with a custom OnPaint handler using Windows primitives.)
Explanation:
1. Entry point @ funcion void ClusterMap::ClusterMapLegend(wxMouseEvent& event)..
2.     m_legendPopup = new LegendTransientPopup(this);
3.     m_drawpane = new BasicDrawPaneLegend(m_window);
4.	   DC calls inside BasicDrawPaneLegend::OnPaint or Draw.

GOAL: All I wanted is to be able to use the DC Draw commands on a blank widget(BasicDrawPaneLegend)
      thats overlaid up out of the popup LegendTransientPopup.
      And put them Next to my Colored textboxes.I am probably doing this really stupidly.
*/
// Enum IDs for the controls and the menu commands are in the headers.

#include "prec.h"
#include "main.h"


// << Map.CPP >> This one works.
void ClusterMap::ClusterMapLegend(wxMouseEvent& WXUNUSED(event))
{
    wxPoint const mousePos = wxGetMousePosition();
    // Not needed. For future reference. Use the mouse to find the WXwindow, and store a pointer in clustermap pointing to itself.(dumb)
    //wxPoint foundMouse;
    //const auto win = reinterpret_cast<ClusterMap*>(wxFindWindowAtPointer(foundMouse));
    //win->m_ClusterMap = win;
    int a, b;
    GetScreenPosition(&a, &b);
    //int c, d;             // not needed.
    //GetPosition(&c, &d);  //returns 2,186 always. (offset of Border / splitter)
    wxPoint relPos;
    relPos.x = mousePos.x - a;
    relPos.y = mousePos.y - b;
    auto ActualLCN = getLCNsfromMousePos(relPos);
    if (ActualLCN)
        dtrace("Actual LCN was %I64u", ActualLCN);
}

//----------------------------------------------------------------------------
// LegendTransientPopup .CPP Implementation
//----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(LegendTransientPopup, wxPopupTransientWindow);
wxBEGIN_EVENT_TABLE(LegendTransientPopup, wxPopupTransientWindow)
END_EVENT_TABLE();
//EVT_MOUSE_EVENTS(LegendTransientPopup::OnMouse)
//EVT_SIZE(LegendTransientPopup::OnSize)
//EVT_SET_FOCUS(LegendTransientPopup::OnSetFocus)
//EVT_KILL_FOCUS(LegendTransientPopup::OnKillFocus)
//EVT_BUTTON(Minimal_PopupButton, LegendTransientPopup::OnButton)
//EVT_SPINCTRL(Minimal_PopupSpinctrl, LegendTransientPopup::OnSpinCtrl)

//This one Works.
LegendTransientPopup::LegendTransientPopup(wxWindow* parent)
    : wxPopupTransientWindow(parent)
{
    wxBoxSizer* bSizer1 = new wxBoxSizer(wxHORIZONTAL);
    m_window = new wxWindow(this, wxID_ANY);
    m_window->SetBackgroundColour(*wxLIGHT_GREY);

    //LEFT COLUMN - launches "Drawcontext"
    //m_drawpane = new BasicDrawPaneLegend(m_window);
    //bSizer1->Add(m_drawpane, 0, wxALL, 5);
    //m_drawpane->Layout();

    // Keep this code to verify if mouse events work, they're required if
    // you're making a control like a combobox where the items are highlighted
    // under the cursor, the m_window is set focus in the Popup() function
    //m_window->Connect(wxEVT_MOTION,wxMouseEventHandler(LegendTransientPopup::OnMouse), NULL, this);

    wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);

    int eI = 0; //enum increment
    m_staticText1 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText1->Wrap(-1);
    m_staticText1->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText1, 0, wxALL, 1);   ++eI;

    m_staticText2 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText2->Wrap(-1);
    m_staticText2->SetBackgroundColour(g_colors[eI]);
    m_staticText2->SetForegroundColour(*wxWHITE);
    bSizer2->Add(m_staticText2, 0, wxALL, 1);   ++eI;

    m_staticText3 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText3->Wrap(-1);
    m_staticText3->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText3, 0, wxALL, 1);   ++eI;

    m_staticText4 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText4->Wrap(-1);
    m_staticText4->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText4, 0, wxALL, 1);   ++eI;

    m_staticText5 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText5->Wrap(-1);
    m_staticText5->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText5, 0, wxALL, 1);   ++eI;

    m_staticText6 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText6->Wrap(-1);
    m_staticText6->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText6, 0, wxALL, 1);   ++eI;

    m_staticText7 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText7->Wrap(-1);
    m_staticText7->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText7, 0, wxALL, 1);   ++eI;

    m_staticText8 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText8->Wrap(-1);
    m_staticText8->SetBackgroundColour(g_colors[eI]);
    m_staticText8->SetForegroundColour(*wxWHITE);
    bSizer2->Add(m_staticText8, 0, wxALL, 1);   ++eI;

    m_staticText9 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText9->Wrap(-1);
    m_staticText9->SetBackgroundColour(g_colors[eI]);
    m_staticText9->SetForegroundColour(*wxWHITE);
    bSizer2->Add(m_staticText9, 0, wxALL, 1);   ++eI;

    m_staticText10 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText10->Wrap(-1);
    m_staticText10->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText10, 0, wxALL, 1);   ++eI;

    m_staticText11 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText11->Wrap(-1);
    m_staticText11->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText11, 0, wxALL, 1);   ++eI;

    m_staticText12 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText12->Wrap(-1);
    m_staticText12->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText12, 0, wxALL, 1);   ++eI;

    m_staticText13 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText13->Wrap(-1);
    m_staticText13->SetBackgroundColour(g_colors[eI]);
    m_staticText13->SetForegroundColour(*wxWHITE);
    bSizer2->Add(m_staticText13, 0, wxALL, 1);   ++eI;

    m_staticText14 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText14->Wrap(-1);
    m_staticText14->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText14, 0, wxALL, 1);   ++eI;

    m_staticText15 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText15->Wrap(-1);
    m_staticText15->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText15, 0, wxALL, 1);   ++eI;

    m_staticText16 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText16->Wrap(-1);
    m_staticText16->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText16, 0, wxALL, 1);   ++eI;

    m_staticText17 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText17->Wrap(-1);
    m_staticText17->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText17, 0, wxALL, 1);   ++eI;

    m_staticText18 = new wxStaticText(m_window, wxID_ANY, g_colornames[eI], wxDefaultPosition, wxDefaultSize, 10);
    m_staticText18->Wrap(-1);
    m_staticText18->SetBackgroundColour(g_colors[eI]);
    bSizer2->Add(m_staticText18, 0, wxALL, 1);   ++eI;

    //Add Middle Column  (static text colored boxes)
    bSizer1->Add(bSizer2, 1, wxALL, 5);

    /*
    //Make a Bit map for right column (square+circle)
    wxImage img(22, 22, true);
    img.Clear(1);
    wxBitmap bmp(img);
    {
        wxMemoryDC mdc(bmp);
        mdc.SetPen(*wxRED_PEN);
        mdc.SetBrush(*wxWHITE_BRUSH);
        mdc.SetBackground(*wxYELLOW);
        mdc.DrawCircle(10, 10, 11);
        wxColour c;
        mdc.SetBrush(*wxGREEN_BRUSH);
        mdc.FloodFill(12, 12, c, wxFLOOD_SURFACE);
    }

    wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);

    m_bpButton1 = new wxBitmapButton(m_window, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    bSizer3->Add(m_bpButton1, 0, wxALL, 5);

    m_bpButton2 = new wxBitmapButton(m_window, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    bSizer3->Add(m_bpButton2, 0, wxALL, 5);

    m_bpButton3 = new wxBitmapButton(m_window, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    bSizer3->Add(m_bpButton3, 0, wxALL, 5);

    m_bpButton4 = new wxBitmapButton(m_window, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    bSizer3->Add(m_bpButton4, 0, wxALL, 5);

    m_bpButton5 = new wxBitmapButton(m_window, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    bSizer3->Add(m_bpButton5, 0, wxALL, 5);

    m_bpButton6 = new wxBitmapButton(m_window, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    bSizer3->Add(m_bpButton6, 0, wxALL, 5);

    m_bpButton7 = new wxBitmapButton(m_window, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    bSizer3->Add(m_bpButton7, 0, wxALL, 5);

    m_bpButton8 = new wxBitmapButton(m_window, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    bSizer3->Add(m_bpButton8, 0, wxALL, 5);

    //Add Right Column (Bitmap column)
    bSizer1->Add(bSizer3, 1, wxALL, 5);
    */

    //Add All.
    bSizer1->Fit(m_window);
    SetSizerAndFit(bSizer1);
}

//THIS IS THE ACTUAL POPUP BEING SHOWN 1 
void LegendTransientPopup::Popup(wxWindow* WXUNUSED(focus))
{
    //wxLogMessage("%p LegendTransientPopup::Popup 1", this);
    wxPopupTransientWindow::Popup();
}

//THIS IS THE ACTUAL POPUP BEING SHOWN 2
bool LegendTransientPopup::Show(bool show)
{
    //wxLogMessage("%p LegendTransientPopup::Show 2 %d", this, int(show));
    return wxPopupTransientWindow::Show(show);
}

void LegendTransientPopup::OnDismiss()
{
    //wxLogMessage("%p LegendTransientPopup::OnDismiss 3", this);
    wxPopupTransientWindow::OnDismiss();
}

bool LegendTransientPopup::ProcessLeftDown(wxMouseEvent& event)
{
    //wxLogMessage("%p LegendTransientPopup::ProcessLeftDown pos(%d, %d)", this, event.GetX(), event.GetY());
    return wxPopupTransientWindow::ProcessLeftDown(event);
}

void LegendTransientPopup::OnSize(wxSizeEvent& event)
{
    //wxLogMessage("%p LegendTransientPopup::OnSize", this);
    event.Skip();
}

void LegendTransientPopup::OnSetFocus(wxFocusEvent& event)
{
    //wxLogMessage("%p LegendTransientPopup::OnSetFocus", this);
    event.Skip();
}

void LegendTransientPopup::OnKillFocus(wxFocusEvent& event)
{
    //wxLogMessage("%p LegendTransientPopup::OnKillFocus", this);
    event.Skip();
}

void LegendTransientPopup::OnMouse(wxMouseEvent& event)
{
    //wxLogMessage("%p LegendTransientPopup::OnMouse", this);
    event.Skip();
}

void LegendTransientPopup::OnButton(wxCommandEvent& event)
{
    //wxLogMessage("%p LegendTransientPopup::OnButton ID %d", this, event.GetId());

    wxButton* button = wxDynamicCast(event.GetEventObject(), wxButton);
    if (button->GetLabel() == wxT("Press Me"))
        button->SetLabel(wxT("Pressed"));
    else
        button->SetLabel(wxT("Press Me"));

    event.Skip();
}

//----------------------------------------------------------------------------
// BasicDrawPaneLegend .CPP Implementation 
//----------------------------------------------------------------------------
wxIMPLEMENT_CLASS(BasicDrawPaneLegend, wxWindow);
BEGIN_EVENT_TABLE(BasicDrawPaneLegend, wxWindow)
EVT_PAINT(BasicDrawPaneLegend::OnPaint) // catch paint events
END_EVENT_TABLE()

//Constructor.
BasicDrawPaneLegend::BasicDrawPaneLegend(wxWindow* parent) : wxWindow(parent, wxID_ANY,
    wxDefaultPosition, wxDefaultSize, wxPOPUP_WINDOW)
{
    m_textureBackground = 1;
    m_mapMode = wxMM_TEXT;
    m_xUserScale = 1;
    m_yUserScale = 1;
    m_xLogicalOrigin = 0;
    m_yLogicalOrigin = 0;
    m_xAxisReversed = false;
    m_yAxisReversed = false;
    wxColour m_colourForeground(*wxBLUE);
    wxColour m_colourBackground(*wxWHITE);
    wxBrush m_backgroundBrush(*wxLIGHT_GREY_BRUSH);
    m_renderer = nullptr;
    m_owner = parent;   //This is the line that made it work.
    m_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPOPUP_WINDOW | wxBORDER);
    m_panel->SetBackgroundColour(*wxCYAN);
    //    m_panel->Connect(wxEVT_MOTION, wxMouseEventHandler(BasicDrawPaneLegend::OnMouse), NULL, this);

    wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);

    //These show up.
    wxStaticText* m_staticText1 = new wxStaticText(m_panel, wxID_ANY, wxT("LEFT color1\n"), wxDefaultPosition,
        wxDefaultSize, 20);
    m_staticText1->Wrap(-1);
    m_staticText1->SetBackgroundColour(*wxRED);
    wxStaticText* m_staticText2 = new wxStaticText(m_panel, wxID_ANY, wxT("LEFT color2\n"), wxDefaultPosition,
        wxDefaultSize, 20);
    m_staticText2->Wrap(-1);
    m_staticText2->SetBackgroundColour(*wxGREEN);
    wxStaticText* m_staticText3 = new wxStaticText(m_panel, wxID_ANY, wxT("LEFT color3\n"), wxDefaultPosition,
        wxDefaultSize, 20);
    m_staticText3->Wrap(-1);
    m_staticText3->SetBackgroundColour(*wxBLUE);

    bSizer1->Add(m_staticText1, 0, wxALL, 1);
    bSizer1->Add(m_staticText2, 0, wxALL, 5);
    bSizer1->Add(m_staticText3, 0, wxALL | wxEXPAND, 20);

    m_panel->SetSizerAndFit(bSizer1);
    m_panel->Layout();

    //SetClientSize(m_panel->GetSize());
}

BasicDrawPaneLegend::~BasicDrawPaneLegend()
{
    //delete m_owner;   //This causes a crash.
    //delete m_panel;     //this doesn't.
    //delete m_renderer;
    //delete m_canvas;
}

void BasicDrawPaneLegend::Draw(wxDC& pdc)
{
#if wxUSE_GRAPHICS_CONTEXT
    wxGCDC gdc;

    if (m_renderer)
    {
        wxGraphicsContext* context;
        if (wxPaintDC* paintdc = wxDynamicCast(&pdc, wxPaintDC))
        {
            context = m_renderer->CreateContext(*paintdc);
        }
        else if (wxMemoryDC* memdc = wxDynamicCast(&pdc, wxMemoryDC))
        {
            context = m_renderer->CreateContext(*memdc);
        }
#if wxUSE_METAFILE && defined(wxMETAFILE_IS_ENH)
        else if (wxMetafileDC *metadc = wxDynamicCast(&pdc, wxMetafileDC))
        {
            context = m_renderer->CreateContext(*metadc);
        }
#endif
        else
        {
            wxFAIL_MSG("Unknown wxDC kind");
            return;
        }

        gdc.SetGraphicsContext(context);
    }

    wxDC& dc = m_renderer ? (wxDC&)gdc : (wxDC&)pdc;
#else
    wxDC &dc = pdc;
#endif
    PrepareDC(dc);

    dc.SetClippingRegion(100, 100, 100, 100);

    dc.Clear();

    PrepareDC(dc);
    dc.SetBackground(wxBrush(wxColour(0, 128, 0)));
    dc.SetClippingRegion(100, 100, 100, 100);

    // Draw circle centered at the origin, then flood fill it with a different
    // color. Done with a wxMemoryDC because Blit (used by generic
    // wxDoFloodFill) from a window that is being painted gives unpredictable
    // results on wxGTK
    {
        wxImage img(21, 21, false);
        img.Clear(1);
        wxBitmap bmp(img);
        {
            wxMemoryDC mdc(bmp);
            mdc.SetBrush(dc.GetBrush());
            mdc.SetPen(dc.GetPen());
            mdc.DrawCircle(10, 10, 10);
            wxColour c;
            if (mdc.GetPixel(11, 11, &c))
            {
                mdc.SetBrush(wxColour(128, 128, 0));
                mdc.FloodFill(11, 11, c, wxFLOOD_SURFACE);
            }
        }
        bmp.SetMask(new wxMask(bmp, wxColour(1, 1, 1)));
        dc.DrawBitmap(bmp, -10, -10, true);
    }

    dc.DrawCheckMark(5, 80, 15, 15);
    dc.DrawCheckMark(25, 80, 30, 30);
    dc.DrawCheckMark(60, 80, 60, 60);

    // this is the test for "blitting bitmap into DC damages selected brush" bug
    wxCoord rectSize = 24 + 10;
    wxCoord x = 100;
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxGREEN_BRUSH);
    dc.DrawRectangle(x, 10, rectSize, rectSize);
    x += rectSize + 10;
    dc.DrawRectangle(x, 10, rectSize, rectSize);
    x += rectSize + 10;
    dc.DrawRectangle(x, 10, rectSize, rectSize);

    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(0, 160, 1000, 300);

    // draw lines
    wxBitmap bitmap(20, 70);
    wxMemoryDC memdc;
    memdc.SelectObject(bitmap);
    memdc.SetBrush(*wxBLACK_BRUSH);
    memdc.SetPen(*wxWHITE_PEN);
    memdc.DrawRectangle(0, 0, 20, 70);
    memdc.DrawLine(10, 0, 10, 70);

    // to the right
    wxPen pen = *wxRED_PEN;
    memdc.SetPen(pen);
    memdc.DrawLine(10, 5, 10, 5);
    memdc.DrawLine(10, 10, 11, 10);
    memdc.DrawLine(10, 15, 12, 15);
    memdc.DrawLine(10, 20, 13, 20);

    // same to the left
    memdc.DrawLine(10, 25, 10, 25);
    memdc.DrawLine(10, 30, 9, 30);
    memdc.DrawLine(10, 35, 8, 35);
    memdc.DrawLine(10, 40, 7, 40);

    // XOR draw lines
    dc.SetPen(*wxWHITE_PEN);
    memdc.SetLogicalFunction(wxINVERT);
    memdc.SetPen(*wxWHITE_PEN);
    memdc.DrawLine(10, 50, 10, 50);
    memdc.DrawLine(10, 55, 11, 55);
    memdc.DrawLine(10, 60, 12, 60);
    memdc.DrawLine(10, 65, 13, 65);

    memdc.DrawLine(12, 50, 12, 50);
    memdc.DrawLine(12, 55, 13, 55);
    memdc.DrawLine(12, 60, 14, 60);
    memdc.DrawLine(12, 65, 15, 65);

    memdc.SelectObject(wxNullBitmap);
    dc.DrawBitmap(bitmap, 10, 170);
    wxImage image = bitmap.ConvertToImage();
    image.Rescale(60, 210);
    bitmap = wxBitmap(image);
    dc.DrawBitmap(bitmap, 50, 170);

    // test the rectangle outline drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(150, 170, 49, 29);
    dc.DrawRectangle(200, 170, 49, 29);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(250, 210, 250, 170);
    dc.DrawLine(260, 200, 150, 200);

    // test the rectangle filled drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(300, 170, 49, 29);
    dc.DrawRectangle(350, 170, 49, 29);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(400, 170, 400, 210);
    dc.DrawLine(300, 200, 410, 200);

    // a few more tests of this kind
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(300, 220, 1, 1);
    dc.DrawRectangle(310, 220, 2, 2);
    dc.DrawRectangle(320, 220, 3, 3);
    dc.DrawRectangle(330, 220, 4, 4);

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(300, 230, 1, 1);
    dc.DrawRectangle(310, 230, 2, 2);
    dc.DrawRectangle(320, 230, 3, 3);
    dc.DrawRectangle(330, 230, 4, 4);

    // and now for filled rect with outline
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(500, 170, 49, 29);
    dc.DrawRectangle(550, 170, 49, 29);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(600, 170, 600, 210);
    dc.DrawLine(500, 200, 610, 200);

    // test the rectangle outline drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRoundedRectangle(150, 270, 49, 29, 6);
    dc.DrawRoundedRectangle(200, 270, 49, 29, 6);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(250, 270, 250, 310);
    dc.DrawLine(150, 300, 260, 300);

    // test the rectangle filled drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRoundedRectangle(300, 270, 49, 29, 6);
    dc.DrawRoundedRectangle(350, 270, 49, 29, 6);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(400, 270, 400, 310);
    dc.DrawLine(300, 300, 410, 300);

    // Added by JACS to demonstrate bizarre behaviour.
    // With a size of 70, we get a missing red RHS,
    // and the height is too small, so we get yellow
    // showing. With a size of 40, it draws as expected:
    // it just shows a white rectangle with red outline.
    int totalWidth = 70;
    int totalHeight = 70;
    wxBitmap bitmap2(totalWidth, totalHeight);

    wxMemoryDC memdc2;
    memdc2.SelectObject(bitmap2);

    memdc2.SetBackground(*wxYELLOW_BRUSH);
    memdc2.Clear();

    // Now draw a white rectangle with red outline. It should
    // entirely eclipse the yellow background.
    memdc2.SetPen(*wxRED_PEN);
    memdc2.SetBrush(*wxWHITE_BRUSH);

    memdc2.DrawRectangle(0, 0, totalWidth, totalHeight);

    memdc2.SetPen(wxNullPen);
    memdc2.SetBrush(wxNullBrush);
    memdc2.SelectObject(wxNullBitmap);

    dc.DrawBitmap(bitmap2, 500, 270);

    // Repeat, but draw directly on dc
    // Draw a yellow rectangle filling the bitmap

    x = 600;
    int y = 270;
    dc.SetPen(*wxYELLOW_PEN);
    dc.SetBrush(*wxYELLOW_BRUSH);
    dc.DrawRectangle(x, y, totalWidth, totalHeight);

    // Now draw a white rectangle with red outline. It should
    // entirely eclipse the yellow background.
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);

    dc.DrawRectangle(x, y, totalWidth, totalHeight);
    //
    PrepareDC(dc);

    /* DC display context NOT WORKING
    //wxBoxSizer* bSizer1 = new wxBoxSizer(wxHORIZONTAL);
    wxPaintDC dc(m_panel);
    // draw some text
    dc.DrawText(wxT("Testing"), 40, 60);

    // draw a circle
    dc.SetBrush(*wxGREEN_BRUSH); // green filling
    dc.SetPen(wxPen(wxColor(255, 0, 0), 5)); // 5-pixels-thick red outline
    dc.DrawCircle(wxPoint(100, 50), 25 //radius);

    // draw a rectangle
    wxBrush brush(*wxBLUE_BRUSH); // blue filling
    wxPen pen(wxColor(255, 175, 175), 10); // 10-pixels-thick pink outline
    wxRect r = wxRect(200, 10, 50, 50);
    // draw a line
    dc.SetPen(wxPen(wxColor(0, 0, 0), 3)); // black line, 3 pixels thick
    dc.DrawLine(100, 100, 100, 100); // draw line across the rectangle
    //Add Both.
    */
}

void BasicDrawPaneLegend::OnShow(wxCommandEvent& event)
{
#if wxUSE_GRAPHICS_CONTEXT
    if (!m_canvas->HasRenderer())
        m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetDefaultRenderer());
#endif
}


#if wxUSE_COLOURDLG
wxColour BasicDrawPaneLegend::SelectColour()
{
    wxColour col;
    wxColourData data;
    wxColourDialog dialog(this, &data);

    if (dialog.ShowModal() == wxID_OK)
    {
        col = dialog.GetColourData().GetColour();
    }

    return col;
}
#endif // wxUSE_COLOURDLG


/** @} */
