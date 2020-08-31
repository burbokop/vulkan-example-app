#!/bin/sh
glslc shader.vert -o vert.spv
glslc shader.frag -o frag.spv
glslc vert_uniform.vert -o vert_uniform.spv

