#include <iostream>
#include <fstream>
#include "board.h"
#include "tpcspecs.h"

using namespace std;


/*RECENT PROGRESS: 

I've set it up so output for MakeSignals() is redirected to the file output.brd (if you want to change 
it back, just comment out the line cout.rdbuf(outFile.rdbuf()); at the beginning of MakeSignals())

I'm using a new system of globals for the origin coordinates of each pad. This way placing pad A only 
involves looking at and modifying x_origin_A and y_origin_A, without affecting the origin coordinates 
for other pads. This makes incrementing the position of pads easier, and debugging much easier. 

I adjusted the parameters given to the MakePad functions in the loop so the naming convention for pads 
goes like B0, A0, B1, A1, etc. (the D pads aren't labelled in the schematic so I left these as they were; 
note I'm also using their i values in MakePadD to determine the appropriate y_origin_D, since it alternates)

The origin vertices are missing or incorrect for pads A and B (they seem to have one vertex with the correct 
x_origin and another with the correct y_origin, but no vertex with the proper x_origin and y_origin). 

~Benito
*/


/*
MORE Progress:

Finished out the C and D pads including all the vias, front rectangle, and back stripe.

*  Worked briefly with the polygon width parameter and discovered how it works (bad news)
--The polygon is drawn with a "wide marker".
--The wide marker creates rounded outside corners and square inside corners.
--The coordinates of the vertices are the MARKER CENTER.
Since we REQUIRE that the outside corners be rounded (and that the thermals are drawn nicely) we absolutely must 
use a finite width marker.  However, we must now adjust the marker's path by 1/2 its width to get the edges where 
we want.
*/

/*
MORE progress:

*  design starting to look really really good.  However, it will benefity the detector ifwe add a redundant chain.  
Here is why in the long run:
1 CHAIN:
--If a resistor shorts, the field is a little distorted, but the detector still works.
--If a resistor opens the detector is dead until removal from the collision hall and repair.

2 CHAINS:
--If a resistor shorts, the field distortion is 1/2 as bad as with one chain.
--If a resistor opens, the detector continues to function but with a slightly distorted field.

This is TOO IMPORTANT TO SKIP!!  After thinking about it, I figured out that having one chain at each end of the 
circuit card is harder to program and looks electrically identical to having both chains at the same end.  For 
this reason, we will be creating a SECOND COPY of the chain displaced vertically downward and thereby create the 
reduncancy we desire.

2017-04-24 Vlad: 
There's a difference between setting the thermals to 0 and actively turning them off. On at 0 still gives a tiny 
thermal so I added a "thermals = \"no\"" to every polygon on the top (layer 1).
*/

ofstream outFile; //output file 

//double x_origin = x0_brd; 
//double y_origin = y0_brd;

//using these instead of a general x_origin and y_origin will be easier and less error-prone
double x_origin_VCC = x0_brd, y_origin_VCC = y0_brd;
double x_origin_A = x_origin_VCC + dx_brd*2, y_origin_A = y_origin_VCC;
double x_origin_B = x_origin_VCC + dx_brd,   y_origin_B = y_origin_VCC;
double x_origin_C = x_origin_VCC + dx_CEE,   y_origin_C = y_origin_VCC - dy_CEE;
double x_origin_D = x_origin_VCC + dx_DEE,   y_origin_D = y_origin_VCC - dy_DEE;

void FullBoard()
{
  cout << "Redirecting cout to file output.brd ..." << endl;
  outFile.open("output.brd"); //open output file
  streambuf* oldCout = cout.rdbuf(); //save original cout stream
  cout.rdbuf(outFile.rdbuf()); //redirect cout to outFile

  InsertFile("/home/tpc/softwarerepo/eagle/R-Test/FirstString.brd");
  MakePlain();
  InsertFile("/home/tpc/softwarerepo/eagle/R-Test/SecondString.brd");

  MakeElements();
  MakeSignals();

  cout.rdbuf(oldCout); //restore cout to normal
  cout << "Verifying that cout is restored to normal" << endl;
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


void MakePlain() {
  /*
    <plain>
    <wire x1="0"   y1="0"   x2="160" y2="0"   width="0" layer="20"/>
    <wire x1="160" y1="0"   x2="160" y2="100" width="0" layer="20"/>
    <wire x1="160" y1="100" x2="0"   y2="100" width="0" layer="20"/>
    <wire x1="0"   y1="100" x2="0"   y2="0"   width="0" layer="20"/>
    <rectangle x1="-210.15" y1="-677.35" x2="210.25" y2="-666.85" layer="29"/>
    <rectangle x1="-210.25" y1="-652.4"  x2="210.25" y2="-647.6"  layer="29"/>
    <rectangle x1="-210.25" y1="-633.85" x2="210.25" y2="-628.8"  layer="29"/>
    <rectangle x1="-210.15" y1="-615"    x2="210.2"  y2="-610.1"  layer="29"/>
    <rectangle x1="-210.1"  y1="-596.2"  x2="210.25" y2="677.4"   layer="29"/>
    </plain>
   */

  cout << "<plain>" << endl;
  cout << "<wire x1=\"" << BOARD_RIGHT_X - (BOARD_LEFT_X ) << "\" y1=\"" << BOARD_TOP_Y - (BOARD_BOT_Y) << "\" x2=\"" << BOARD_RIGHT_X - (BOARD_RIGHT_X) 
                                                           << "\" y2=\"" << BOARD_TOP_Y - (BOARD_BOT_Y) << "\" width=\"0\" layer=\"20\"/>" << endl;
  cout << "<wire x1=\"" << BOARD_RIGHT_X - (BOARD_RIGHT_X) << "\" y1=\"" << BOARD_TOP_Y - (BOARD_BOT_Y) << "\" x2=\"" << BOARD_RIGHT_X - (BOARD_RIGHT_X) 
                                                           << "\" y2=\"" << BOARD_TOP_Y - (BOARD_TOP_Y) << "\" width=\"0\" layer=\"20\"/>" << endl;
  cout << "<wire x1=\"" << BOARD_RIGHT_X - (BOARD_RIGHT_X) << "\" y1=\"" << BOARD_TOP_Y - (BOARD_TOP_Y) << "\" x2=\"" << BOARD_RIGHT_X - (BOARD_LEFT_X ) 
                                                           << "\" y2=\"" << BOARD_TOP_Y - (BOARD_TOP_Y) << "\" width=\"0\" layer=\"20\"/>" << endl;
  cout << "<wire x1=\"" << BOARD_RIGHT_X - (BOARD_LEFT_X ) << "\" y1=\"" << BOARD_TOP_Y - (BOARD_TOP_Y) << "\" x2=\"" << BOARD_RIGHT_X - (BOARD_LEFT_X ) 
                                                           << "\" y2=\"" << BOARD_TOP_Y - (BOARD_BOT_Y) << "\" width=\"0\" layer=\"20\"/>" << endl;

  double x1 = BOARD_LEFT_X;
  double x2 = BOARD_RIGHT_X;
  double y1 = BOARD_BOT_Y;
  double y2 = dx_gap + dy_space;
  cout << "<rectangle x1=\"" <<x1<< "\" y1=\"" <<y1<< "\" x2=\"" <<x2<< "\" y2=\"" <<y2<< "\" layer=\"29\"/>" << endl;
  
  y1 = y2 + dy_inside + dy_footprint + dy_gap;
  y2 = y1 + dy_sep;
  cout << "<rectangle x1=\"" <<x1<< "\" y1=\"" <<y1<< "\" x2=\"" <<x2<< "\" y2=\"" <<y2<< "\" layer=\"29\"/>" << endl;

  y1 = y2 + dy_inside + dy_footprint + dy_gap;
  y2 = y1 + dy_sep;
  cout << "<rectangle x1=\"" <<x1<< "\" y1=\"" <<y1<< "\" x2=\"" <<x2<< "\" y2=\"" <<y2<< "\" layer=\"29\"/>" << endl;

  y1 = y2 + dy_inside + dy_footprint + dy_gap;
  y2 = y1 + dy_sep;
  cout << "<rectangle x1=\"" <<x1<< "\" y1=\"" <<y1<< "\" x2=\"" <<x2<< "\" y2=\"" <<y2<< "\" layer=\"29\"/>" << endl;

  y1 = y2 + dy_inside + dy_footprint + dy_gap;
  y2 = BOARD_TOP_Y;
  cout << "<rectangle x1=\"" <<x1<< "\" y1=\"" <<y1<< "\" x2=\"" <<x2<< "\" y2=\"" <<y2<< "\" layer=\"29\"/>" << endl;

  cout << "</plain>" << endl;
}


void MakeElements()
{
  double y[4];
  y[0] = y_origin_C + 0.5*dy_footgap;
  y[1] = y_origin_C - dy_pad - 0.5*dy_footgap;
  y[2] = y_origin_D + 0.5*dy_footgap;
  y[3] = y_origin_D - dy_pad - 0.5*dy_footgap;

  cout << "<elements>" << endl;
  for(int count = 0; count < TPC_Nresistors; count++) 
    {
      double x = x_origin_C + 0.5*dx_footprint + (count/2)*dx_brd;

      cout << "<element name=\"R" << count+1 << "\" library=\"resistor\" package=\"M1206\" value=\"4M\" x=\"" 
	   << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y[count%4] ) << "\" rot=\"R90\">" << endl;
      cout << "<attribute name=\"MF\" value=\"\" x=\"" 
	   << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y[count%4] ) 
	   << "\" size=\"1.778\" layer=\"27\" rot=\"R90\" display=\"off\"/>" << endl;
      cout << "<attribute name=\"MPN\" value=\"HVC1206-4M0JT3\" x=\"" 
	   << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y[count%4] ) 
	   << "\" size=\"1.778\" layer=\"27\" rot=\"R90\" display=\"off\"/>" << endl;
      cout << "<attribute name=\"OC_NEWARK\" value=\"66X4681\" x=\"" 
	   << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y[count%4] ) 
	   << "\" size=\"1.778\" layer=\"27\" rot=\"R90\" display=\"off\"/>" << endl;
      cout << "</element>" << endl;

      cout << "<element name=\"R" << count+2001 << "\" library=\"resistor\" package=\"M1206\" value=\"4M\" x=\"" 
	   << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y[count%4]-dy_delta ) << "\" rot=\"R90\">" << endl;
      cout << "<attribute name=\"MF\" value=\"\" x=\"" 
	   << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y[count%4]-dy_delta ) 
	   << "\" size=\"1.778\" layer=\"27\" rot=\"R90\" display=\"off\"/>" << endl;
      cout << "<attribute name=\"MPN\" value=\"HVC1206-4M0JT3\" x=\"" 
	   << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y[count%4]-dy_delta ) 
	   << "\" size=\"1.778\" layer=\"27\" rot=\"R90\" display=\"off\"/>" << endl;
      cout << "<attribute name=\"OC_NEWARK\" value=\"66X4681\" x=\"" 
	   << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y[count%4]-dy_delta ) 
	   << "\" size=\"1.778\" layer=\"27\" rot=\"R90\" display=\"off\"/>" << endl;
      cout << "</element>" << endl;    
    }

  cout << "</elements>" << endl;
  /*<elements>
<element name="R1" library="resistor" package="M1206" value="4M" x="3.05" y="143.88" rot="R90">
<attribute name="MF" value="" x="3.05" y="143.88" size="1.778" layer="27" rot="R90" display="off"/>
<attribute name="MPN" value="HVC1206-4M0JT3" x="3.05" y="143.88" size="1.778" layer="27" rot="R90" display="off"/>
<attribute name="OC_NEWARK" value="66X4681" x="3.05" y="143.88" size="1.778" layer="27" rot="R90" display="off"/>
</element>
  */
}


