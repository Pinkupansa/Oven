#pragma once

namespace Oven{

    enum class RenderingAPI{
        None = 0, OpenGL = 1
    };

    class Renderer{

        public:
            inline static RenderingAPI GetRenderingAPI(){return s_RenderingAPI;}
        private:
            static RenderingAPI s_RenderingAPI;
    };
}