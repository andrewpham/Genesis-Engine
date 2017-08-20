// GL includes
#include "LifeOfGaben.h"
#include "Breakout.h"
#include "RealisticSkin.h"
#include "RigidBodySim.h"
#include "Demos.h"
#include "Window.h"

gwindow::Window window;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	window.create(WINDOW_NAME, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();
	glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

	/** Uncomment only this function to play the Life of Gaben game... */
	run_gaben_game(window.getWindow());

	/** ...or uncomment only this function to run Breakout... */
	//play_breakout(window.getWindow());

	/** ...or uncomment only this function to view the skin rendering demo */
	//run_skin_demo(window.getWindow());

	/** ...or uncomment only this function to run the rigid body simulation */
	//run_physics_demo(window.getWindow());

	/** ...or uncomment ONLY ONE of these functions to display a demo showcasing some of the 3D rendering capabilities of this game engine */
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