void MakeSignals()
{
  cout << "<signals>" << endl;

  MakePre();
  MakePrePads();
  MakeVCC();
  for (int i=0; i<TPC_Nresistors-4; i+=4)
    {
      MakePadC(i/4);
      MakePadB(i/4);
      MakePadD(i/4);
      MakePadA(i/4);
    }
  MakePadC((TPC_Nresistors-2)/4);
  MakeGND();
  MakePost();
  MakePostPads();
  MakeCutouts();

  cout << "</signals>" << endl;
  cout << "</board>" << endl;
  cout << "</drawing>" << endl;
  cout << "</eagle>" << endl;
}


/*
For All Polygons:
To account for the width of the line that draws the polygon outline 
we must follow these sets of rules:
"out" means "outside-corner" &&  "in" means "inside-corner"

out-out -->  decrease delta by PolygonWidth
in-out  -->  unchanged
out-in  -->  unchanged
in-in   -->  increase delta by PolygonWidth
*/

void MakePre()
{
  // x_origin += dx_biggap;
  // Pad D is the pad contianing the vias, only 4 sides
  
  double dx[16] = {0};
  double dy[16] = {0};
  //int curve[16] = {0};
  //curve[16] at [2,3,6,8,9,12]
  int curve[16] = {0,0,1,1,0,0,1,0,1,1,0,0,1,0,0,0};
  
  dx[0] =  (dx_medgap - dx_rgap - PolygonWidth); // oo
  dy[1] = -(dy_pad - PolygonWidth);              // oo
  dx[2] =  -dx[0];                               // oo
  dy[3] =  -dy[1];                               // oo

  double x = x_origin_VCC + 0.5*PolygonWidth;
  double y = y_origin_D   - 0.5*PolygonWidth;

  cout << "<signal name=\"PREPAD\">" << endl;

  //  Make the front polygon #1
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;

  //  Make front Polygon #2
  x = x_origin_VCC + 0.5*PolygonWidth;
  y = y_origin_D - (dy_delta) - 0.5*PolygonWidth;  
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;


  dx[0] =  (0.5*dx_backnormal -0.5*dx_gap - EndPolygonWidth);                      // oo
  dy[1] = -(dy_space - 2.0*dy_gap - EndPolygonWidth);                              // oo
  dx[2] = -(dx_backnormal - dx_backnarrow)*0.5;                                // oi
  dy[3] = -(3.0*dy_gap + dy_footprint + dy_inside + 2.0*dy_gap + EndPolygonWidth); // ii
  dx[4] =  (dx_backwide - dx_backnarrow)*0.5;                                  // io
  dy[5] = -(dy_sep - 2.0*dy_gap + dy_inside + dy_footprint + dy_gap + dy_gap - EndPolygonWidth); // oo
  dx[6] = -(dx_backwide - dx_backnormal)*0.5;                                  // oi

  //  Now jump and repeat the preceeding pattern
  dy[7]  = -dy_delta - dy[3] - dy[5]; // io
  dx[8]  =  dx[2]; // oi
  dy[9]  =  dy[3]; // ii
  dx[10] =  dx[4]; // io
  dy[11] =  dy[5]; // oo
  dx[12] =  dx[6]; // oi

  dy[13] = -(dy_excess - dy_gap);                            // io
  dx[14] = -(0.5*dx_backnormal - 0.5*dx_gap - EndPolygonWidth);    // oo
  dy[15] = -(dy[1]+dy[3]+dy[5]+dy[7]+dy[9]+dy[11]+dy[13]);

  x = x_origin_VCC + 0.5*EndPolygonWidth;
  y = y_origin_A - 0.5*EndPolygonWidth;  //  =top of back traces aligns with "A" shape.

  cout << "<polygon width=\"" << EndPolygonWidth << "\" layer=\"16\">" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<16; j++)
    {
      x += dx[j];
      y += dy[j];
      if (curve[j]==0)
	{
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      else {
	  double cx1 = x - Sign(dx[j])*InnerRadius;
	  double cy1 = y - Sign(dy[j])*InnerRadius;
	  double cx2 = x + Sign(dx[j+1])*InnerRadius;
	  double cy2 = y + Sign(dy[j+1])*InnerRadius;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx1 ) << "\" y=\"" << BOARD_TOP_Y - ( cy1 ) << "\" curve=\"90\" />" << endl;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx2 ) << "\" y=\"" << BOARD_TOP_Y - ( cy2 ) << "\"/>" << endl;
	}
    }
  cout << "</polygon>" << endl;

  double viax = x_origin_VCC+(0.5*(dx_medgap-dx_rgap));
  double viay = y_origin_D-(dy_pad/2);
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) <<"\" extent=\"1-16\" drill=\"" << SmDrillSize << "\" shape=\"square\"/>" << endl;
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) <<"\" extent=\"1-16\" drill=\"" << SmDrillSize << "\" shape=\"square\"/>" << endl;

  viax = x_origin_VCC+(0.5*(dx_medgap-dx_rgap));
  viay = y_origin_D - dy_delta - (dy_pad/2);
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) <<"\" extent=\"1-16\" drill=\"" << SmDrillSize << "\" shape=\"square\"/>" << endl;
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) <<"\" extent=\"1-16\" drill=\"" << SmDrillSize << "\" shape=\"square\"/>" << endl;

  cout << "</signal>" << endl;
}


