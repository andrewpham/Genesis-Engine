/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
// GL includes
#include "Breakout.h"
#include "Demos.h"
#include "Window.h"

gwindow::Window window;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	window.create(GAME_NAME, SCREEN_WIDTH, SCREEN_HEIGHT);
	//window.create(WINDOW_NAME, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();
	glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

	/** Uncomment this function to run Breakout... */
	play(window.getWindow());

	/** ...or uncomment ONE of these functions to display a demo showcasing some of the 3D rendering capabilities of this game engine */
	//render_superbible_perpixelgloss(window.getWindow());
	//render_superbible_cubemapenv(window.getWindow());
	//render_superbible_equirectangular(window.getWindow());
	//render_superbible_envmapsphere(window.getWindow());
	//render_superbible_rimlight(window.getWindow());
	//render_superbible_phonglighting(window.getWindow());
	//render_superbible_csflocking(window.getWindow());
	//render_superbible_shapedpoints(window.getWindow());
	//render_superbible_hdrtonemap(window.getWindow());
	//render_superbible_polygonsmooth(window.getWindow());
	//render_superbible_linesmooth(window.getWindow());
	//render_superbible_basicfbo(window.getWindow());
	//render_superbible_depthclamp(window.getWindow());
	//render_superbible_multiscissor(window.getWindow());
	//render_superbible_noperspective(window.getWindow());
	//render_superbible_multiviewport(window.getWindow());
	//render_superbible_gsquads(window.getWindow());
	//render_superbible_normalviewer(window.getWindow());
	//render_superbible_gstessellate(window.getWindow());
	//render_superbible_objectexploder(window.getWindow());
	//render_superbible_gsculling(window.getWindow());
	//render_superbible_cubicbezier(window.getWindow());
	//render_superbible_dispmap(window.getWindow());
	//render_superbible_tessmodes(window.getWindow());
	//render_superbible_clipdistance(window.getWindow());
	//render_superbible_multidrawindirect(window.getWindow());
	//render_superbible_instancedattribs(window.getWindow());
	//render_superbible_fragmentlist(window.getWindow());
	//render_skybox_demo(window.getWindow());
	//render_exploding_demo(window.getWindow());
	//render_instancing_demo(window.getWindow());

	return 0;
}