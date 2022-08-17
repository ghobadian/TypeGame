#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include<dos.h>
#include<conio.h>
//.......................................................................................................................................................................
 COORD coord={0,0};
 void gotoxy(int x,int y);
void gotoxy(int x,int y)//function to set the cursor position(gotoxy())
{
    coord.X=x;
    coord.Y=y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}
//.......................................................................................................................................................................
void delay(unsigned int mseconds)//function for delay
{
    clock_t goal=mseconds+clock();
    while(goal>clock());
}
//.............................................................................................................................................................................
void setcolor(int ForgC) //function to change setcolor
 {
 WORD wColor;

  HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;

                       //We use csbi for the wAttributes word.
 if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
 {
                 //Mask out all but the background attribute, and add in the forgournd color
      wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
      SetConsoleTextAttribute(hStdOut, wColor);
 }
 return;
 }
void design()  //design for a frame
{
    int x=30,y=1;

    for(y=1;y<20;y=y+1)
    {   gotoxy(x,y);
        printf("*");
        x=x+60;
        gotoxy(x,y);
        printf("*");
        x=x-60;
    }
    gotoxy(x+60,y);
    printf("*");
    y=1;
    for(x=30;x<90;x++)
    {
         gotoxy(x,y);
        printf("*");
        y=y+19;
        gotoxy(x,y);
        printf("*");
        y=y-19;
    }
   for(x=30;x<90;x++)
   {
       y=4;
       gotoxy(x,y);
       printf("*");
   }
   gotoxy(48,20);

   printf("#Created by Shashi Kumar#");
}