void MakePrePads()
{
  //x_origin += dx_biggap;
  // Pad D is the pad contianing the vias, only 4 sides
  
  double dx[16] = {0};
  double dy[16] = {0};

  dx[0]=  (dx_medgap - dx_rgap - PolygonWidth); // oo
  dy[1]= -(dy_footprint - PolygonWidth);        // oo
  dx[2]=  -dx[0];                               // oo
  dy[3]=  -dy[1];                               // oo

  double x = x_origin_VCC + 0.5*PolygonWidth;
  double y = y_origin_D + dy_footgap + dy_footprint  - 0.5*PolygonWidth;

  cout << "<signal name=\"BILL\">" << endl;
  //  Make the front polygon #1
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;
  cout << "</signal>" << endl;

  //  Make front Polygon #2
  cout << "<signal name=\"TED\">" << endl;
  x = x_origin_VCC + 0.5*PolygonWidth;
  y = y_origin_D + dy_footgap + dy_footprint - (dy_delta) - 0.5*PolygonWidth;  
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;
  cout << "</signal>" << endl;

}

void MakeVCC()
{
  //set up first y-origin for first D pad
  
  double dx[24] = {0};
  double dy[24] = {0};
  //int curve[24] = {0};
  //curve at [1,4,5,7,10,11,16,17,20,21]
  int curve[24] = {0,1,0,0,1,1,0,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1};

  //  The first indentation is located here:
  dx[0]=      (dx_wid - PolygonWidth);                 // oo
  dy[1]=     -(dy_space+dy_gap);                 // oi
  dx[2]=  0.5*(dx_gap+dx_footprint);             // io
  dy[3]=     -(dy_footprint - PolygonWidth);           // oo
  dx[4]=     -(dx_long);                         // oi
  dy[5]=     -(dy_inside + PolygonWidth);              // ii
  dx[6]=      (dx_medgap);                       // io

  //  Jump to the second indentation:
  dy[7]=     -(dy_delta - dy_footprint - dy_inside); // oi

  //  Copy the prior pattern here:
  dx[8]=      dx[2];  // io
  dy[9]=      dy[3];  // oo
  dx[10]=     dx[4];  // oi
  dy[11]=     dy[5];  // ii
  dx[12]=     dx[6];  // io

  //  Continue the prior deltas like before... (these are the lower copy, but that is OK)
  dy[13]=    -(dy_sep + dy_gap + dy_footprint + dy_inside + dy_excess - PolygonWidth); // oo
  dx[14]=    -(dx_wid - PolygonWidth);                                                 // oo
  dy[15]=     (dy_excess + dy_gap + dy_footprint - PolygonWidth);                      // oo
  dx[16]=     (dx_medgap);                                              // oi
  dy[17]=     (dy_inside + PolygonWidth);                                              // ii 
  dx[18]=    -(dx_medgap);                                              // io

  //  Now jump up to the next pattern by reversing "extra delta"
  dy[19]=     (dy_delta - dy_inside - PolygonWidth);  // oo

  //  Now finish the thing out by repeating the indentation pattern.
  dx[20]=    dx[16];  //  oi
  dy[21]=    dy[17];  //  ii
  dx[22]=    dx[18];  //  io
  dy[23]=   (dy_sep + dy_inside + dy_footprint + dy_gap + dy_space - PolygonWidth);  // oo

  double x = x_origin_VCC + 0.5*PolygonWidth;
  double y = y_origin_VCC - 0.5*PolygonWidth;
  
  cout << "<signal name=\"VCC\">" << endl;
  cout << "<contactref element=\"R1\" pad=\"1\"/>" << endl;
  cout << "<contactref element=\"R2001\" pad=\"1\"/>" << endl;
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;

  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<24; j++)
    {
      x += dx[j];
      y += dy[j];
      if (curve[j]==0)
	{
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      else {
	  double cx1 = x - Sign(dx[j])*InnerRadius;
	  double cy1 = y - Sign(dy[j])*InnerRadius;
	  double cx2 = x + Sign(dx[j+1])*InnerRadius;
	  double cy2 = y + Sign(dy[j+1])*InnerRadius;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx1 ) << "\" y=\"" << BOARD_TOP_Y - ( cy1 ) << "\" curve=\"90\" />" << endl;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx2 ) << "\" y=\"" << BOARD_TOP_Y - ( cy2 ) << "\"/>" << endl;
	}
    }
  cout << "</polygon>" << endl;
  cout << "</signal>" << endl;
  
}

void MakePadA(int i)
{
  // pad A has the top resistor on the right side
  
  double dx[28] = {0};
  double dy[28] = {0};
  //int curve[28] = {0};
  //curve at [1,4,5,7,10,11,15,18,19,21,24,25]
  int curve[28] = {0,1,0,0,1,1,0,1,0,0,1,0,0,0,0,1,0,0,1,1,0,1,0,0,1,1};

  dx[0]=      (dx_wid - PolygonWidth);                 // oo
  dy[1]=     -(dy_space+dy_gap);                 // oi
  dx[2]=  0.5*(dx_gap+dx_footprint);             // io
  dy[3]=     -(dy_footprint - PolygonWidth);           // oo
  dx[4]=     -(dx_long);                         // oi
  dy[5]=     -(dy_inside + PolygonWidth);              // ii
  dx[6]=      (dx_medgap);                       // io

  //  Jump to the second indentation:
  dy[7]=     -(dy_delta - dy_footprint - dy_inside); // oi

  //  Copy the prior pattern here:
  dx[8]=      dx[2];  // io
  dy[9]=      dy[3];  // oo
  dx[10]=     dx[4];  // oi
  dy[11]=     dy[5];  // ii
  dx[12]=     dx[6];  // io

  //  Continue the prior deltas like before... (these are the lower copy, but that is OK)
  dy[13]=     -(dy_sep + dy_inside + dy_footprint + dy_gap + dy_excess - PolygonWidth); // oo
  dx[14]=     -(dx_wid - PolygonWidth);                                                 // oo
  dy[15]=      (dy_excess + dy_gap);                                     // oi
  dx[16]= -0.5*(dx_gap+dx_footprint);                                    // io
  dy[17]=      (dy_footprint - PolygonWidth);                                           // oo 
  dx[18]=      (dx_long);                                                // oi
  dy[19]=      (dy_inside + PolygonWidth);                                              // ii
  dx[20]=     -(dx_medgap);                                              // io 

  //  Now jump up to the next pattern by reversing "extra delta"
  dy[21]=     (dy_delta - dy_footprint - dy_inside);  // oi

  dx[22]=     dx[16];  // io
  dy[23]=     dy[17];  // oo
  dx[24]=     dx[18];  // oi
  dy[25]=     dy[19];  // ii
  dx[26]=     dx[20];  // io
  dy[27]=     (dy_sep + dy_inside + dy_footprint + dy_gap + dy_space - PolygonWidth); //oo

  double x = x_origin_A + 0.5*PolygonWidth;
  double y = y_origin_A - 0.5*PolygonWidth;
  
  cout << "<signal name=\"PAD$A" << i << "\">" << endl;
  cout << "<contactref element=\"R" << i*4+4 << "\" pad=\"" << 2<< "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+5 << "\" pad=\"" << 1<< "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+4+2000 << "\" pad=\"" << 2<< "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+5+2000 << "\" pad=\"" << 1<< "\"/>" << endl;
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;

  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<28; j++)
    {
      x += dx[j];
      y += dy[j];
      if (curve[j]==0)
	{
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      else {
	  double cx1 = x - Sign(dx[j])*InnerRadius;
	  double cy1 = y - Sign(dy[j])*InnerRadius;
	  double cx2 = x + Sign(dx[j+1])*InnerRadius;
	  double cy2 = y + Sign(dy[j+1])*InnerRadius;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx1 ) << "\" y=\"" << BOARD_TOP_Y - ( cy1 ) << "\" curve=\"90\" />" << endl;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx2 ) << "\" y=\"" << BOARD_TOP_Y - ( cy2 ) << "\"/>" << endl;
	}
    }
  cout << "</polygon>" << endl;
  //y_origin -= dy_respad;//sets up y cord for the next d pad

  if (i == MagicA)
    {
      dx[0] =  dx_magicpad - PolygonWidth; // oo
      dy[1] = -(dy_magicpad - PolygonWidth);  // oo
      dx[2] = -dx[0]; // oo
      dy[3] = -dy[1]; // oo

      x = x_origin_A + 0.5*dx_wid - 0.5*dx_magicpad + 0.5*PolygonWidth;
      y = y_origin_A - dy_space - 2.0*dy_delta - dy_gap - 0.5*PolygonWidth;

      cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"16\">" << endl;
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
      for (int j=0; j<4; j++)
	{
	  x += dx[j];
	  y += dy[j];
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;     
	}
      cout << "</polygon>" << endl;      

      double viax = x_origin_A+(0.5*dx_wid);
      double viay = y_origin_A - dy_space - 2.0*dy_delta - dy_gap - 0.5*dy_magicpad;
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-2 )<< "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+2 )<< "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
    }

  cout << "</signal>" << endl;
  
  x_origin_A += 2*dx_brd;
}


