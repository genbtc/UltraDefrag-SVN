//////////////////////////////////////////////////////////////////////////
//
//  UltraDefrag - a powerful defragmentation tool for Windows NT.
//  Copyright (c) 2007-2017 Dmitri Arkhangelski (dmitriar@gmail.com).
//  Copyright (c) 2010-2013 Stefan Pendl (stefanpe@users.sourceforge.net).
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////

/**
 * @file map.cpp
 * @brief Cluster map.
 * @details Even wxBufferedPaintDC
 * is too expensive and causes flicker
 * on map resize, so we're using low
 * level API here.
 * @addtogroup ClusterMap
 * @{
 */

// Ideas by Stefan Pendl <stefanpe@users.sourceforge.net>
// and Dmitri Arkhangelski <dmitriar@gmail.com>.

// The 'Flicker Free Drawing' article of James Brown
// helped us to reduce GUI flicker on window resize:
// http://www.catch22.net/tuts/flicker

// =======================================================================
//                            Declarations
// =======================================================================

#include "prec.h"
#include "main.h"
#pragma comment(lib, "gdi32")

// =======================================================================
//                            Cluster map
// =======================================================================

ClusterMap::ClusterMap(wxWindow* parent) : wxWindow(parent,wxID_ANY)
{
    HDC hdc = GetDC((HWND)GetHandle());
    m_cacheDC = ::CreateCompatibleDC(hdc);
    if(!m_cacheDC) letrace("cannot create cache dc");
    m_cacheBmp = ::CreateCompatibleBitmap(hdc,
        wxGetDisplaySize().GetWidth(),
        wxGetDisplaySize().GetHeight()
    );
    if(!m_cacheBmp) letrace("cannot create cache bitmap");
    ::SelectObject(m_cacheDC,m_cacheBmp);
    ::SetBkMode(m_cacheDC,TRANSPARENT);
    ::ReleaseDC((HWND)GetHandle(),hdc);

    for(int i = 0; i < SPACE_STATES; i++)
        m_brushes[i] = ::CreateSolidBrush(g_colors[i]);

    m_width = m_height = 0;
    m_legendPopup = NULL;
}

ClusterMap::~ClusterMap()
{
    ::DeleteDC(m_cacheDC);
    ::DeleteObject(m_cacheBmp);
    for(int i = 0; i < SPACE_STATES; i++)
        ::DeleteObject(m_brushes[i]);
}

// =======================================================================
//                           Event handlers
// =======================================================================

BEGIN_EVENT_TABLE(ClusterMap, wxWindow)
    EVT_ERASE_BACKGROUND(ClusterMap::OnEraseBackground)
    EVT_PAINT(ClusterMap::OnPaint)
    EVT_RIGHT_DOWN(ClusterMap::ClusterMapLegend)
END_EVENT_TABLE();

void ClusterMap::OnEraseBackground(wxEraseEvent& event)
{
    int width, height; GetClientSize(&width,&height);
    if(width > m_width || height > m_height){
        // expand free space to reduce flicker
        HDC hdc = GetDC((HWND)GetHandle());

        char free_r = (char)g_mainFrame->CheckOption(wxT("UD_FREE_COLOR_R"));
        char free_g = (char)g_mainFrame->CheckOption(wxT("UD_FREE_COLOR_G"));
        char free_b = (char)g_mainFrame->CheckOption(wxT("UD_FREE_COLOR_B"));
        HBRUSH brush = ::CreateSolidBrush(RGB(free_r,free_g,free_b));

        RECT rc; rc.left = m_width; rc.top = 0;
        rc.right = width; rc.bottom = height;
        ::FillRect(hdc,&rc,brush);

        rc.left = 0; rc.top = m_height;
        rc.right = width; rc.bottom = height;
        ::FillRect(hdc,&rc,brush);

        ::DeleteObject(brush);

        ::ReleaseDC((HWND)GetHandle(),hdc);
    }
    m_width = width; m_height = height;
}

/**
 * @brief Scales cluster map
 * of the current job to fit
 * inside of the map control.
 */
