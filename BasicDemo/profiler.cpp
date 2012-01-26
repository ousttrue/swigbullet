#include "profiler.h"
#include "GLDebugFont.h"
#include "btBulletDynamicsCommon.h"
#ifdef _WINDOWS
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#endif //_WINDOWS


Profiler::Profiler()
{
#ifndef BT_NO_PROFILE
    m_profileIterator = CProfileManager::Get_Iterator();
#endif //BT_NO_PROFILE
}

Profiler::~Profiler()
{
#ifndef BT_NO_PROFILE
    CProfileManager::Release_Iterator(m_profileIterator);
#endif //BT_NO_PROFILE
}

void Profiler::keyboardCallback(unsigned char key, int x, int y)
{
#ifndef BT_NO_PROFILE
    if (key >= 0x31 && key <= 0x39)
    {
        int child = key-0x31;
        m_profileIterator->Enter_Child(child);
    }
    if (key==0x30)
    {
        m_profileIterator->Enter_Parent();
    }
#endif //BT_NO_PROFILE
}

static void displayProfileString(int xOffset,int yStart,char* message)
{
    glRasterPos3f(btScalar(xOffset),btScalar(yStart),btScalar(0));
    GLDebugDrawString(xOffset,yStart,message);
}


void Profiler::showProfileInfo(bool isIdle, int& xOffset,int& yStart, int yIncr)
{
#ifndef BT_NO_PROFILE
    static double time_since_reset = 0.f;
    if (!isIdle)
    {
        time_since_reset = CProfileManager::Get_Time_Since_Reset();
    }

    {
        //recompute profiling data, and store profile strings

        char blockTime[128];

        double totalTime = 0;

        int frames_since_reset = CProfileManager::Get_Frame_Count_Since_Reset();

        m_profileIterator->First();

        double parent_time = m_profileIterator->Is_Root() ? 
            time_since_reset : 
            m_profileIterator->Get_Current_Parent_Total_Time();

        {
            sprintf(blockTime,"--- Profiling: %s (total running time: %.3f ms) ---", 
                    m_profileIterator->Get_Current_Parent_Name(), parent_time );
            displayProfileString(xOffset,yStart,blockTime);
            yStart += yIncr;
            sprintf(blockTime,"press (1,2...) to display child timings, or 0 for parent" );
            displayProfileString(xOffset,yStart,blockTime);
            yStart += yIncr;

        }

        double accumulated_time = 0.f;
        for (int i = 0; !m_profileIterator->Is_Done(); m_profileIterator->Next())
        {
            double current_total_time = m_profileIterator->Get_Current_Total_Time();
            accumulated_time += current_total_time;
            double fraction = parent_time > SIMD_EPSILON ? (current_total_time / parent_time) * 100 : 0.f;

            sprintf(blockTime,"%d -- %s (%.2f %%) :: %.3f ms / frame (%d calls)",
                    ++i, m_profileIterator->Get_Current_Name(), fraction,
                    (current_total_time / (double)frames_since_reset),m_profileIterator->Get_Current_Total_Calls());
            displayProfileString(xOffset,yStart,blockTime);
            yStart += yIncr;
            totalTime += current_total_time;
        }

        sprintf(blockTime,"%s (%.3f %%) :: %.3f ms", "Unaccounted",
                // (min(0, time_since_reset - totalTime) / time_since_reset) * 100);
            parent_time > SIMD_EPSILON ? ((parent_time - accumulated_time) / parent_time) * 100 : 0.f, parent_time - accumulated_time);

        displayProfileString(xOffset,yStart,blockTime);
        yStart += yIncr;



        sprintf(blockTime,"-------------------------------------------------");
        displayProfileString(xOffset,yStart,blockTime);
        yStart += yIncr;

    }
#endif//BT_NO_PROFILE
}


void Profiler::render(bool isIdle, int debugMode)
{
    glDisable(GL_LIGHTING);
    glColor3f(0, 0, 0);
    int xOffset = 10;
    int yStart = 20;
    int yIncr = 20;
    if ((debugMode & btIDebugDraw::DBG_NoHelpText)==0) {
        showProfileInfo(isIdle, xOffset,yStart,yIncr);
#ifdef USE_QUICKPROF
        if ( debugMode & btIDebugDraw::DBG_ProfileTimings) {
            static int counter = 0;
            counter++;
            std::map<std::string, hidden::ProfileBlock*>::iterator iter;
            for (iter = btProfiler::mProfileBlocks.begin(); 
                    iter != btProfiler::mProfileBlocks.end(); 
                    ++iter)
            {
                char blockTime[128];
                sprintf(blockTime, "%s: %lf",
                        &((*iter).first[0]),
                        btProfiler::getBlockTime((*iter).first, 
                            btProfiler::BLOCK_CYCLE_SECONDS));//BLOCK_TOTAL_PERCENT));
                glRasterPos3f(xOffset,yStart,0);
                GLDebugDrawString(BMF_GetFont(BMF_kHelvetica10),blockTime);
                yStart += yIncr;
            }
        }
#endif //USE_QUICKPROF
    }
}

void Profiler::resize(int w, int h)
{
    GLDebugResetFont(w,h);
}