void MakePadB(int i)
{
  
  //  x_origin += dx_brd-dx_biggap;
  //  y_origin = y0_brd;
  
  // pad B has the top resistor on the left side
  
  double dx[28] = {0};
  double dy[28] = {0};
  //int curve[28] = {0};
  //curve at [1,4,5,7,10,11,15,18,19,21,24,25]
  int curve[28] = {0,1,0,0,1,1,0,1,0,0,1,1,0,0,0,1,0,0,1,1,0,1,0,0,1,1};
  
  //  Here is the original code for the first resistor pair...
  dx[0]=     (dx_wid - PolygonWidth);                                                  // oo
  dy[1]=    -(dy_space + dy_inside + dy_footprint + dy_gap + dy_sep + dy_gap);  // oi
  dx[2]= 0.5*(dx_gap+dx_footprint);                                             // io
  dy[3]=    -(dy_footprint - PolygonWidth);                                            // oo
  dx[4]=    -(dx_long);                                                         // oi
  dy[5]=    -(dy_inside + PolygonWidth);                                               // ii
  dx[6]=     (dx_medgap);                                                       // io
  
  //  Now we must jump the gap to get to the second resistor pair...
  dy[7]=    -(dy_delta - dy_footprint - dy_inside);  //oi

  //  Now we shall repeat the prior pattern...
  dx[8]=     dx[2];  // io
  dy[9]=     dy[3];  // oo
  dx[10]=    dx[4];  // oi
  dy[11]=    dy[5];  // ii
  dx[12]=    dx[6];  // io

  //  Now we reuse the original code to make the bottom pattern...
  dy[13]=   -(dy_excess - PolygonWidth);                                                     // oo
  dx[14]=   -(dx_wid - PolygonWidth);                                                        // oo
  dy[15]=    (dy_excess + dy_gap + dy_footprint + dy_inside + dy_sep + dy_gap); // oi
  dx[16]= -0.5*(dx_gap+dx_footprint);                                           // io
  dy[17]=    (dy_footprint - PolygonWidth);                                                  // oo
  dx[18]=    (dx_long);                                                         // oi
  dy[19]=    (dy_inside + PolygonWidth);                                                     // ii
  dx[20]=   -(dx_medgap);                                                       // io

  //  Now we must jump up and then repeat the pattern
  dy[21]=   (dy_delta - dy_footprint - dy_inside);// oi
  dx[22]=    dx[16];  // io
  dy[23]=    dy[17];  // oo
  dx[24]=    dx[18];  // oi
  dy[25]=    dy[19];  // ii
  dx[26]=    dx[20];  // io
  dy[27]=   (dy_space - PolygonWidth);   // oo
  
  double x = x_origin_B + 0.5*PolygonWidth;
  double y = y_origin_B - 0.5*PolygonWidth;
  
  cout << "<signal name=\"PAD$B" << i << "\">" << endl;
  cout << "<contactref element=\"R" << i*4+2 << "\" pad=\"" << 2<< "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+3 << "\" pad=\"" << 1<< "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+2+2000 << "\" pad=\"" << 2<< "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+3+2000 << "\" pad=\"" << 1<< "\"/>" << endl;
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<28; j++)
    {
      x += dx[j];
      y += dy[j];
      if (curve[j]==0)
	{
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      else {
	  double cx1 = x - Sign(dx[j])*InnerRadius;
	  double cy1 = y - Sign(dy[j])*InnerRadius;
	  double cx2 = x + Sign(dx[j+1])*InnerRadius;
	  double cy2 = y + Sign(dy[j+1])*InnerRadius;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx1 ) << "\" y=\"" << BOARD_TOP_Y - ( cy1 ) << "\" curve=\"90\" />" << endl;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx2 ) << "\" y=\"" << BOARD_TOP_Y - ( cy2 ) << "\"/>" << endl;
	}
    }
  cout << "</polygon>" << endl;
  cout << "</signal>" << endl;
  
  //y_origin -= dy_long+dy_footgap;//setting up the y cord for the next dpad
  
  x_origin_B += 2*dx_brd;
}


void MakePadC(int i)
{
  if(i != 37)
    {
      //x_origin=999; need origin for back pad 
      //y_origin is needed for padC so it doesnt mess up the origin of dpad
      
      double dx[28] = {0};
      double dy[28] = {0};
      //int curve[28] = {0};
      //curve at[1,4,5,7,10,11,16,17,20,22,23,26]
      int curve[28] = {0,1,0,0,1,1,0,1,0,0,1,1,0,0,0,0,1,1,0,0,1,0,1,1,0,0,1};
      
      dx[0]=  (dx_footprint - PolygonWidth); // oo
      dy[1]= -(dy_pad - PolygonWidth);       // oo
      dx[2]= -(dx_footprint - PolygonWidth); // oo
      dy[3]=  (dy_pad - PolygonWidth);       // oo
      
      double x = x_origin_C + 0.5*PolygonWidth;
      double y = y_origin_C - 0.5*PolygonWidth;
      
      cout << "<signal name=\"PAD$C" << i << "\">" << endl;
      cout << "<contactref element=\"R" << i*4+1 << "\" pad=\"" << 2 << "\"/>" << endl;
      cout << "<contactref element=\"R" << i*4+2 << "\" pad=\"" << 1 << "\"/>" << endl;
      cout << "<contactref element=\"R" << i*4+1+2000 << "\" pad=\"" << 2 << "\"/>" << endl;
      cout << "<contactref element=\"R" << i*4+2+2000 << "\" pad=\"" << 1 << "\"/>" << endl;
      
      //  Make the front polygon #1
      cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
      for (int j=0; j<4; j++)
	{
	  x += dx[j];
	  y += dy[j];
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      cout << "</polygon>" << endl;
      
      //  Make Front Polygon #2
      x = x_origin_C + 0.5*PolygonWidth;
      y = y_origin_C - (dy_delta) - 0.5*PolygonWidth;
      cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
      for (int j=0; j<4; j++)
	{
	  x += dx[j];
	  y += dy[j];
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      cout << "</polygon>" << endl;
      
      // Make the back polygon     
      dx[0] =      (dx_backnormal - PolygonWidth);                                  // oo
      dy[1] =     -(dy_space - dy_gap);                                       // oi
      dx[2] =  0.5*(dx_backwide - dx_backnormal);                             // io
      dy[3] =     -(3.0*dy_gap + dy_footprint + dy_inside - PolygonWidth);          // oo
      dx[4] = -0.5*(dx_backwide - dx_backnarrow);                             // oi
      dy[5] =     -(dy_sep - dy_gap + dy_inside + dy_footprint + dy_gap + dy_gap + dy_gap + PolygonWidth);  // ii
      dx[6] =  0.5*(dx_backnormal - dx_backnarrow);                           // io
      
      //  Now jump down to the repeated pattern...
      dy[7]  = -dy_delta - dy[3] - dy[5]; // oi
      dx[8]  =  dx[2]; //io
      dy[9]  =  dy[3]; //oo
      dx[10] =  dx[4]; //oi
      dy[11] =  dy[5]; //ii
      dx[12] =  dx[6]; //io
      dy[13] = -(dy_excess - 2.0*dy_gap - PolygonWidth);      // oo
      dx[14] = -(dx_backnormal - PolygonWidth);               // oo
      
      dy[15] = -dy[13]; //oo??????????????
      dx[16] =  dx[12]; //
      dy[17] = -dy[11]; //
      dx[18] =  dx[10]; //
      dy[19] = -dy[9];  //
      dx[20] =  dx[8];  //
      dy[21] = -dy[7];  //
      dx[22] =  dx[6];  //
      dy[23] = -dy[5];  //
      dx[24] =  dx[4];  //
      dy[25] = -dy[3];  //
      dx[26] =  dx[2];  //
      dy[27] = -dy[1];  //
      
      x = x_origin_C + 0.5*dx_footprint - 0.5*dx_backnormal + 0.5*PolygonWidth;
      y = y_origin_A - 0.5*PolygonWidth;  // = top of back traces aligns with "A" shape.
      
      cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"16\">" << endl;
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
      for (int j=0; j<28; j++)
	{
	  x += dx[j];
	  y += dy[j];
	  if (curve[j]==0)
	    {
	      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	    }
	  else {
	      double cx1 = x - Sign(dx[j])*InnerRadius;
	      double cy1 = y - Sign(dy[j])*InnerRadius;
	      double cx2 = x + Sign(dx[j+1])*InnerRadius;
	      double cy2 = y + Sign(dy[j+1])*InnerRadius;
	      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx1 ) << "\" y=\"" << BOARD_TOP_Y - ( cy1 ) << "\" curve=\"90\" />" << endl;
	      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx2 ) << "\" y=\"" << BOARD_TOP_Y - ( cy2 ) << "\"/>" << endl;
	    }
	}
      cout << "</polygon>" << endl;
      
      double viax= x_origin_C+(0.5*dx_footprint);
      double viay= y_origin_C-(dy_pad/2);
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
      
      viax= x_origin_C+(0.5*dx_footprint);
      viay= y_origin_C-(dy_pad/2) - (dy_delta);
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
      
      cout << "</signal>" << endl;
      
      x_origin_C += 2.0*dx_brd;
    }
  
  else {
      MakePadCPrime(i);
    }
}


