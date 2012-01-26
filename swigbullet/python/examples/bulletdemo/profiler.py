from OpenGL.GL import *
import swigbullet as bullet
from .gldebugfont import  *


class Profiler(object):
    def __init__(self):
        self.m_profileIterator = bullet.CProfileManager.Get_Iterator();
        self.w=None
        self.h=None
        self.counter = 0;

    def __del__(self):
        bullet.CProfileManager.Release_Iterator(self.m_profileIterator);

    def keyboardCallback(self, key, x, y):
        if (key >= 0x31 and key <= 0x39):
            child = key-0x31;
            self.m_profileIterator.Enter_Child(child);
        if (key==0x30):
            self.m_profileIterator.Enter_Parent();

    def displayProfileString(self, xOffset, yStart, message):
        glRasterPos3f(xOffset, yStart, 0);
        GLDebugDrawString(xOffset, yStart, message);

    def showProfileInfo(self, isIdle, xOffset, yStart, yIncr):
        time_since_reset = 0.0;
        if not isIdle:
            time_since_reset = bullet.CProfileManager.Get_Time_Since_Reset();

            # recompute profiling data, and store profile strings
            totalTime = 0;

            frames_since_reset = bullet.CProfileManager.Get_Frame_Count_Since_Reset();

            self.m_profileIterator.First();

            parent_time = (self.m_profileIterator.Is_Root() 
                    and time_since_reset 
                    or self.m_profileIterator.Get_Current_Parent_Total_Time());

            blockTime="--- Profiling: %s (total running time: %.3f ms) ---" % (
                    self.m_profileIterator.Get_Current_Parent_Name(), 
                    parent_time);
            self.displayProfileString(xOffset, yStart, blockTime);
            yStart += yIncr;

            blockTime="press (1,2...) to display child timings, or 0 for parent"
            self.displayProfileString(xOffset, yStart, blockTime);
            yStart += yIncr;

            accumulated_time = 0.0;
            i=1
            while not self.m_profileIterator.Is_Done():
                current_total_time = self.m_profileIterator.Get_Current_Total_Time();
                accumulated_time += current_total_time;
                fraction = (parent_time > bullet.SIMD_EPSILON 
                        and (current_total_time / parent_time) * 100 
                        or 0.0);
                blockTime="%d -- %s (%.2f %%) :: %.3f ms / frame (%d calls)" % (
                        i, 
                        self.m_profileIterator.Get_Current_Name(), 
                        fraction,
                        (current_total_time / frames_since_reset),
                        self.m_profileIterator.Get_Current_Total_Calls()
                        );
                self.displayProfileString(xOffset, yStart, blockTime);
                yStart += yIncr;

                totalTime += current_total_time;
                # next
                i+=1
                self.m_profileIterator.Next()

            blockTime="%s (%.3f %%) :: %.3f ms" % (
                    "Unaccounted",
                    (parent_time > bullet.SIMD_EPSILON 
                        and ((parent_time - accumulated_time) / parent_time) * 100 
                        or 0.0), 
                    parent_time - accumulated_time);
            self.displayProfileString(xOffset,yStart,blockTime);
            yStart += yIncr;

            blockTime="-------------------------------------------------"
            self.displayProfileString(xOffset,yStart,blockTime);
            yStart += yIncr;

    def render(self, isIdle, debugMode, w, h):
        if self.w!=w or self.h!=h:
            GLDebugResetFont(w,h);
            self.w=w
            self.h=h
        glDisable(GL_LIGHTING);
        glColor3f(0, 0, 0);
        xOffset = 10;
        yStart = 20;
        yIncr = 20;
        if (debugMode & bullet.btIDebugDraw.DBG_NoHelpText)==0:
            self.showProfileInfo(isIdle, xOffset,yStart,yIncr);

