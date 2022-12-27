const char * VertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 Position;
    layout (location = 2) in vec3 Normal;

    uniform mat4 Model;
    uniform mat4 View;
    uniform mat4 Proj;

    out vec3 ourNormal;
    out vec3 crntPos;

    void main() {

        gl_Position = Proj * View * Model * vec4(Position,1.0f);
        ourNormal = vec3(Model * vec4(Normal,0));
        crntPos = vec3(Model * vec4(Position,1.0f));

    }
)";

const char * FragmentShaderSource = R"(
    #version 330 core
    out vec4 Frag;
    in vec3 ourNormal;
    in vec3 crntPos;

    void main() {

        vec3 LightPos = vec3(-5,-5,0);
    float ambient = 0.0f;

    float diffuseInt = 0.3f;
    vec3 normal  = normalize(ourNormal);
    vec3 LightDir = normalize(normal - crntPos);
    float diffuse = max(dot(ourNormal,LightPos),0);

    float result = ambient + diffuse * diffuseInt;
        Frag = result * vec4(1.0f, 0.0f, 1.0f, 1.0f);
    }
)";