void MakePadCPrime(int i)
{
  //x_origin=999; need origin for back pad 
  //y_origin is needed for padC so it doesnt mess up the origin of dpad
  
  double dx[32] = {0};
  double dy[32] = {0};
  //int curve[32] = {0};
  //curve at [1,4,5,7,10,11,16,17,20,21,24,26,27,30]
  int curve[32] = {0,1,0,0,1,1,0,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,1,0,1,1,0,0,1};

  dx[0]=  (dx_footprint - PolygonWidth); // oo
  dy[1]= -(dy_pad - PolygonWidth);       // oo
  dx[2]= -(dx_footprint - PolygonWidth); // oo
  dy[3]=  (dy_pad - PolygonWidth);       // oo

  double x = x_origin_C + 0.5*PolygonWidth;
  double y = y_origin_C - 0.5*PolygonWidth;

  cout << "<signal name=\"PAD$C" << i << "\">" << endl;
  cout << "<contactref element=\"R" << i*4+1 << "\" pad=\"" << 2 << "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+2 << "\" pad=\"" << 1 << "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+1+2000 << "\" pad=\"" << 2 << "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+2+2000 << "\" pad=\"" << 1 << "\"/>" << endl;

  //  Make the front polygon #1
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;

  //  Make Front Polygon #2
  x= x_origin_C + 0.5*PolygonWidth;
  y= y_origin_C - (dy_delta) - 0.5*PolygonWidth;
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;

  // Make the back polygon
  dx[0] =      (dx_backnormal - PolygonWidth);                                        // oo
  dy[1] =     -(dy_space - dy_gap);                                              // oi
  dx[2] =  0.5*(dx_backwide - dx_backnormal);                                    // io
  dy[3] =     -(3.0*dy_gap + dy_footprint + dy_inside - PolygonWidth);                // oo
  dx[4] = -0.5*(dx_backwide - dx_backnarrow);                                    // oi
  dy[5] =     -(dy_sep - dy_gap + dy_inside + dy_footprint + dy_gap + dy_gap + dy_gap + PolygonWidth);  // ii
  dx[6] =  0.5*(dx_backnormal - dx_backnarrow);                                  // io

  //  Now jump down to the repeated pattern...
  dy[7]  = -dy_delta - dy[3] - dy[5]; // oi
  dx[8]  =  dx[2]; // io
  dy[9]  =  dy[3]; // oo
  dx[10] =  dx[4]; // oi
  dy[11] =  dy[5]; // ii 
  dx[12] =  dx[6]; // io

  //make the odd peice
  dy[13] = -(dy_excess - 2.0*dy_gap - PolygonWidth);                        // oo
  dx[14] = -(dx_backnormal - PolygonWidth);                                 // oo
  dy[15] =  (dy_excess - dy_magicpad - dy_sep - 2.0*dy_gap - PolygonWidth);  // oo
  dx[16] =  (dx_magicgap + 0.5*dx_magicpad - dx_backgap);    // oi
  dy[17] =  (2.0*dy_gap + dy_magicpad + PolygonWidth);            // ii
  dx[18] =  -dx[16];                                         // io
  dy[19] =  (dy_sep - 2.0*dy_gap - PolygonWidth);                 // oo

  dx[20]=  dx[12]; // 
  dy[21]= -dy[11]; // 
  dx[22]=  dx[10]; // 
  dy[23]= -dy[9];  // 
  dx[24]=  dx[8];  // 
  dy[25]= -dy[7];  // 
  dx[26]=  dx[6];  // 
  dy[27]= -dy[5];  // 
  dx[28]=  dx[4];  // 
  dy[29]= -dy[3];  // 
  dx[30]=  dx[2];  // 
  dy[31]= -dy[1];  // 

  x = x_origin_C + 0.5*dx_footprint - 0.5*dx_backnormal + 0.5*PolygonWidth;
  y = y_origin_A - 0.5*PolygonWidth;  // = top of back traces aligns with "A" shape.

  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"16\">" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<32; j++)
    {
      x += dx[j];
      y += dy[j];
      if (curve[j]==0)
	{
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      else {
	  double cx1 = x - Sign(dx[j])*InnerRadius;
	  double cy1 = y - Sign(dy[j])*InnerRadius;
	  double cx2 = x + Sign(dx[j+1])*InnerRadius;
	  double cy2 = y + Sign(dy[j+1])*InnerRadius;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx1 ) << "\" y=\"" << BOARD_TOP_Y - ( cy1 ) << "\" curve=\"90\" />" << endl;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx2 ) << "\" y=\"" << BOARD_TOP_Y - ( cy2 ) << "\"/>" << endl;
	}
    }
  cout << "</polygon>" << endl;

  double viax= x_origin_C+(0.5*dx_footprint);
  double viay= y_origin_C-(dy_pad/2);
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;

  viax= x_origin_C+(0.5*dx_footprint);
  viay= y_origin_C-(dy_pad/2) - (dy_delta);
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;

  cout << "</signal>" << endl;

  x_origin_C += 2.0*dx_brd;  
}


