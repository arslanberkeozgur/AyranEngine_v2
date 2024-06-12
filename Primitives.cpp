#include "Primitives.h"

                                   // Coords        // Texture
const float Quad::vertices[] = {  1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 
								  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 
                                 -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 
                                 -1.0f,  1.0f, 0.0f, 0.0f, 1.0f };

const unsigned int Quad::indices[] = { 0, 1, 2,
                                       0, 3, 2 };

