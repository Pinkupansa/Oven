#include "Oven/Platform/OpenGL/OpenGLShader.h"
#include <iostream> 
#include <Oven.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>
class TestLayer : public Oven::Layer
{
    public: 
        TestLayer() : Layer("Example"), m_CameraController(16.0f/9.0f),  m_SquarePosition(0.0f), m_SquareColor(0.2f, 0.3f, 0.8f){

            m_VertexArray.reset(Oven::VertexArray::Create());

            float vertices[3*7] = { 
                -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  
                0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 
                0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
            };

            //Create vertex buffer and layout

            Oven::Ref<Oven::VertexBuffer> vertexBuffer;
            vertexBuffer.reset(Oven::VertexBuffer::Create(vertices, sizeof(vertices)));
        
            {
                Oven::BufferLayout layout = {
                    {Oven::ShaderDataType::Float3, "a_Position"},
                    {Oven::ShaderDataType::Float4, "a_Color"},
                };
                vertexBuffer->SetLayout(layout);
            }
            
            Oven::Ref<Oven::IndexBuffer> indexBuffer;
            //Create index buffer 
            uint32_t indices[3] = {0, 1, 2};
            indexBuffer.reset(Oven::IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t)));
            
            //Bind to vertex array
            m_VertexArray->AddVertexBuffer(vertexBuffer);
            m_VertexArray->SetIndexBuffer(indexBuffer);


            m_SquareVA.reset(Oven::VertexArray::Create());
            float squareVertices[5*4] = { 
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
            };
            
            Oven::Ref<Oven::VertexBuffer> squareVB;
            squareVB.reset(Oven::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
            
            squareVB->SetLayout({
                {Oven::ShaderDataType::Float3, "a_Position"},
                {Oven::ShaderDataType::Float2, "a_TexUV"}
            });

            uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
            
            Oven::Ref<Oven::IndexBuffer> squareIB; 
            squareIB.reset(Oven::IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t)));
            m_SquareVA->AddVertexBuffer(squareVB);
            m_SquareVA->SetIndexBuffer(squareIB);

            std::string vertexSrc = R"(
                #version 330 core 

                layout(location = 0) in vec3 a_Position;
                layout(location = 1) in vec4 a_Color;

                uniform mat4 u_ViewProjection;
                uniform mat4 u_Model; 

                out vec3 v_Position;
                out vec4 v_Color;

                void main(){
                    v_Position = a_Position;
                    v_Color = a_Color;
                    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
                }

            )";

            std::string fragmentSrc =R"(
                #version 330 core 

                layout(location = 0) out vec4 color;

                in vec3 v_Position;
                in vec4 v_Color;

                void main(){
                    color = vec4(v_Position*0.5 + 0.5, 1.0);
                    color = v_Color;
                }

            )";

            std::string vertexSrc2 = R"(
                #version 330 core 

                layout(location = 0) in vec3 a_Position;

                uniform mat4 u_ViewProjection;
                uniform mat4 u_Model;
                out vec3 v_Position;

                void main(){
                    v_Position = a_Position;
                    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
                }

            )";

            std::string fragmentSrc2 =R"(
                #version 330 core 

                layout(location = 0) out vec4 color;

                uniform vec3 u_Color; 
                in vec3 v_Position;

                void main(){
                    color = vec4(u_Color, 1); 
                }

            )"; 

            m_SingleColorShader = Oven::Shader::Create("SingleColorShader", vertexSrc2, fragmentSrc2);
            m_Shader = Oven::Shader::Create("TestShader", vertexSrc, fragmentSrc);
            
            auto textureShader = m_ShaderLibrary.Load("sandbox/assets/shaders/Texture.glsl");
            
            uint32_t texSlot = 0;
            m_OvenLogoTexture = Oven::Texture2D::Create("sandbox/assets/oven_logo_notext.png");

            textureShader->Bind();
            std::dynamic_pointer_cast<Oven::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", texSlot);
            textureShader->Unbind();
            

            m_WaterTexture = Oven::Texture2D::Create("sandbox/assets/textures/water.png");

        }

        void OnUpdate() override{ 

            //Update
            m_CameraController.OnUpdate(); 
            if(Oven::Input::KeyPressed(OVEN_KEY_L)){
                m_SquarePosition.x += m_SquareSpeed * Oven::Time::GetDeltaTime();
            }
            if(Oven::Input::KeyPressed(OVEN_KEY_J)){
                m_SquarePosition.x -= m_SquareSpeed * Oven::Time::GetDeltaTime();
            }
            if(Oven::Input::KeyPressed(OVEN_KEY_I)){
                m_SquarePosition.y += m_SquareSpeed * Oven::Time::GetDeltaTime();
            }
            if(Oven::Input::KeyPressed(OVEN_KEY_K)){
                m_SquarePosition.y -= m_SquareSpeed * Oven::Time::GetDeltaTime();
            }

            //Render
            Oven::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
            Oven::RenderCommand::Clear();
            
            
            Oven::Renderer::BeginScene(m_CameraController.GetCamera());

            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1));
            
            glm::vec4 blackColor(0.01, 0.01, 0.01, 1);
            glm::vec4 whiteColor(1, 1, 1, 1);
            
            m_SingleColorShader->Bind();
            std::dynamic_pointer_cast<Oven::OpenGLShader>(m_SingleColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);
            
            for(int x = 0; x < 8; x++){
                for(int y = 0; y < 8; y++){
                    glm::vec3 pos(x * 0.11f, y*0.11f, 0.0f);
                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos + m_SquarePosition) * scale;
                    Oven::Renderer::Submit(m_SingleColorShader, m_SquareVA, transform);
                }
            }

            auto textureShader = m_ShaderLibrary.Get("Texture"); 
            m_OvenLogoTexture->Bind(0);
            Oven::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
            Oven::Renderer::EndScene();
  
        }

        void OnImGuiRender() override{
            ImGui::Begin("Settings");
            ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
            ImGui::End();

        }

        void OnEvent(Oven::Event& event) override{
            m_CameraController.OnEvent(event);
            Oven::EventDispatcher dispatcher(event); 
            dispatcher.Dispatch<Oven::KeyPressedEvent>(OVEN_BIND_EVENT_FN(TestLayer::OnKeyPressedEvent));
        }

        bool OnKeyPressedEvent(Oven::KeyPressedEvent& event){
            return false;
        }

        private: 
            Oven::ShaderLibrary m_ShaderLibrary; 
            Oven::Ref<Oven::Shader> m_Shader;
            Oven::Ref<Oven::Shader> m_SingleColorShader;
            Oven::Ref<Oven::VertexArray> m_VertexArray;
            
            Oven::Ref<Oven::VertexArray> m_SquareVA;
            Oven::OrthographicCameraController m_CameraController;

            glm::vec3 m_SquarePosition;

            float m_SquareSpeed = 3;

            glm::vec3 m_SquareColor;

            Oven::Ref<Oven::Texture2D> m_WaterTexture;
            Oven::Ref<Oven::Texture2D> m_OvenLogoTexture;
};
class Sandbox : public Oven::Application
{
    public:
        Sandbox()
        {
            PushLayer(new TestLayer());
        }
        ~Sandbox()
        {

        }
    
};

Oven::Application* Oven::CreateApplication(){
    printf("Launching Sandbox App ...\n");
    return new Sandbox();
}