void MakePadD(int i)
{
  if(i != 36)
    {
      //_origin += dx_biggap;
      // Pad D is the pad contianing the vias, only 4 sides
      
      double dx[28] = {0};
      double dy[28] = {0};
      //int curve[28] = {0};
      //curve at [2,3,6,8,9,12,15,18,19,21,24,25]
      int curve[28] = {0,0,1,1,0,0,1,0,1,1,0,0,1,0,0,1,0,0,1,1,0,1,0,0,1,1};

      dx[0]=  (dx_footprint - PolygonWidth); // oo
      dy[1]= -(dy_pad - PolygonWidth);       // oo
      dx[2]= -(dx_footprint - PolygonWidth); // oo
      dy[3]=  (dy_pad - PolygonWidth);       // oo
      
      double x = x_origin_D + 0.5*PolygonWidth;
      double y = y_origin_D - 0.5*PolygonWidth;
            
      cout << "<signal name=\"PAD$D" << i << "\">" << endl;
      cout << "<contactref element=\"R" << i*4+3 << "\" pad=\"" << 2 << "\"/>" << endl;
      cout << "<contactref element=\"R" << i*4+4 << "\" pad=\"" << 1 << "\"/>" << endl;
      cout << "<contactref element=\"R" << i*4+3+2000 << "\" pad=\"" << 2 << "\"/>" << endl;
      cout << "<contactref element=\"R" << i*4+4+2000 << "\" pad=\"" << 1 << "\"/>" << endl;
      
      //  Make the front polygon #1
      cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
      for (int j=0; j<4; j++)
	{
	  x += dx[j];
	  y += dy[j];
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      cout << "</polygon>" << endl;
      
      //  Make front Polygon #2
      x = x_origin_D + 0.5*PolygonWidth;
      y = y_origin_D - (dy_delta) - 0.5*PolygonWidth;  
      cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
      for (int j=0; j<4; j++)
	{
	  x += dx[j];
	  y += dy[j];
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      cout << "</polygon>" << endl;


      dx[0] =      (dx_backnormal - PolygonWidth);                                                    // oo
      dy[1] =     -(dy_space - 2.0*dy_gap - PolygonWidth);                                            // oo
      dx[2] = -0.5*(dx_backnormal - dx_backnarrow);                                    // oi
      dy[3] =     -(3.0*dy_gap + dy_footprint + dy_inside + 2.0*dy_gap + PolygonWidth);               // ii
      dx[4] =  0.5*(dx_backwide - dx_backnarrow);                                      // io
      dy[5] =     -(dy_sep - 2.0*dy_gap + dy_inside + dy_footprint + dy_gap + dy_gap - PolygonWidth); // oo
      dx[6] = -0.5*(dx_backwide - dx_backnormal);                                      // oi
      
      //  Now jump and repeat the preceeding pattern
      dy[7]  = -dy_delta - dy[3] - dy[5]; // io
      dx[8]  =  dx[2];  // oi
      dy[9]  =  dy[3];  // ii
      dx[10] =  dx[4];  // io
      dy[11] =  dy[5];  // oo
      dx[12] =  dx[6];  // oi
      
      dy[13] = -(dy_excess - dy_gap);             // io
      dx[14] = -(dx_backnormal - PolygonWidth);        // oo
      dy[15] =  -dy[13]; // 
      dx[16] =   dx[12]; // 
      dy[17] =  -dy[11]; // 
      dx[18] =   dx[10]; // 
      dy[19] =  -dy[9];  // 
      dx[20] =   dx[8];  // 
      dy[21] =  -dy[7];  // 
      dx[22] =   dx[6];  // 
      dy[23] =  -dy[5];  // 
      dx[24] =   dx[4];  // 
      dy[25] =  -dy[3];  // 
      dx[26] =   dx[2];  // 
      dy[27] =  -dy[1];  // 
      
      x = x_origin_D + 0.5*dx_footprint - 0.5*dx_backnormal + 0.5*PolygonWidth;
      y = y_origin_A - 0.5*PolygonWidth;  // =top of back traces aligns with "A" shape.
      
      cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"16\">" << endl;
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
      for (int j=0; j<28; j++)
	{
	  x += dx[j];
	  y += dy[j];
	  if (curve[j]==0)
	    {
	      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	    }
	  else {
	      double cx1 = x - Sign(dx[j])*InnerRadius;
	      double cy1 = y - Sign(dy[j])*InnerRadius;
	      double cx2 = x + Sign(dx[j+1])*InnerRadius;
	      double cy2 = y + Sign(dy[j+1])*InnerRadius;
	      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx1 ) << "\" y=\"" << BOARD_TOP_Y - ( cy1 ) << "\" curve=\"90\" />" << endl;
	      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx2 ) << "\" y=\"" << BOARD_TOP_Y - ( cy2 ) << "\"/>" << endl;
	    }
	}
      cout << "</polygon>" << endl;
      
      double viax= x_origin_D+(0.5*dx_footprint);
      double viay= y_origin_D-(dy_pad/2);
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
      
      viax= x_origin_D+(0.5*dx_footprint);
      viay= y_origin_D - dy_delta - (dy_pad/2);
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
      cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
      
      cout << "</signal>" << endl;
      
      x_origin_D += 2.0*dx_brd;
      
    }
  else {
      MakePadDPrime(i);
    }
}


void MakePadDPrime(int i)
{
  //x_origin += dx_biggap;
  // Pad D is the pad contianing the vias, only 4 sides
  
  double dx[32] = {0};
  double dy[32] = {0};
  //int curve[32] = {0};
  //curve at [2,3,6,8,9,12,14,15,19,22,23,25,28,29]
  int curve[32] = {0,0,1,1,0,0,1,0,1,1,0,0,1,0,1,1,0,0,0,1,0,0,1,1,0,1,0,0,1,1};

  dx[0]=  (dx_footprint - PolygonWidth); // oo
  dy[1]= -(dy_pad - PolygonWidth);       // oo
  dx[2]= -(dx_footprint - PolygonWidth); // oo
  dy[3]=  (dy_pad - PolygonWidth);       // oo

  double x = x_origin_D + 0.5*PolygonWidth;
  double y = y_origin_D - 0.5*PolygonWidth;


  cout << "<signal name=\"PAD$D" << i << "\">" << endl;
  cout << "<contactref element=\"R" << i*4+3 << "\" pad=\"" << 2 << "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+4 << "\" pad=\"" << 1 << "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+3+2000 << "\" pad=\"" << 2 << "\"/>" << endl;
  cout << "<contactref element=\"R" << i*4+4+2000 << "\" pad=\"" << 1 << "\"/>" << endl;

  //  Make the front polygon #1
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;

  //  Make front Polygon #2
  x = x_origin_D + 0.5*PolygonWidth;
  y = y_origin_D - (dy_delta) - 0.5*PolygonWidth;  
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;


  dx[0] =      (dx_backnormal - PolygonWidth);                                       // oo
  dy[1] =     -(dy_space - 2.0*dy_gap - PolygonWidth);                               // oo
  dx[2] = -0.5*(dx_backnormal - dx_backnarrow);                        // oi
  dy[3] =     -(3.0*dy_gap + dy_footprint + dy_inside + 2.0*dy_gap + PolygonWidth);  // ii
  dx[4] =  0.5*(dx_backwide - dx_backnarrow);                          // io
  dy[5] =     -(dy_sep - 2.0*dy_gap + dy_inside + dy_footprint + dy_gap + dy_gap - PolygonWidth); // oo
  dx[6] = -0.5*(dx_backwide - dx_backnormal);                          // oi

  //  Now jump and repeat the preceeding pattern
  dy[7]  = -dy_delta - dy[3] - dy[5];  // io
  dx[8]  =  dx[2]; // oi
  dy[9]  =  dy[3]; // ii
  dx[10] =  dx[4]; // io
  dy[11] =  dy[5]; // oo
  dx[12] =  dx[6]; // oi

  //  Now we need to start the "notch" for the magic pad
  dy[13] = -(dy_sep - dy_gap);                            // io
  dx[14] = -(dx_magicgap + 0.5*dx_magicpad - dx_backgap); // oi
  dy[15] = -(2.0*dy_gap + dy_magicpad + PolygonWidth);         // ii
  dx[16] = (dx_magicgap + 0.5*dx_magicpad - dx_backgap);  // io
  dy[17] = -(dy_excess - dy_sep - dy_magicpad -2.0*dy_gap - PolygonWidth); // oo
  dx[18] = -(dx_backnormal - PolygonWidth);                    //oo

  dy[19] = (dy_excess - dy_gap);  // oi

  dx[20] =  dx[12]; // 
  dy[21] = -dy[11]; // 
  dx[22] =  dx[10]; // 
  dy[23] = -dy[9];  // 
  dx[24] =  dx[8];  // 
  dy[25] = -dy[7];  // 
  dx[26] =  dx[6];  // 
  dy[27] = -dy[5];  // 
  dx[28] =  dx[4];  // 
  dy[29] = -dy[3];  // 
  dx[30] =  dx[2];  // 
  dy[31] = -dy[1];  // 

  x = x_origin_D + 0.5*dx_footprint - 0.5*dx_backnormal + 0.5*PolygonWidth;
  y = y_origin_A - 0.5*PolygonWidth;  //  =top of back traces aligns with "A" shape.

  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"16\">" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<32; j++)
    {
      x += dx[j];
      y += dy[j];
      if (curve[j]==0)
	{
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      else {
	  double cx1 = x - Sign(dx[j])*InnerRadius;
	  double cy1 = y - Sign(dy[j])*InnerRadius;
	  double cx2 = x + Sign(dx[j+1])*InnerRadius;
	  double cy2 = y + Sign(dy[j+1])*InnerRadius;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx1 ) << "\" y=\"" << BOARD_TOP_Y - ( cy1 ) << "\" curve=\"90\" />" << endl;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx2 ) << "\" y=\"" << BOARD_TOP_Y - ( cy2 ) << "\"/>" << endl;
	}
    }
  cout << "</polygon>" << endl;

  double viax= x_origin_D+(0.5*dx_footprint);
  double viay= y_origin_D-(dy_pad/2);
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;

  viax= x_origin_D+(0.5*dx_footprint);
  viay= y_origin_D - dy_delta - (dy_pad/2);
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) << "\" extent=\"1-16\" drill=\"" << DrillSize << "\" shape=\"square\"/>" << endl;

  cout << "</signal>" << endl;

  x_origin_D += 2.0*dx_brd;  
}


