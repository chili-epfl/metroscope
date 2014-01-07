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
seq 110 200 | while read n; do ../../dist/applications/craftag/tagcreator $n; mogrify -scale 4000% $n.png; done;
scala makeTaggedFaces.scala 110
convert 110.svg 110.pdf
*/

val fromTagId = argv(0) toInt
val tagSize = 20.0
val faceSize = 35.0
//val pageWidth = 210.0
//val pageHeight = 297.0
val pageWidth = 297.0
val pageHeight = 210.0

val col = pageWidth/faceSize toInt
val lin = pageHeight/faceSize toInt
val xPageMargin = (pageWidth-faceSize*col)/2.0
val yPageMargin = (pageHeight-faceSize*lin)/2.0
val padding = (faceSize-tagSize)/2.0

val xs = (0 until col)
val ys = (0 until lin)
val tags = fromTagId until (fromTagId+(col*lin))

val data = (ys.map(y => xs.map(x => (x,y))) flatten) zip tags

def build(header: String, line: String, sep: String, footer: String) = 
  header +
  (
    data map { case ((x,y),t) => line.format(t,
      xPageMargin+x*faceSize+padding,yPageMargin+y*faceSize+padding,
      xPageMargin+x*faceSize,yPageMargin+y*faceSize)} mkString(sep)
  ) +
  footer


val svgheader = """<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg xmlns:svg="http://www.w3.org/2000/svg" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1" width="%.2fmm" height="%.2fmm">
""".format(pageWidth, pageHeight)
val svgline = """<image xlink:href="%%d.png" x="%%.2fmm" y="%%.2fmm" width="%.2fmm" height="%.2fmm"/>
<rect style="fill:none;stroke:#000000;stroke-opacity:1" x="%%.2fmm" y="%%.2fmm" width="%.2fmm" height="%.2fmm" />""".format(tagSize, tagSize, faceSize, faceSize)
val svgfooter = """
</svg>"""

val svgwriter = new java.io.FileWriter("%d.svg".format(fromTagId))
svgwriter.write(build(svgheader, svgline, "\n", svgfooter))
svgwriter.close

