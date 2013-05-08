/*************************************************************
GeometryBuilder: Provides utility functions for creating 
geometrical objects and populating the renderer for it

Author: Valentin Hinov
Date: 09/04/2013
Version: 1.0

Requires: Vertex.h
**************************************************************/
#include "Vertex.h"
#include "Renderer.h"

#ifndef _H_GEOMETRY_BUILDER_H
#define _H_GEOMETRY_BUILDER_H

// Creates a sphere with the given number of rings and segments and then populates the vertex and index data into the target renderer
// REQUIRES: Renderer to be set to TRIANGLE_STRIP
bool BuildSphere(int nRings, int nSegments, ID3D10Device* device, Renderer *targetRenderer);

// Creates a sphere with the given color, number of rings and segments and then populates the vertex and index data into the target renderer
// REQUIRES: Renderer to be set to TRIANGLE_STRIP
bool BuildColoredSphere(int nRings, int nSegments, ID3D10Device* device, Renderer *targetRenderer, Color color);

// Creates a unit cube with the target color and then populates the vertex and index data into the target renderer
bool BuildColoredCube(ID3D10Device* device, Renderer *targetRenderer, Color color);

#endif