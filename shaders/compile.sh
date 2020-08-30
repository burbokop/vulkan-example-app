#!/bin/sh
glslc shader.vert -o vert.spv
glslc shader.frag -o frag.spv
glslc vertex_buffer.vert -o vertex_buffer.spv

