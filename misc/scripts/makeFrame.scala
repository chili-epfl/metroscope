/*******************************************************************************
*   Copyright 2013 EPFL                                                        *
*                                                                              *
*   This file is part of metroscope.                                           *
*                                                                              *
*   Metroscope is free software: you can redistribute it and/or modify         *
*   it under the terms of the GNU General Public License as                    *
*   published by the Free Software Foundation, either version 3 of the         *
*   License, or (at your option) any later version.                            *
*                                                                              *
*   Metroscope is distributed in the hope that it will be useful,              *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*   GNU General Public License for more details.                               *
*                                                                              *
*   You should have received a copy of the GNU General Public License          *
*   along with Metroscope.  If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************/

/* Example
seq 100 200 | while read n; do ../../../craftag/tagcreator $n; mogrify -scale 4000% $n.png; done;
scala makeFrame.scala 7 10 100
scala makeFrame.scala 7 10 130
convert 100.svg 100.pdf
convert 130.svg 130.pdf
pdftk 100.pdf 130.pdf cat output 100130.pdf
rm *.png 100.pdf 130.pdf *.svg
*/

val col = argv(0) toInt
val lin = argv(1) toInt
val fromTagId = argv(2) toInt
val width = 210.0
val height = 297.0
val tagSize = 20.0
val margin = 10.0

val xPadding = (width-tagSize*col)/(col+1)
val yPadding = (height-tagSize*lin)/(lin+1)

def repeat(v:Int, n:Int) = (1 to n) map (_ => v)
val xs = (1 until col) ++ repeat(col,lin-1) ++ (2 to col reverse) ++ repeat(1,lin-1)
val ys = repeat(1,col-1) ++ (1 until lin) ++ repeat(lin, col-1) ++ (2 to lin reverse)
val tags = fromTagId to (fromTagId+2*(col-1+lin-1))

val data = xs.toList zip ys.toList zip tags.toList


def build(header: String, line: String, sep: String, footer: String) = 
  header +
  (
  if (lin==2)
    (data map { case ((x,y),t) => line.format(t,x*xPadding+(x-1)*tagSize,if (y==1) margin else height-margin-tagSize)} mkString(sep) )
  else
    (data map { case ((x,y),t) => line.format(t,x*xPadding+(x-1)*tagSize,y*yPadding+(y-1)*tagSize)} mkString(sep) )
  )+
  footer

val cfgheader = """marker: { type: "aggregated"; size: 20.0; position: [%.2f, %.2f]; markers: (
""".format((width-tagSize)/2.0, (height-tagSize)/2.0)
val cfgline = """	{ marker: { type: "craftag"; id: %d; persistence: 0; }; position: [%.2f, %.2f]; size: 20.0; }"""
val cfgfooter = """
);};"""

val cfgwriter = new java.io.FileWriter("%d.cfg".format(fromTagId))
cfgwriter.write(build(cfgheader, cfgline, ",\n", cfgfooter))
cfgwriter.close


val svgheader = """<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg xmlns:svg="http://www.w3.org/2000/svg" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" width="210mm" height="297mm">
"""
val svgline = """<image xlink:href="tags/%d.png" x="%.2fmm" y="%.2fmm" width="2cm" height="2cm"/>"""
val svgfooter = """
</svg>"""

val svgwriter = new java.io.FileWriter("%d.svg".format(fromTagId))
svgwriter.write(build(svgheader, svgline, "\n", svgfooter))
svgwriter.close