void MakeGND()
{
  //  x_origin += dx_brd-dx_biggap;
  //  y_origin = y0_brd;
  
  // pad B has the top resistor on the left side
  
  double dx[24] = {0};
  double dy[24] = {0};
  int curve[24] = {0};
  curve[2] = 1;
  curve[3] = 1;
  curve[6] = 1;
  curve[7] = 1;
  curve[11] = 1;
  curve[14] = 1;
  curve[15] = 1;
  curve[17] = 1;
  curve[20] = 1;
  curve[21] = 1;

  
  dx[0] =  (dx_wid - PolygonWidth);                                                                         // oo
  dy[1] = -(dy_space + dy_inside + dy_footprint + dy_gap + dy_sep + dy_gap + dy_footprint - PolygonWidth);  // oo                      
  dx[2] = -(dx_medgap);                                 // oi                      
  dy[3] = -(dy_inside + PolygonWidth);                                                                      // ii
  dx[4] =  (dx_medgap);                                 // io

  //  Now make the jump...
  dy[5] = -(dy_delta - dy_inside - PolygonWidth); //oo
  dx[6] =   dx[2]; // 
  dy[7] =   dy[3];
  dx[8] =   dx[4];
  dy[9] = -(dy_excess - PolygonWidth);  // oo


  dx[10] =     -(dx_wid - PolygonWidth);                                                 // oo
  dy[11] =      (dy_excess + dy_gap + dy_footprint + dy_inside + dy_sep + dy_gap); // oi
  dx[12] = -0.5*(dx_gap+dx_footprint);                                             // io
  dy[13] =      (dy_footprint - PolygonWidth);                                           // oo
  dx[14] =      (dx_long);                                                         // oi
  dy[15] =      (dy_inside + PolygonWidth);                                              // ii
  dx[16] =     -(dx_medgap);                                                       // io

  dy[17] = (dy_delta - dy_footprint - dy_inside); // oi
  dx[18] =  dx[12];
  dy[19] =  dy[13];
  dx[20] =  dx[14];
  dy[21] =  dy[15];
  dx[22] =  dx[16];

  dy[23] = (dy_space - PolygonWidth);     // oo
  
  double x = x_origin_B + 0.5*PolygonWidth;
  double y = y_origin_B - 0.5*PolygonWidth;
  
  cout << "<signal name=\"GND\">" << endl;
  cout << "<contactref element=\"R"<< TPC_Nresistors << "\" pad=\"" << 2<< "\"/>" << endl;
  int bignum = 2000+TPC_Nresistors;
  cout << "<contactref element=\"R"<< bignum << "\" pad=\"" << 2<< "\"/>" << endl;
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<24; j++)
    {
      x += dx[j];
      y += dy[j];
      if (curve[j]==0)
	{
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      else {
	  double cx1 = x - Sign(dx[j])*InnerRadius;
	  double cy1 = y - Sign(dy[j])*InnerRadius;
	  double cx2 = x + Sign(dx[j+1])*InnerRadius;
	  double cy2 = y + Sign(dy[j+1])*InnerRadius;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx1 ) << "\" y=\"" << BOARD_TOP_Y - ( cy1 ) << "\" curve=\"90\" />" << endl;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx2 ) << "\" y=\"" << BOARD_TOP_Y - ( cy2 ) << "\"/>" << endl;
	}
    }
  cout << "</polygon>" << endl;
  cout << "</signal>" << endl;
  
  //y_origin -= dy_long+dy_footgap;//setting up the y cord for the next dpad
  
 x_origin_B += 2*dx_brd;
}


void MakePost()
{
  //x_origin += dx_biggap;
  //Pad D is the pad contianing the vias, only 4 sides
  
  double dx[16] = {0};
  double dy[16] = {0};
  int curve[16] = {0};
  curve[2]=1;  
  curve[3]=1;
  curve[6]=1;
  curve[8]=1;
  curve[9]=1;
  curve[12]=1;


  dx[0]=  (0.5*dx_footprint - 0.5*dx_gap - PolygonWidth); // oo
  dy[1]= -(dy_pad - PolygonWidth);       // oo
  dx[2]= -dx[0];       // oo
  dy[3]= -dy[1];       // oo

  double x = x_origin_D + 0.5*PolygonWidth;
  double y = y_origin_D - 0.5*PolygonWidth;

  cout << "<signal name=\"POSTPAD\">" << endl;

  //  Make the front polygon #1
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;

  //  Make front Polygon #2
  x = x_origin_D + 0.5*PolygonWidth;
  y = y_origin_D - (dy_delta) - 0.5*PolygonWidth;  
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;


  dx[0] =     -(0.5*dx_backnormal -0.5*dx_gap - EndPolygonWidth);                                    // oo
  dy[1] =     -(dy_space - 2.0*dy_gap - EndPolygonWidth);                                            // oo
  dx[2] =  0.5*(dx_backnormal - dx_backnarrow);                                    // oi
  dy[3] =     -(3.0*dy_gap + dy_footprint + dy_inside + 2.0*dy_gap + EndPolygonWidth);               // ii
  dx[4] = -0.5*(dx_backwide - dx_backnarrow);                                      // io
  dy[5] =     -(dy_sep - 2.0*dy_gap + dy_inside + dy_footprint + dy_gap + dy_gap - EndPolygonWidth); // oo
  dx[6] =  0.5*(dx_backwide - dx_backnormal);                                      // oi

  //  Now jump and repeat the preceeding pattern
  dy[7]  =  -dy_delta - dy[3] - dy[5]; // io
  dx[8]  =   dx[2]; // 
  dy[9]  =   dy[3]; // 
  dx[10] =   dx[4]; // 
  dy[11] =   dy[5]; // 
  dx[12] =   dx[6]; // 
  dy[13] = -(dy_excess - dy_gap);      // io

  dx[14] =  -dx[0];                    // oo
  dy[15] = -(dy[1]+dy[3]+dy[5]+dy[7]+dy[9]+dy[11]+dy[13]);

  x = x_origin_D + 0.5*dx_footprint - 0.5*dx_gap + 0.5*EndPolygonWidth;
  y = y_origin_A - 0.5*EndPolygonWidth;  // =top of back traces aligns with "A" shape.

  cout << "<polygon width=\"" << EndPolygonWidth << "\" layer=\"16\">" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<16; j++)
    {
      x += dx[j];
      y += dy[j];
      if (curve[j]==0)
	{
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
	}
      else {
	  double cx1 = x - Sign(dx[j])*InnerRadius;
	  double cy1 = y - Sign(dy[j])*InnerRadius;
	  double cx2 = x + Sign(dx[j+1])*InnerRadius;
	  double cy2 = y + Sign(dy[j+1])*InnerRadius;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx1 ) << "\" y=\"" << BOARD_TOP_Y - ( cy1 ) << "\" curve=\"-90\" />" << endl;
	  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( cx2 ) << "\" y=\"" << BOARD_TOP_Y - ( cy2 ) << "\"/>" << endl;
	}
    }
  cout << "</polygon>" << endl;

  double viax= x_origin_D+(0.5*(0.5*dx_footprint - 0.5*dx_gap));
  double viay= y_origin_D-(dy_pad/2);
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) << "\" extent=\"1-16\" drill=\"" << SmDrillSize << "\" shape=\"square\"/>" << endl;
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) << "\" extent=\"1-16\" drill=\"" << SmDrillSize << "\" shape=\"square\"/>" << endl;

  viax = x_origin_D+(0.5*(0.5*dx_footprint - 0.5*dx_gap));
  viay = y_origin_D - dy_delta - (dy_pad/2);
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay-1 ) << "\" extent=\"1-16\" drill=\"" << SmDrillSize << "\" shape=\"square\"/>" << endl;
  cout << "<via x=\"" << BOARD_RIGHT_X - ( viax ) << "\" y=\"" << BOARD_TOP_Y - ( viay+1 ) << "\" extent=\"1-16\" drill=\"" << SmDrillSize << "\" shape=\"square\"/>" << endl;

  cout << "</signal>" << endl;
}


