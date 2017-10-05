#ifndef __BOARD_H__
#define __BOARD_H__

void FullBoard();
void InsertFile(const char *);
void MakeElements();
void MakeSignals();

void MakePlain();
void MakePre();
void MakePrePads();
void MakeVCC();
void MakePadA(int i);
void MakePadB(int i);
void MakePadC(int i);
void MakePadD(int i);
void MakePadCPrime(int i);
void MakePadDPrime(int i);
void MakeGND();
void MakePost();
void MakePostPads();
void MakeCutouts();
void OutputOneCutout(double xCenter, double yCenter);

double Sign(double );

#endif /*__BOARD_H__*/

/*
<elements>
<element name="R1" library="resistor" package="M1206" value="" x="24.13" y="57.15" rot="R270"/>
<element name="R2" library="resistor" package="M1206" value="" x="24.13" y="44.45" rot="R270"/>
<element name="R3" library="resistor" package="M1206" value="" x="45.72" y="57.15" rot="R270"/>
<element name="R4" library="resistor" package="M1206" value="" x="45.72" y="44.45" rot="R270"/>
</elements>

<signals>
<signal name="PAD$A1">
<contactref element="R1" pad="2"/>
<contactref element="R2" pad="1"/>
<polygon width="0.4064" layer="16">
<vertex x="13.97" y="63.5"/>
<vertex x="33.02" y="63.5"/>
<vertex x="34.29" y="63.5"/>
<vertex x="34.29" y="6.35"/>
<vertex x="15.24" y="6.35"/>
<vertex x="13.97" y="6.35"/>
</polygon>
<polygon width="0.4064" layer="1">
<vertex x="21.59" y="55.88"/>
<vertex x="25.4" y="55.88"/>
<vertex x="25.4" y="45.72"/>
<vertex x="21.59" y="45.72"/>
</polygon>
<via x="22.86" y="52.07" extent="1-16" drill="0.6" shape="square"/>
<via x="22.86" y="49.53" extent="1-16" drill="0.6" shape="square"/>
</signal>

<signal name="PAD$C1">
<contactref element="R2" pad="2"/>
<contactref element="R3" pad="1"/>
<polygon width="0.4064" layer="1">
<vertex x="21.59" y="63.5"/>
<vertex x="49.53" y="63.5"/>
<vertex x="49.53" y="58.42"/>
<vertex x="43.18" y="58.42"/>
<vertex x="43.18" y="36.83"/>
<vertex x="20.32" y="36.83"/>
<vertex x="20.32" y="43.18"/>
<vertex x="27.94" y="43.18"/>
<vertex x="27.94" y="60.96"/>
<vertex x="21.59" y="60.96"/>
</polygon>
</signal>

<signal name="PAD$B1">
<contactref element="R3" pad="2"/>
<contactref element="R4" pad="1"/>
<polygon width="0.4064" layer="1">
<vertex x="44.45" y="55.88"/>
<vertex x="48.26" y="55.88"/>
<vertex x="48.26" y="45.72"/>
<vertex x="44.45" y="45.72"/>
</polygon>
<polygon width="0.4064" layer="16">
<vertex x="36.83" y="63.5"/>
<vertex x="55.88" y="63.5"/>
<vertex x="57.15" y="63.5"/>
<vertex x="57.15" y="6.35"/>
<vertex x="38.1" y="6.35"/>
<vertex x="36.83" y="6.35"/>
</polygon>
<via x="45.72" y="52.07" extent="1-16" drill="0.6" shape="square"/>
<via x="45.72" y="49.53" extent="1-16" drill="0.6" shape="square"/>
</signal>
</signals>
*/
