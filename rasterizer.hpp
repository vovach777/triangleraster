/*
 * Copyright (C) 2009 Josh A. Beam
 * Copyright (C) 2024 Vladimir Poslavskiy
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <cstdio>
#include <cstring>
#include <cmath>
#include <utility>
#include <initializer_list>
namespace Rasterizer {

template <typename Color>
struct Edge
{
		Color Color1{}, Color2{};
		int X1{}, Y1{}, X2{}, Y2{};
};

template <typename Color>
Edge<Color> MakeEdge(Color color1, int x1, int y1,
           Color color2, int x2, int y2)
{

	if(y1 < y2) {
        return {color1, color2, x1, y1, x2,y2};
	} else {
        return {color2, color1, x2, y2, x1,y1};
	}
}

template <typename Color>
struct Span {
    Color Color1{}, Color2{};
    int X1{}, X2{};
};

template <typename Color>
Span<Color> MakeSpan(Color color1, int x1, Color color2, int x2)
{
	if(x1 < x2) {
		return {color1,color2, x1,x2};
	} else {
        return {color2,color1,x2,x1};
	}
}


template<typename Span, typename SetPixelFunctor>
void DrawSpan(Span && span, int y, SetPixelFunctor && SetPixel)
{
	int xdiff = span.X2 - span.X1;
	if(xdiff == 0)
		return;

	auto colordiff = span.Color2 - span.Color1;

	float factor = 0.0f;
	float factorStep = 1.0f / (float)xdiff;

	// draw each pixel in the span
	for(int x = span.X1; x < span.X2; x++) {
		SetPixel(x, y, span.Color1 + (colordiff * factor));
		factor += factorStep;
	}
}

template<typename Edge, typename SetPixelFunctor>
void DrawSpansBetweenEdges(Edge &&e1, Edge &&e2, SetPixelFunctor && SetPixel)
{
	// calculate difference between the y coordinates
	// of the first edge and return if 0
	float e1ydiff = (float)(e1.Y2 - e1.Y1);
	if(e1ydiff == 0.0f)
		return;

	// calculate difference between the y coordinates
	// of the second edge and return if 0
	float e2ydiff = (float)(e2.Y2 - e2.Y1);
	if(e2ydiff == 0.0f)
		return;

	// calculate differences between the x coordinates
	// and colors of the points of the edges
	float e1xdiff = (float)(e1.X2 - e1.X1);
	float e2xdiff = (float)(e2.X2 - e2.X1);
	auto e1colordiff = (e1.Color2 - e1.Color1);
	auto e2colordiff = (e2.Color2 - e2.Color1);

	// calculate factors to use for interpolation
	// with the edges and the step values to increase
	// them by after drawing each span
	float factor1 = (float)(e2.Y1 - e1.Y1) / e1ydiff;
	float factorStep1 = 1.0f / e1ydiff;
	float factor2 = 0.0f;
	float factorStep2 = 1.0f / e2ydiff;

	// loop through the lines between the edges and draw spans
	for(int y = e2.Y1; y < e2.Y2; y++) {
		// create and draw span
		// Span span(e1.Color1 + (e1colordiff * factor1),
		//           e1.X1 + (int)(e1xdiff * factor1),
		//           e2.Color1 + (e2colordiff * factor2),
		//           e2.X1 + (int)(e2xdiff * factor2));
		DrawSpan(
            MakeSpan(e1.Color1 + (e1colordiff * factor1),
                            e1.X1 + (int)(e1xdiff * factor1),
                            e2.Color1 + (e2colordiff * factor2),
                            e2.X1 + (int)(e2xdiff * factor2)),
                            y, std::forward<SetPixelFunctor>(SetPixel));

		// increase factors
		factor1 += factorStep1;
		factor2 += factorStep2;
	}
}


template<typename Color, typename SetPixelFunctor>
void DrawTriangle(Color &&color1, float x1, float y1,
                  Color &&color2, float x2, float y2,
                  Color &&color3, float x3, float y3,
                   SetPixelFunctor && SetPixel)
{
	// create edges for the triangle (uses initializer_list to array trick)
	auto edges_il = {
		MakeEdge(color1, (int)x1, (int)y1, color2, (int)x2, (int)y2),
		MakeEdge(color2, (int)x2, (int)y2, color3, (int)x3, (int)y3),
		MakeEdge(color3, (int)x3, (int)y3, color1, (int)x1, (int)y1)
	};
    auto edges = std::begin(edges_il);

	int maxLength = 0;
	int longEdge = 0;

	// find edge with the greatest length in the y axis
	for(int i = 0; i < 3; i++) {
		int length = edges[i].Y2 - edges[i].Y1;
		if(length > maxLength) {
			maxLength = length;
			longEdge = i;
		}
	}

	int shortEdge1 = (longEdge + 1) % 3;
	int shortEdge2 = (longEdge + 2) % 3;

	// draw spans between edges; the long edge can be drawn
	// with the shorter edges to draw the full triangle
	DrawSpansBetweenEdges(edges[longEdge], edges[shortEdge1], std::forward<SetPixelFunctor>(SetPixel));
	DrawSpansBetweenEdges(edges[longEdge], edges[shortEdge2], std::forward<SetPixelFunctor>(SetPixel));
}

template<typename Color, typename SetPixelFunctor>
void DrawLine(  Color &&color1, float x1, float y1,
                Color &&color2, float x2, float y2,
                SetPixelFunctor && SetPixel)
{

	float xdiff = (x2 - x1);
	float ydiff = (y2 - y1);

	if(xdiff == 0.0f && ydiff == 0.0f) {
		SetPixel(x1, y1, color1);
		return;
	}

	if(fabs(xdiff) > fabs(ydiff)) {
		float xmin, xmax;

		// set xmin to the lower x value given
		// and xmax to the higher value
		if(x1 < x2) {
			xmin = x1;
			xmax = x2;
		} else {
			xmin = x2;
			xmax = x1;
		}

		// draw line in terms of y slope
		float slope = ydiff / xdiff;
		for(float x = xmin; x <= xmax; x += 1.0f) {
			float y = y1 + ((x - x1) * slope);
			SetPixel(x, y, color1 + ((color2 - color1) * ((x - x1) / xdiff)));
		}
	} else {
		float ymin, ymax;

		// set ymin to the lower y value given
		// and ymax to the higher value
		if(y1 < y2) {
			ymin = y1;
			ymax = y2;
		} else {
			ymin = y2;
			ymax = y1;
		}

		// draw line in terms of x slope
		float slope = xdiff / ydiff;
		for(float y = ymin; y <= ymax; y += 1.0f) {
			float x = x1 + ((y - y1) * slope);
			SetPixel(x, y, color1 + ((color2 - color1) * ((y - y1) / ydiff)));
		}
	}
}
}
