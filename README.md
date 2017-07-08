# RenderDevice

A demonstration of a graphics library abstraction layer that you might see in a typical rendering engine for games and simulations.

See http://amesgames.net/2017/06/27/c-rendering-engine-i-abstracting-the-render-device/.

## Dependencies

* C++11 Compiler
* CMake 3.2 or greater

## Features

* OpenGL 4.1 RenderDevice
 * Vertex Buffers
 * Index Buffers
 * Vertex Shaders
 * Fragment Shaders
 * 2D RGB Textures
 * Shader Uniform Variables
 * Raster States
 * Depth/Stencil States

* Platform Abstraction
 * Single window for the render viewport
 * Trackball interface for inspecting an object of interest

* Samples
 * Triangle: renders a static, solid-colored triangle in normalized device coordinates
 * Cube: renders a textured cube and supports the ability to rotate the cube with the left mouse button

## Roadmap

* OpenGL 4.1 RenderDevice
 * Various Pixel Formats for Textures
 * Blend States
 * Uniform Buffers
 * Shader Image Load/Store
 * Offscreen and Multiple Render Targets
 * Geometry Shaders
 * Tessellation Shaders
 * Compute Shaders

* Platform Abstraction
 * Multiple Window Support
 * Platform implementation for a Windows DirectX RenderDevice
