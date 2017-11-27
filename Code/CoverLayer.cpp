#include <iostream>
#include <fstream>
#include "tpcspecs.h"
#include "board.h"

using namespace std;

ofstream outFile;


void FullCover()
{
  cout << "Creating an output file named: \"CoverLayerOutput.brd\" . . ." << endl;
  outFile.open("CoverLayerOutput.brd");
  streambuf* oldCout= cout.rdbuf();
  cout.rdbuf(outFile.rdbuf());

  InsertFile("/home/blake/TPC/Projects/Code/Pre-Code Junk.txt");

  MakeCoverSignals();

  cout.rdbuf(oldCout);
  outFile.close();
}



void InsertFile(const char *source)
{
  ifstream ifs(source,std::ifstream::in);
  char c = ifs.get();
  while (ifs.good())
    {
      cout << c;
      c = ifs.get();
    }
  ifs.close();
}




void MakeCoverSignals()
{
 cout << "<signals>" << endl;

 for (int i=0; i<TPC_Nresistors-4; i+=4)
   {
     MakeCoverStripes(i/4);
     cout<< "</signal>" << endl;
       }

  cout << "</signals>" << endl;
  cout << "</board>" << endl;
  cout << "</drawing>" << endl;
  cout << "</eagle>" << endl; 
}


//Origins
double x_origin_VCC = x0_brd, y_origin_VCC = y0_brd;
double x_origin_A = x_origin_VCC + dx_brd*2, y_origin_A = y_origin_VCC;
double x_origin_B = x_origin_VCC + dx_brd,   y_origin_B = y_origin_VCC;



void MakeCoverStripes(int i)
{
  double dx[12] = {0};
  double dy[12] = {0};
  int curve[12] = {0,1,0,0,1,0,0,1,0,0,1,0};

  dx[0]=      dx_coverTop;                // oo
  dy[1]=     -(dy_coverShort);            // oi
  dx[2]=      dx_coverShortWidth;              // io
  dy[3]=     -(dy_coverLong);             // oo
  dx[4]=     -(dx_coverShortWidth);            // oi
  dy[5]=     -(dy_coverShort);            // io
           //Start of going up
  dx[6]=     - (dx_coverTop);              // oo
  dy[7]=      dy_coverShort;              // oi
  dx[8]=     -(dx_coverShortWidth);            // io
  dy[9]=      dy_coverLong;               // oo
  dx[10]=    dx_coverShortWidth;              // oi
  dy[11]=     dy_coverShort;              // io 

  double x = x_origin_A + 0.5*PolygonWidth; //may need to adjust with one dx_coverShort
  double y = y_origin_A - 0.5*PolygonWidth;

  cout << "<signal name=\"PAD$A" << i << "\">" << endl;
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;

  cout << "<vertex x=\"" << x  << "\" y=\"" << y << "\"/>" << endl;
  for (int j=0; j<12; j++)
    {
      x += dx[j];
      y += dy[j];
      if (curve[j]==0)
	{
	  cout << "<vertex x=\"" << x << "\" y=\"" << y << "\"/>" << endl;
	}
      else {
	double cx1 = x; //- (dx[j])*(.1*InnerRadius);
	double cy1 = y + (.5*dx_coverShortWidth);//- (dy[j])*(.1*InnerRadius);
	double cx2 = x + (.5*dx_coverShortWidth);// (dx[j+1])*(.1*InnerRadius);
	double cy2 = y ;//+ (dy[j+1])*(.1*InnerRadius);
	  cout << "<vertex x=\"" << cx1  << "\" y=\"" << cy1  << "\" curve=\"90\" />" << endl;
	  cout << "<vertex x=\"" << cx2  << "\" y=\"" << cy2  << "\"/>" << endl;
      }
    }
  cout << "</polygon>" << endl;
  
  x_origin_A += 2*(dx_brd);
}


