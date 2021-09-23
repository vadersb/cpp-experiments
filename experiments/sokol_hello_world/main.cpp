//
// Created by Alexander on 23.09.2021.
//

#define SOKOL_IMPL
#define SOKOL_D3D11

#include "main.h"

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"

sg_pass_action pass_action;


void init(void)
{
	sg_desc desc;
	CleanupStruct(desc);

	desc.context = sapp_sgcontext();

	sg_setup(desc);

	CleanupStruct(pass_action);

	pass_action.colors[0].value = {1.0f, 0.0f, 0.0f, 1.0f};
	pass_action.colors[0].action = SG_ACTION_CLEAR;
}



void frame(void)
{
	float g = pass_action.colors[0].value.g + 0.01f;
	pass_action.colors[0].value.g = (g > 1.0f) ? 0.0f : g;
	sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
	sg_end_pass();
	sg_commit();

	//todo add drawing
}


void cleanup(void)
{
	sg_shutdown();
}


sapp_desc sokol_main(int argc, char* argv[])
{
	sapp_desc desc;

	CleanupStruct(desc);

	desc.init_cb = init;
	desc.frame_cb = frame;
	desc.cleanup_cb = cleanup;
	desc.width = 1280;
	desc.height = 800;
	desc.window_title = "Clear Sample";

	return desc;


}