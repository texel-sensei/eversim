#pragma once

#include "core\utility\plattform.h"

class switch_viewport{
        public:
            switch_viewport(GLint x, GLint y, GLsizei w, GLsizei h) {
                glGetIntegerv(GL_VIEWPORT, viewport);
                glViewport(x,y,w,h);
            }
            ~switch_viewport() {
                glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
            }

        private:
            GLint viewport[4];

            switch_viewport(switch_viewport const&) = delete;
            switch_viewport& operator=(switch_viewport const&) = delete;
};