#ifndef __TPCSPECS_H__
#define __TPCSPECS_H__

#define TPC_Nresistors 298
//#define TPC_Nresistors 14
#define ResistorsPerRow 32
#define PadTypes 4

//  WARNING:  Hand-typed...must be recalculated if you change number of stripes!
#define MagicA 36
#define MagicC 37
#define MagicD 36

#define PolygonWidth 0.4
//#define PolygonWidth 0.1   //2017-04-20 Vlad: Solid polygon need to be thinner for smooth shapes by Cutout polygons
#define EndPolygonWidth 0.2
#define DrillSize 0.500
#define SmDrillSize 0.250
#define InnerRadius 0.400

// Variables for drawing the schematic...
#define x0_sch 2.54  // 0.1 inches
#define y0_sch 579.12// 5.0 inches
#define dx_sch 17.78 // 0.7 inches
#define dy_sch 15.24 // 0.6 inches
#define r_size 5.08  // 0.2 inches

// Variables for drawing the board...
#define dx_brd 2.8
#define dx_gap 0.5
#define dx_wid (dx_brd - dx_gap)
#define dx_footprint 2.0
#define dx_rgap 0.55
#define dx_long (dx_footprint + dx_rgap)
#define dx_medgap (dx_rgap + 0.5*dx_footprint - 0.5*dx_gap)

#define dx_magicgap 0.4
#define dx_magicpad 1.0

#define dy_space 10
#define dy_gap 1
#define dy_footprint 1.6
#define dy_footgap 1.2
#define dy_rlength 4
#define dy_sep 4
#define dy_inside (2.0*dy_footgap + 3.0*dy_footprint + dy_rlength + dy_gap)

#define dy_magicpad 6.0

#define THERMAL_WIDTH (0.014*25.4)

#define dx_CEE (dx_wid + 0.5*dx_gap - 0.5*dx_footprint)
#define dy_CEE (dy_space + dy_gap + dy_footprint + dy_footgap)
#define dx_DEE (dx_CEE + dx_brd)
#define dy_DEE (dy_space + dy_gap + dy_footprint + dy_inside + dy_sep + dy_gap + dy_footprint + dy_footgap)
#define dy_pad (dy_footprint + dy_footprint + dy_rlength)

#define dx_backnormal 1.8
#define dx_skinny     (dx_wid + 0.5*dx_gap + 0.5*dx_footprint - dx_long)
#define dx_backgap    0.5
#define dx_backwide   (2.0*(dx_wid + 0.5*dx_gap - 0.5*dx_skinny - 0.5*dx_backgap))
#define dx_backnarrow (dx_gap + dx_skinny - dx_backgap)

#define dy_delta     (dy_sep + dy_gap + dy_footprint + dy_inside + dy_sep + dy_gap + dy_footprint + dy_inside)

#define BOARD_LEFT_X 0
#define BOARD_RIGHT_X ((TPC_Nresistors+2)/2.0*dx_brd + dx_gap)

//  Choose only one of the lines below at a time!!
//#define DIAMETER_OF_CYLINDER (50)  // INNER FIELD CAGE
#define DIAMETER_OF_CYLINDER ((16.972 - 2*6*0.005)*25.4)  // INNER FIELD CAGE--12 layers of kapton!
//#define DIAMETER_OF_CYLINDER (60.190*25.4)  // OUTER FIELD CAGE...WARNING TOO BIG!!!!
#define CIRCUMFERENCE (DIAMETER_OF_CYLINDER*3.1415926)

//#define BOARD_TOP_Y (CIRCUMFERENCE + dy_space)                for regular board, not cover
#define BOARD_TOP_Y 0
#define BOARD_BOT_Y -170

#define dy_excess (BOARD_TOP_Y - 2.0*dx_gap - dy_space - 2.0*dy_gap - 2.0*dy_footprint - 2.0*dy_inside - dy_sep - dy_delta)

#define x0_brd dx_gap
#define y0_brd (BOARD_TOP_Y - dx_gap)


/*
-
-     For the Cover Layer
-
 */

#define dx_coverTop 1.15 //wrong value...... maybe
#define dx_coverShortWidth 0.575
#define dy_coverShort 25.4
#define dy_coverLong 127 //88.9 for shorter version


#endif /*__TPCSPECS_H__*/