void MakePostPads()
{
  //x_origin += dx_biggap;
  //Pad D is the pad contianing the vias, only 4 sides
  
  double dx[16] = {0};
  double dy[16] = {0};
  

  dx[0]=  (0.5*dx_footprint - 0.5*dx_gap - PolygonWidth); // oo
  dy[1]= -(dy_footprint - PolygonWidth);       // oo
  dx[2]= -dx[0];       // oo
  dy[3]= -dy[1];       // oo

  double x = x_origin_D + 0.5*PolygonWidth;
  double y = y_origin_D - dy_pad - dy_footgap - 0.5*PolygonWidth;

  cout << "<signal name=\"SMITH\">" << endl;

  //  Make the front polygon #1
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;
  cout << "</signal>" << endl;

  //  Make front Polygon #2
  cout << "<signal name=\"WESSON\">" << endl;
  x = x_origin_D + 0.5*PolygonWidth;
  y = y_origin_D - dy_pad - dy_footgap - (dy_delta) - 0.5*PolygonWidth;  
  cout << "<polygon width=\"" << PolygonWidth << "\" layer=\"1\" >" << endl;
  cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
  for (int j=0; j<4; j++)
    {
      x += dx[j];
      y += dy[j];
      cout << "<vertex x=\"" << BOARD_RIGHT_X - ( x ) << "\" y=\"" << BOARD_TOP_Y - ( y ) << "\"/>" << endl;
    }
  cout << "</polygon>" << endl;
  cout << "</signal>" << endl;
}


double Sign(double a)
{
  if (a>0) return  1.0;
  if (a<0) return -1.0;
  return 0.0;
}


void MakeCutouts()
{
  double y[4];
  y[0] = y_origin_C + 0.5*dy_footgap;
  y[1] = y_origin_C - dy_pad - 0.5*dy_footgap;
  y[2] = y_origin_D + 0.5*dy_footgap;
  y[3] = y_origin_D - dy_pad - 0.5*dy_footgap;

  //  Pattern-A
  double PAdx[4];
  PAdx[0] = -0.5*dx_footprint - 0.5*THERMAL_WIDTH;
  PAdx[1] = -0.5*dx_footprint - 0.5*THERMAL_WIDTH;
  PAdx[2] =  PolygonWidth;
  PAdx[3] = -PolygonWidth;

  double PAdy[4];
  PAdy[0] = 0.5*dy_footgap + 0.5*dy_footprint + PolygonWidth;
  PAdy[1] = 0.5*dy_footgap + 0.5*dy_footprint - PolygonWidth;
  PAdy[2] = -(0.5*dy_footgap + dy_footprint + 0.5*THERMAL_WIDTH);
  PAdy[3] = -(0.5*dy_footgap + dy_footprint + 0.5*THERMAL_WIDTH);

  //  Pattern-B
  double PBdx[4];
  PBdx[0] = 0.5*dx_footprint + 0.5*THERMAL_WIDTH;
  PBdx[1] = 0.5*dx_footprint + 0.5*THERMAL_WIDTH;
  PBdx[2] =  PolygonWidth;
  PBdx[3] = -PolygonWidth;

  double PBdy[4];
  PBdy[0] = -(0.5*dy_footgap + 0.5*dy_footprint + PolygonWidth);
  PBdy[1] = -(0.5*dy_footgap + 0.5*dy_footprint - PolygonWidth);
  PBdy[2] = 0.5*dy_footgap + dy_footprint + 0.5*THERMAL_WIDTH;
  PBdy[3] = 0.5*dy_footgap + dy_footprint + 0.5*THERMAL_WIDTH;


  for(int count = 0; count < TPC_Nresistors; count++) 
    {
      double xRes = x_origin_VCC + dx_CEE + 0.5*dx_footprint + (count/2)*dx_brd;
      double yRes = y[count%4];
      if (count%2) // Pattern B
	{
	  for (int i=0; i<4; i++)
	    {
	      OutputOneCutout( xRes+PBdx[i] , yRes+PBdy[i] );          // Rn
	      OutputOneCutout( xRes+PBdx[i] , yRes+PBdy[i]-dy_delta ); // R2000+n
	    }
	}
      else  // Pattern A
	{
	  for (int i=0; i<4; i++)
	    {
	      OutputOneCutout( xRes+PAdx[i] , yRes+PAdy[i] );          // Rn
	      OutputOneCutout( xRes+PAdx[i] , yRes+PAdy[i]-dy_delta ); // R2000+n
	    }
	}
    } 

}

void OutputOneCutout(double xCenter, double yCenter)
{
  static int blah=0;
  blah++;

  // cout << " " << BOARD_RIGHT_X;
  // cout << " " << xCenter;
  // cout << " " << BOARD_TOP_Y;
  // cout << " " << yCenter;
  // cout << endl;

  cout << "<signal name=\"Shit$"<<blah<<"\">" << endl;
  cout << "<polygon width=\"0.01\" layer=\"1\" pour=\"cutout\">" << endl;

  cout << "<vertex x=\""<< BOARD_RIGHT_X - (xCenter-0.5*THERMAL_WIDTH) <<"\" y=\""<< BOARD_TOP_Y - (yCenter)                   <<"\" curve=\"-90\"/>" << endl;
  cout << "<vertex x=\""<< BOARD_RIGHT_X - (xCenter)                   <<"\" y=\""<< BOARD_TOP_Y - (yCenter+0.5*THERMAL_WIDTH) <<"\" curve=\"-90\"/>" << endl;
  cout << "<vertex x=\""<< BOARD_RIGHT_X - (xCenter+0.5*THERMAL_WIDTH) <<"\" y=\""<< BOARD_TOP_Y - (yCenter)                   <<"\" curve=\"-90\"/>" << endl;
  cout << "<vertex x=\""<< BOARD_RIGHT_X - (xCenter)                   <<"\" y=\""<< BOARD_TOP_Y - (yCenter-0.5*THERMAL_WIDTH) <<"\" curve=\"-90\"/>" << endl;

  cout << "</polygon>" << endl;
  cout << "</signal>" << endl;
 /*
    <signal name="S$2">
     <polygon width="0" layer="1" pour="cutout">
      <vertex x="4.55" y="64.04" curve="-90"/>
      <vertex x="4.56" y="64.05" curve="-90"/>
      <vertex x="4.57" y="64.04" curve="-90"/>
      <vertex x="4.56" y="64.03" curve="-90"/>
     </polygon>
    </signal>
  */
}

/*
<signal name="PAD$C1">
<contactref element="R5" pad="2"/>
<contactref element="R6" pad="1"/>
<polygon width="0.4064" layer="1">
<vertex x="46.99" y="77.47"/>
<vertex x="52.07" y="77.47"/>
<vertex x="52.07" y="68.58"/>
<vertex x="46.99" y="68.58"/>
</polygon>
<polygon width="0.4064" layer="16">
<vertex x="44.45" y="41.91"/>
<vertex x="44.45" y="83.82"/>
<vertex x="54.61" y="83.82"/>
<vertex x="55.88" y="83.82"/>
<vertex x="55.88" y="41.91"/>
</polygon>
<via x="49.53" y="71.12" extent="1-16" drill=""<<DrillSize<<"" shape="square"/>
<via x="49.53" y="74.93" extent="1-16" drill="0.6" shape="square"/>
</signal>
*/


/*
  Sample of output for stripes  
  <vertex x="13.97" y="63.5"/>
  <vertex x="33.02" y="63.5"/>
  <vertex x="34.29" y="63.5"/>
  <vertex x="34.29" y="6.35"/>
  <vertex x="15.24" y="6.35"/>
  <vertex x="13.97" y="6.35"/>
*/


/*
  Sample of output for vias
<polygon width="0.4064" layer="1">
<vertex x="21.59" y="55.88"/>
<vertex x="25.4" y="55.88"/>
<vertex x="25.4" y="45.72"/>
<vertex x="21.59" y="45.72"/>
</polygon>
<via x="22.86" y="52.07" extent="1-16" drill="0.6" shape="square"/>
<via x="22.86" y="49.53" extent="1-16" drill="0.6" shape="square"/>
</signal>
*/
