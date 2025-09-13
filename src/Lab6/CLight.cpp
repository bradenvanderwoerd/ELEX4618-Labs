#include "Lab6/CLight.h"

#include <glm/gtc/type_ptr.hpp>

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

CLight::CLight() {}

CLight::CLight(glm::vec3 pos, glm::vec3 col) {
	_position = pos;
	_color = col;
	
	init_depth_map();
	update_light_space_matrix();
}

CLight::~CLight() {
	glDeleteFramebuffers(1, &_depth_map_FBO);
	glDeleteTextures(1, &_depth_map);
}

void CLight::init_depth_map() {
    glGenFramebuffers(1, &_depth_map_FBO);
    glGenTextures(1, &_depth_map);

    glBindTexture(GL_TEXTURE_2D, _depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, _depth_map_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth_map, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CLight::update_light_space_matrix() {
    glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.0f);
    glm::mat4 light_view = glm::lookAt(_position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    _light_space_matrix = light_projection * light_view;
}

void CLight::render_depth_map(GLuint depth_shader, CShip* &ship, std::vector<CAsteroid*>& asteroids, std::vector<CMissile*>& missiles) {
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, _depth_map_FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(depth_shader);

    // Compute light space matrix
    glm::mat4 light_projection = ship->get_projection_matrix();
    glm::mat4 light_view = glm::lookAt(_position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    _light_space_matrix = light_projection * light_view;

    GLint light_space_loc = glGetUniformLocation(depth_shader, "light_space_matrix");
    glUniformMatrix4fv(light_space_loc, 1, GL_FALSE, glm::value_ptr(_light_space_matrix));

    // Render depth for each object
    ship->draw();
    for (CAsteroid* asteroid : asteroids)
        asteroid->draw();
    for (CMissile* missile : missiles)
        missile->draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}