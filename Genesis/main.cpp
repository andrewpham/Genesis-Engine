// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GL includes
#include "Demos.h"
#include "Window.h"

genesis::Window window;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	window.create(WINDOW_NAME, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

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
	render_instancing_demo(window.getWindow());

	return 0;
}