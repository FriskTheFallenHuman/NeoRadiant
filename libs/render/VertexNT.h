#pragma once

#include "math/Vector3.h"

/// 3D vertex with normal and UV coordinates
struct VertexNT
{
    Vector3 vertex;		// 3D position
    Vector2 texcoord;	// UV coordinates
    Vector3 normal;		// Normal vector

    bool operator==(const VertexNT& other) const
    {
        return (vertex == other.vertex 
                && texcoord == other.texcoord 
                && normal == other.normal);
    }
};


