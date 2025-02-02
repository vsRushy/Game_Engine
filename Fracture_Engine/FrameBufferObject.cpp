#include "GL/glew.h"

#include "Application.h"
#include "Globals.h"

#include"FrameBufferObject.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleUserInterface.h"
#include "PanelScene.h"

FrameBufferObject::FrameBufferObject()
{

}

FrameBufferObject::~FrameBufferObject()
{

}

void FrameBufferObject::GenerateFBO()
{
	glGenFramebuffers(1, &fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

	CreateTexture();
	GenerateRBO();

	auto fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbo_status != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG(LOG_ERROR, "Frame Buffer Object not complete: %d", fbo_status);
	}
	else
	{
		LOG(LOG_INFORMATION, "Successfully created Frame Buffer Object");
	}

	if (fbo_status == GL_FRAMEBUFFER_UNSUPPORTED)
		LOG(LOG_ERROR, "Implementation is not supported by OpenGL driver %d", fbo_status);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::GenerateRBO()
{
	glGenRenderbuffers(1, &rbo_id);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->GetWindowWidth(), App->window->GetWindowHeight());

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_id);
}

void FrameBufferObject::CreateTexture()
{
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int w = App->window->GetWindowWidth();
	int h = App->window->GetWindowHeight();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);
}

void FrameBufferObject::BindAndOperateOnFBO(const ImVec2& viewport_size)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
	
	glClearColor(0.07f, 0.07f, 0.07f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SetCamera(viewport_size);
	SetDepthBuffer(viewport_size);
	SetTexture(viewport_size);

	glStencilFunc(GL_ALWAYS, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void FrameBufferObject::SetCamera(const ImVec2& size)
{
	glViewport(0, 0, size.x, size.y);

	glMatrixMode(GL_PROJECTION);
	App->renderer3D->ProjectionMatrix = perspective(App->camera->fov, size.x / size.y, App->camera->near_plane, App->camera->far_plane);
	glLoadMatrixf(&App->renderer3D->ProjectionMatrix);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(App->camera->GetViewMatrix());
}

void FrameBufferObject::SetDepthBuffer(const ImVec2& size)
{
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void FrameBufferObject::SetTexture(const ImVec2& size)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}


void FrameBufferObject::UnbindFBO()
{
	/* Default */
	glStencilFunc(GL_ALWAYS, 1, 0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::DeleteFBO()
{
	glDeleteTextures(1, &texture_id);
	glDeleteRenderbuffers(1, &rbo_id);
	glDeleteFramebuffers(1, &fbo_id);
}
