const char * VertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 Position;
    layout (location = 1) in vec2 TexCoord;
    layout (location = 2) in vec3 Normal;
    
    uniform mat4 Model;
    uniform mat4 View;
    uniform mat4 Proj;

    out vec3 ourNormal;
    out vec3 crntPos;
    out vec2 ourTexture;

    void main() {

        gl_Position = Proj * View * Model * vec4(Position,1.0f);
        ourNormal = vec3(Model * vec4(Normal,0));
        crntPos = vec3(Model * vec4(Position,1.0f));
        ourTexture = TexCoord;

    }
)";

const char * FragmentShaderSource = R"(
    #version 330 core
    out vec4 Frag;

    in vec3 ourNormal;
    in vec3 crntPos;
    in vec2 ourTexture;

    uniform float objInt;
    uniform sampler2D Texture;
    
    void main() {

    vec3 LightPos = vec3(0,0,0);
    vec3 LightColor = vec3(1,1,1);
    
    float ambient = 0.1f;
    vec3 ambientLight = ambient * LightColor;

    float diffuseInt = 0.3f;
    vec3 normal  = normalize(ourNormal);
    vec3 LightDir = normalize(LightPos - crntPos);
    float diffuse = max(dot(normal,LightDir),0);
    vec3 diffuseLight = diffuse *diffuseInt*LightColor;

    vec3 result = (ambientLight + diffuseLight) * objInt;
     
    Frag = vec4(result,1.0f) * texture(Texture,ourTexture);
    
    }
)";


