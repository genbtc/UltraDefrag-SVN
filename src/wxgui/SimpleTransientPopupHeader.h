#pragma once

//----------------------------------------------------------------------------
// LegendTransientPopup Header.h
//----------------------------------------------------------------------------
class LegendTransientPopup : public wxPopupTransientWindow
{
public:
    explicit LegendTransientPopup(wxWindow* parent);
    //virtual ~LegendTransientPopup();

    void Popup(wxWindow* focus = nullptr) wxOVERRIDE;
    void OnDismiss() wxOVERRIDE;
    bool ProcessLeftDown(wxMouseEvent& event) wxOVERRIDE;
    bool Show(bool show = true) wxOVERRIDE;

    wxWindow* m_owner;  //PARENT
    wxWindow* m_window;
    wxWindow* m_drawpane;

private:
    void OnMouse(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnButton(wxCommandEvent& event);

    wxStaticText* m_mouseText;
    wxStaticText* m_staticText1;
    wxStaticText* m_staticText2;
    wxStaticText* m_staticText3;
    wxStaticText* m_staticText4;
    wxStaticText* m_staticText5;
    wxStaticText* m_staticText6;
    wxStaticText* m_staticText7;
    wxStaticText* m_staticText8;
    wxStaticText* m_staticText9;
    wxStaticText* m_staticText10;
    wxStaticText* m_staticText11;
    wxStaticText* m_staticText12;
    wxStaticText* m_staticText13;
    wxStaticText* m_staticText14;
    wxStaticText* m_staticText15;
    wxStaticText* m_staticText16;
    wxStaticText* m_staticText17;
    wxStaticText* m_staticText18;
    wxStaticText* m_staticText19;

    wxBitmapButton* m_bpButton1;
    wxBitmapButton* m_bpButton2;
    wxBitmapButton* m_bpButton3;
    wxBitmapButton* m_bpButton4;
    wxBitmapButton* m_bpButton5;
    wxBitmapButton* m_bpButton6;
    wxBitmapButton* m_bpButton7;
    wxBitmapButton* m_bpButton8;
    int m_width;
    int m_height;

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_ABSTRACT_CLASS(LegendTransientPopup)
    DECLARE_EVENT_TABLE()
};

class MyCanvas; //forward dec ok 
                //----------------------------------------------------------------------------
                // BasicDrawPaneLegendPopup Header.h
                //----------------------------------------------------------------------------
class BasicDrawPaneLegend : public wxWindow
{
public:
    explicit BasicDrawPaneLegend(wxWindow* parent);
    virtual ~BasicDrawPaneLegend();
private:
    MyCanvas* m_canvas;

    virtual void Draw(wxDC& dc);
    void OnShow(wxCommandEvent& event);
    //void PrepareDC(wxDC& dc) wxOVERRIDE;
    void PrepareDC(wxDC& dc) wxOVERRIDE
    {
        dc.SetLogicalOrigin(m_xLogicalOrigin, m_yLogicalOrigin);
        dc.SetAxisOrientation(m_xAxisReversed, !m_yAxisReversed);
        dc.SetUserScale(m_xUserScale, m_yUserScale);
        dc.SetMapMode(wxMM_TEXT);
    }

    virtual void OnPaint(wxPaintEvent& evt)
    {
        wxPaintDC pdc(this);
        Draw(pdc);
    }

#if wxUSE_GRAPHICS_CONTEXT
    wxGraphicsRenderer* m_renderer;
#endif
#if wxUSE_COLOURDLG
    wxColour SelectColour();
#endif

    wxWindow* m_owner;  //possible PARENT
    wxPanel*  m_panel;  //PARENT

                        //int         m_backgroundMode;
    int m_textureBackground;
    wxMappingMode m_mapMode;
    double m_xUserScale;
    double m_yUserScale;
    int m_xLogicalOrigin;
    int m_yLogicalOrigin;
    bool m_xAxisReversed,
        m_yAxisReversed;
    wxColour m_colourForeground, // these are _text_ colours
        m_colourBackground;
    wxBrush m_backgroundBrush;

    DECLARE_ABSTRACT_CLASS(BasicDrawPaneLegend)
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// MyCanvas Header.h
//----------------------------------------------------------------------------
class MyCanvas : public BasicDrawPaneLegend
{
public:
    explicit MyCanvas::MyCanvas(BasicDrawPaneLegend* parent)
        : BasicDrawPaneLegend(parent)
    {
        m_show = 0;
        m_owner = parent;
        m_std_icon = wxArtProvider::GetIcon(wxART_INFORMATION);
        m_clip = false;
        m_rubberBand = false;
#if wxUSE_GRAPHICS_CONTEXT
        m_renderer = nullptr;
#endif
        m_useBuffer = false;
    }

    void Draw(wxDC& pdc);

    void OnPaint(wxPaintEvent& evt)
    {
        wxPaintDC pdc(this);
        Draw(pdc);
    }

    void OnMouseMove(wxMouseEvent& event);

    void ToShow(int show)
    {
        m_show = show;
        Refresh();
    }

    // set or remove the clipping region
    void Clip(bool clip)
    {
        m_clip = clip;
        Refresh();
    }
#if wxUSE_GRAPHICS_CONTEXT
    bool HasRenderer() const { return m_renderer != nullptr; }

    void UseGraphicRenderer(wxGraphicsRenderer* renderer)
    {
        m_renderer = renderer;
        Refresh();
    }

    bool IsDefaultRenderer() const
    {
        if (!m_renderer) return false;
        return m_renderer == wxGraphicsRenderer::GetDefaultRenderer();
    }

    bool IsRendererName(const wxString& name) const
    {
        if (!m_renderer) return name.empty();
        return m_renderer->GetName() == name;
    }

    void PrepareDC(wxDC& dc)
    {
        dc.SetLogicalOrigin(m_xLogicalOrigin, m_yLogicalOrigin);
        dc.SetAxisOrientation(!m_xAxisReversed, m_yAxisReversed);
        dc.SetUserScale(m_xUserScale, m_yUserScale);
        dc.SetMapMode(m_mapMode ? m_mapMode : wxMM_TEXT);
    }
#endif // wxUSE_GRAPHICS_CONTEXT
    void UseBuffer(bool use)
    {
        m_useBuffer = use;
        Refresh();
    }
private:
    wxWindow* m_owner;

    int m_show;
    wxBitmap m_smile_bmp;
    wxIcon m_std_icon;
    bool m_clip;
    wxOverlay m_overlay;
    bool m_rubberBand;
    wxPoint m_anchorpoint;
    wxPoint m_currentpoint;
#if wxUSE_GRAPHICS_CONTEXT
    wxGraphicsRenderer* m_renderer;
#endif
    bool m_useBuffer;
    MyCanvas* m_canvas; //PARENT
    int m_backgroundMode;
    int m_textureBackground;
    wxMappingMode m_mapMode;
    double m_xUserScale;
    double m_yUserScale;
    int m_xLogicalOrigin;
    int m_yLogicalOrigin;
    bool m_xAxisReversed,
        m_yAxisReversed;
    wxColour m_colourForeground, // these are _text_ colours
        m_colourBackground;
    wxBrush m_backgroundBrush;

    wxMenuItem* m_menuItemUseDC;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT,
    Minimal_TestDialog,
    Minimal_StartSimplePopup,
    Minimal_StartScrolledPopup,
    Minimal_LogWindow,
    Minimal_PopupButton,
    Minimal_PopupSpinctrl
};
