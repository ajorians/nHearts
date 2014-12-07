#ifndef MOUSEHANDLING_H
#define MOUSEHANDLING_H

extern "C"
{
        #include <os.h>
}

class Config;

enum MouseButton
{
   NoButton,
   CenterButton,
   LeftButton,
   RightButton,
   UpButton,
   DownButton
};

class MouseHandling
{
public:
   MouseHandling(Config* pConfig);
   void UpdateConfig(Config* pConfig);
   bool PollMouse(int& nMX, int& nMY);
   MouseButton GetMouseButton();

protected:
   touchpad_info_t* m_ptad_info;
   touchpad_report_t m_tpad_report;
   int m_nMouseX, m_nMouseY;
   int m_nMouseCenterX, m_nMouseCenterY;
   MouseButton m_eMouseButton, m_eCurrentButton;
};

#endif

