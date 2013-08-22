#ifndef     __STARTPROGRESS_H__
#define     __STARTPROGRESS_H__

#include <semaphore.h>
#include "GUI/UIWindow.h"
#include "Framework/CDisplay.h"
#include "GUI/UIProgressBar.h"
#include "Common/LockedQueue.h"


class CStartProgress
{
private:
    struct ProgressData
    {
        int start;
        int end;
        int step;
    };
public:
    CStartProgress(CDisplay* display);
    ~CStartProgress();

    void  ShowStartPicture();
    void SetProgressData(int start, int end, int step)
    {
        ProgressData progressData = {start, end, step};
        m_progressData.Send(progressData);
    } 
    void  Run();
    int   ProgressExit();

private:
    static void* ProgressThread(void *lparam);
    void SetProgress(int cur);

private:
    dk::common::LockedQueue<ProgressData> m_progressData;
    UIProgressBar  m_bar;
    CDisplay* m_display;
    pthread_t m_tid;
    bool m_doExit;
	int x;
	int y;
	int w;
	int h;
};


#endif