char *ClusterMap::ScaleMap(int scaled_size)
{
    JobsCacheEntry *currentJob = g_mainFrame->m_currentJob;
    int map_size = currentJob->pi.cluster_map_size;

    // dtrace("map size = %u, scaled size = %u",map_size,scaled_size);

    if(scaled_size == map_size)
        return nullptr; // no need to scale

    char *scaledMap = new char[scaled_size];

    int ratio = scaled_size / map_size;
    int used_cells = 0;
    if(ratio){
        // scale up
        for(int i = 0; i < map_size; i++){
            for(int j = 0; j < ratio; j++){
                scaledMap[used_cells] = currentJob->clusterMap[i];
                used_cells ++;
            }
        }
    } else {
        // scale down
        ratio = map_size / scaled_size;
        if(ratio * scaled_size != map_size)
            ratio ++; /* round up */

        used_cells = map_size / ratio;
        for(int i = 0; i < used_cells; i++){
            int states[SPACE_STATES];
            memset(states,0,sizeof(states));
            bool mft_detected = false;

            int sequence_length = (i < used_cells - 1) ? \
                ratio : map_size - i * ratio;

            for(int j = 0; j < sequence_length; j++){
                int index = (int)currentJob->clusterMap[i * ratio + j];
                if(index >= 0 && index < SPACE_STATES) states[index] ++;
                if(index == MFT_SPACE) mft_detected = true;
            }

            if(mft_detected){
                scaledMap[i] = MFT_SPACE;
            } else {
                // draw cell in dominating color
                int maximum = states[0]; scaledMap[i] = 0;
                for(int j = 1; j < SPACE_STATES; j++){
                    if(states[j] >= maximum){
                        maximum = states[j];
                        scaledMap[i] = (char)j;
                    }
                }
            }
        }
    }

    // mark unused cells
    for(int i = used_cells; i < scaled_size; i++)
        scaledMap[i] = UNUSED_MAP_SPACE;

    return scaledMap;
}

void ClusterMap::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    JobsCacheEntry *currentJob;
    
    cmapreturn *gs = new cmapreturn();
    GetGridSizeforCMap(gs);

    // fill map by the free color
    char free_r = (char)g_mainFrame->CheckOption(wxT("UD_FREE_COLOR_R"));
    char free_g = (char)g_mainFrame->CheckOption(wxT("UD_FREE_COLOR_G"));
    char free_b = (char)g_mainFrame->CheckOption(wxT("UD_FREE_COLOR_B"));
    HBRUSH brush = ::CreateSolidBrush(RGB(free_r,free_g,free_b));
    RECT rc; rc.left = rc.top = 0; rc.right = gs->width; rc.bottom = gs->height;
    ::FillRect(m_cacheDC,&rc,brush); ::DeleteObject(brush);
    if(!gs->blocks_per_line || !gs->lines) goto draw;

    // draw grid lines.
    if(gs->line_width){
        char grid_r = (char)g_mainFrame->CheckOption(wxT("UD_GRID_COLOR_R"));
        char grid_g = (char)g_mainFrame->CheckOption(wxT("UD_GRID_COLOR_G"));
        char grid_b = (char)g_mainFrame->CheckOption(wxT("UD_GRID_COLOR_B"));
        brush = ::CreateSolidBrush(RGB(grid_r,grid_g,grid_b));
        for(int i = 0; i < gs->blocks_per_line + 1; i++){
            RECT rc; rc.left = gs->cell_size * i; rc.top = 0;
            rc.right = rc.left + gs->line_width;
            rc.bottom = gs->cell_size * gs->lines + gs->line_width;
            ::FillRect(m_cacheDC,&rc,brush);
        }
        for(int i = 0; i < gs->lines + 1; i++){
            RECT rc; rc.left = 0; rc.top = gs->cell_size * i;
            rc.right = gs->cell_size * gs->blocks_per_line + gs->line_width;
            rc.bottom = rc.top + gs->line_width;
            ::FillRect(m_cacheDC,&rc,brush);
        }
        //Delete the brush:
        ::DeleteObject(brush);
    }

    // draw squares
    currentJob = g_mainFrame->m_currentJob;
    if(currentJob){
        if(currentJob->pi.cluster_map_size){
            int scaled_size = gs->blocks_per_line * gs->lines;
            char *scaledMap = ScaleMap(scaled_size);

            // draw either normal or scaled map
            char *map = scaledMap ? scaledMap : currentJob->clusterMap;
            for(int i = 0; i < gs->lines; i++){
                for(int j = 0; j < gs->blocks_per_line; j++){
                    RECT rc;
                    rc.top = gs->cell_size * i + gs->line_width;
                    rc.left = gs->cell_size * j + gs->line_width;
                    rc.right = rc.left + gs->block_size;
                    rc.bottom = rc.top + gs->block_size;
                    int index = (int)map[i * gs->blocks_per_line + j];
                    if(index != FREE_SPACE){
                        ::FillRect(m_cacheDC,&rc,m_brushes[index]);
                    }
                }
            }
            //delete the pre-scaled array
            delete [] scaledMap;
        }
    }

