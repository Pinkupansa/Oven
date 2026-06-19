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

class TestLayer : public Oven::Layer
{
    public: 
        TestLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CamPos(0.0f), m_SquarePosition(0.0f){

            m_VertexArray.reset(Oven::VertexArray::Create());

            float vertices[3*7] = { 
                -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  
                0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 
                0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
            };

            //Create vertex buffer and layout

            std::shared_ptr<Oven::VertexBuffer> vertexBuffer;
            vertexBuffer.reset(Oven::VertexBuffer::Create(vertices, sizeof(vertices)));
        
            {
                Oven::BufferLayout layout = {
                    {Oven::ShaderDataType::Float3, "a_Position"},
                    {Oven::ShaderDataType::Float4, "a_Color"},
                };
                vertexBuffer->SetLayout(layout);
            }
            
            std::shared_ptr<Oven::IndexBuffer> indexBuffer;
            //Create index buffer 
            uint32_t indices[3] = {0, 1, 2};
            indexBuffer.reset(Oven::IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t)));
            
            //Bind to vertex array
            m_VertexArray->AddVertexBuffer(vertexBuffer);
            m_VertexArray->SetIndexBuffer(indexBuffer);


            m_SquareVA.reset(Oven::VertexArray::Create());
            float squareVertices[3*4] = { 
                -0.75f, -0.75f, 0.0f,
                0.75f, -0.75f, 0.0f,
                0.75f, 0.75f, 0.0f,
                -0.75f, 0.75f, 0.0f
            };
            
            std::shared_ptr<Oven::VertexBuffer> squareVB;
            squareVB.reset(Oven::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
            
            squareVB->SetLayout({
                {Oven::ShaderDataType::Float3, "a_Position"}
            });

            uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
            
            std::shared_ptr<Oven::IndexBuffer> squareIB; 
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
            m_SingleColorShader.reset(Oven::Shader::Create(vertexSrc2, fragmentSrc2));
            m_Shader.reset(Oven::Shader::Create(vertexSrc, fragmentSrc));

        }

        void OnUpdate() override{

            if(Oven::Input::KeyPressed(OVEN_KEY_RIGHT)){
                m_CamPos.x += m_CamSpeed * Oven::Time::GetDeltaTime();
            }
            if(Oven::Input::KeyPressed(OVEN_KEY_LEFT)){
                m_CamPos.x -= m_CamSpeed * Oven::Time::GetDeltaTime();
            }
            if(Oven::Input::KeyPressed(OVEN_KEY_UP)){
                m_CamPos.y += m_CamSpeed * Oven::Time::GetDeltaTime();
            }
            if(Oven::Input::KeyPressed(OVEN_KEY_DOWN)){
                m_CamPos.y -= m_CamSpeed * Oven::Time::GetDeltaTime();
            }

            if(Oven::Input::KeyPressed(OVEN_KEY_A)){
                m_CamRot += m_CamRotSpeed * Oven::Time::GetDeltaTime();
            }
            if(Oven::Input::KeyPressed(OVEN_KEY_D)){
                m_CamRot -= m_CamRotSpeed * Oven::Time::GetDeltaTime();
            }
            
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

            m_Camera.SetPosition(m_CamPos);
            m_Camera.SetRotation(m_CamRot);
            Oven::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
            Oven::RenderCommand::Clear();
            
            
            Oven::Renderer::BeginScene(m_Camera);

            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1 * 0.5f/0.75f));
            
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
            //Oven::Renderer::Submit(m_Shader, m_VertexArray);
            Oven::Renderer::EndScene();
  
        }

        void OnImGuiRender() override{
            ImGui::Begin("Settings");
            ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
            ImGui::End();

        }

        void OnEvent(Oven::Event& event) override{
            Oven::EventDispatcher dispatcher(event); 
            dispatcher.Dispatch<Oven::KeyPressedEvent>(OVEN_BIND_EVENT_FN(TestLayer::OnKeyPressedEvent));
        }

        bool OnKeyPressedEvent(Oven::KeyPressedEvent& event){
            return false;
        }

        private: 
            std::shared_ptr<Oven::Shader> m_Shader;
            std::shared_ptr<Oven::Shader> m_SingleColorShader;
            std::shared_ptr<Oven::VertexArray> m_VertexArray;

            std::shared_ptr<Oven::VertexArray> m_SquareVA;
            Oven::OrthographicCamera m_Camera;
            glm::vec3 m_CamPos;
            glm::vec3 m_SquarePosition;
            
            float m_CamRot = 0;
            float m_CamSpeed = 5;
            float m_CamRotSpeed = 50;
            float m_SquareSpeed = 3;

            glm::vec3 m_SquareColor;

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