#ifndef OriConsoleH
#define OriConsoleH

#include "../OriUntil/OriThread.h"
#include "OriTime.h"

class   OriConsole;
typedef OriConsole LogMsg;

#if defined ORI_WINDOWS
class KernelApiConsole {

   public:
	   void ConsoleTitle(char *NameTitle);
	   void OriWriteConsole(int CoordX, int CoordY, char *Text);

       void HideCursor();
	   void ShowCursor();
	   void ShowCursor(int Size);

	   void PositionCursor(int &_X, int &_Y);
	  
};


class OriProgressConsoleBar {

   public:
	   OriProgressConsoleBar();
	   ~OriProgressConsoleBar();
	   
   public:    
	   int  MaxValue();
	   int  MaxValue(int _NewValue);
	   bool Iteration();
	   void Create();
	   void Create(const char *_Text);
	   void Text(const char *_Text);	   
	   void Stop();
	   
   public:     
	   int  SizeBar;
	   int  Style;
       int  _MaxValue;
	   int  Value;
	   int  PositionX;
	   int  PositionY;
	   
   private:
	   bool isStart;
	   KernelApiConsole ApiConsole;
	   ThreadObj;
};
#endif


enum OriConsoleColor {
	Black = 0, Blue = 1, Green = 2, Cyan = 3, Red = 4, Magenta = 5, Brown = 6,
	LightGray = 7, DarkGray = 8, LightBlue = 9, LightGreen = 10, LightCyan = 11,
	LightRed = 12, LightMagenta = 13, Yellow = 14, White = 15
};

enum OriConsoleType {
	TimeNow
};

class OriConsole
{
  public:

	OriConsole(OriTime * &_sLTime)   {	
		isSaveLog = false;
        this->LTime = _sLTime;		
   }


	OriConsole()   {
		isSaveLog = false;
	};

   void InitTime(OriTime * &_sLTime){  
	    this->LTime = _sLTime;	  
   }

   void InitTime(OriTime &_sLTime) {   
	     this->LTime =& _sLTime;		
   }

   void printf(char *_Str, ...);

   // Вывести сообщение в консоли
   void OriMsg(const char *msg1);
   // Вывести сообщение в консоли
   void OriLMsg(const char *msg1);
   void Msg(const char TypeMsg, const int color, char *msg, ...);

   OriConsole& operator << (const char *Str);

   OriBaseMutex LogMutex;
   OriTime   *LTime;
   std::ofstream LogFile;

#ifdef ORI_LINUX
   struct tm *gm;
   struct tm *tm;
   time_t t;
   char LocalTime[100];
#endif

   bool isSaveLog;
private:
	

};




//---------------------------------------------------------------------------
#endif