draw:
    // draw map on the screen
    PAINTSTRUCT ps;
    HDC hdc = ::BeginPaint((HWND)GetHandle(),&ps);
    ::BitBlt(hdc,0,0, gs->width, gs->height,m_cacheDC,0,0,SRCCOPY);
    ::EndPaint((HWND)GetHandle(),&ps);
    delete gs;
}


/**
* \brief Determine how large the Cluster Map should be (window size)
* \return A total number of cells/blocks.
*/
int MainFrame::GetMapSize() {
    int width, height; g_mainFrame->m_cMap->GetClientSize(&width, &height);
    int block_size = CheckOption("UD_MAP_BLOCK_SIZE");
    int line_width = CheckOption("UD_GRID_LINE_WIDTH");
    int cell_size = block_size + line_width;
    int blocks_per_line = (width - line_width) / cell_size;
    int lines = (height - line_width) / cell_size;
    return (blocks_per_line * lines);
}

// =======================================================================
// duplicated the above by accident. didnt see it (was in Job.cpp)
void ClusterMap::GetGridSizeforCMap(cmapreturn *gs)
{
    GetClientSize(&gs->width, &gs->height);
    gs->block_size = g_mainFrame->CheckOption(wxT("UD_MAP_BLOCK_SIZE"));
    gs->line_width = g_mainFrame->CheckOption(wxT("UD_GRID_LINE_WIDTH"));

    gs->cell_size = gs->block_size + gs->line_width;
    gs->blocks_per_line = gs->cell_size ? (gs->width - gs->line_width) / gs->cell_size : 0;
    gs->lines = gs->cell_size ? (gs->height - gs->line_width) / gs->cell_size : 0;
}


ULONGLONG ClusterMap::getLCNsfromMousePos(const wxPoint& pos)
{
    cmapreturn *gs = new cmapreturn();
    GetGridSizeforCMap(gs);

    double const x = pos.x;
    double const y = pos.y;

    //get current cell in full cell integers.
    int currentRow = ceil(y / gs->cell_size);
    int currentCol = ceil(x / gs->cell_size);
    //infer the X/Y matrix into a single cell.
    int rowsused = gs->blocks_per_line * (currentRow-1);
    int ActualCell = rowsused + currentCol;
    dtrace("Actual cell was %i", ActualCell);
    LONGLONG total_space;
    int bpc;    
    ULONGLONG clusters;
    int cmapCells;
    double LCNperCell;
    //We Cant do any of this if Map has not been generated by analysis yet...
    JobsCacheEntry *currentJob;
    currentJob = g_mainFrame->m_currentJob;
    if (currentJob) {
        total_space = g_mainFrame->m_volinfocache.total_space.QuadPart; //120038031360
        bpc = g_mainFrame->m_volinfocache.bytes_per_cluster;    //4096
        clusters = total_space / bpc;                   //29306160
        cmapCells = currentJob->pi.cluster_map_size;    //14508
        if (cmapCells) {
            LCNperCell = (double)clusters / cmapCells;  //2020
            int scaled_size = gs->blocks_per_line * gs->lines;
            
            //These should be equal. Make sure nothing weird happens while I debug this.
            assert(scaled_size == cmapCells);
            return (LCNperCell * ActualCell);
        }
    }
    delete gs;
    return 0;
}

void ClusterMap::DrawSingleRectangleBorder(HDC m_cacheDC2, int xblock, int yblock, 
    int line_width, int cell_size, HBRUSH brush, HBRUSH infill)
{
    int x = xblock * cell_size;
    int y = yblock * cell_size;
    int w, r;
    w = r = line_width;
    for (int q = 0; q <= 1; q++, r--)
    {
        RECT rc = { x + q * w,y + q * w,x + cell_size + w * r,y + cell_size + w * r };
        ::FillRect(m_cacheDC2, &rc, brush);
        brush = infill;
    }
}

void MainFrame::RedrawMap(wxCommandEvent& WXUNUSED(event))
{
    m_cMap->Refresh();
}


/** @} */
