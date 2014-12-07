#include "MouseHandling.h"
#include "Config.h"
#include "Defines.h"

MouseHandling::MouseHandling(Config* pConfig)
: m_nMouseX(SCREEN_WIDTH/2), m_nMouseY(SCREEN_HEIGHT/2), m_nMouseCenterX(-1), m_nMouseCenterY(-1), m_eMouseButton(NoButton), m_eCurrentButton(NoButton)
{
   UpdateConfig(pConfig);
}

void MouseHandling::UpdateConfig(Config* pConfig)
{
   m_ptad_info = /*pConfig->GetAllowMouse() ?*/ touchpad_getinfo() /*: NULL*/;
}

bool MouseHandling::PollMouse(int& nMX, int& nMY)
{
   bool bChanges = false;
   if( m_ptad_info )
   {
       touchpad_scan(&m_tpad_report);
       if( m_eMouseButton != NoButton ) {
           if( m_tpad_report.arrow == TPAD_ARROW_NONE ) {
               m_eCurrentButton = m_eMouseButton;
               m_eMouseButton = NoButton;
               bChanges = true;
            }
        }
        else if( m_tpad_report.arrow == TPAD_ARROW_CLICK ) {//Center button
            m_eMouseButton = CenterButton;
        }
        else if( m_tpad_report.arrow == TPAD_ARROW_LEFT ) {
            m_eMouseButton = LeftButton;
        }
        else if( m_tpad_report.arrow == TPAD_ARROW_RIGHT ) {
            m_eMouseButton = RightButton;
        }
        else if( m_tpad_report.arrow == TPAD_ARROW_UP ) {
            m_eMouseButton = UpButton;
        }
        else if( m_tpad_report.arrow == TPAD_ARROW_DOWN ) {
            m_eMouseButton = DownButton;
        }
        else if( m_tpad_report.proximity > 0 && !m_tpad_report.pressed )
        {
            int nX = m_tpad_report.x * SCREEN_WIDTH / 2328;
            int nY = (1691-m_tpad_report.y) * SCREEN_HEIGHT / 1691;
            if( m_nMouseCenterX == -1 && m_nMouseCenterY == -1 ) {
                m_nMouseCenterX = nX;
                m_nMouseCenterY = nY;
             }
             else {
                int nMouseX = (m_nMouseX + (nX-m_nMouseCenterX));
                int nMouseY = (m_nMouseY + (nY-m_nMouseCenterY));
                nMouseX = Puz_Min(SCREEN_WIDTH, Puz_Max(0, nMouseX));
                nMouseY = Puz_Min(SCREEN_HEIGHT, Puz_Max(0, nMouseY));
                nMX = m_nMouseX = nMouseX, nMY = m_nMouseY = nMouseY;
                m_nMouseCenterX = nX, m_nMouseCenterY = nY;
		
                bChanges = true;
             }
        }
        else
        {
             m_nMouseCenterX = m_nMouseCenterY = -1;
        }
    }
   return bChanges;
}

MouseButton MouseHandling::GetMouseButton()
{
   MouseButton eButton = m_eCurrentButton;
   m_eCurrentButton = NoButton;
   return eButton;
}


