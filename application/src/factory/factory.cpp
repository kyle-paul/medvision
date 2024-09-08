#include "factory.h"
#include <stdexcept>

Factory::Factory(std::unordered_map<unsigned int, TransformComponent> &transform_components,
                 std::unordered_map<unsigned int, PhysicsComponent> &physics_components,
                 std::unordered_map<unsigned int, RenderComponent> &render_components) : transform_components(transform_components),
                                                                                         physics_components(physics_components),
                                                                                         render_components(render_components)
{
    EntityMade = 0;
}

Factory::~Factory()
{
}

std::vector<std::string> Factory::split(const std::string &line,
                                        const std::string &delimiter,
                                        std::vector<std::string> &result)
{
    size_t pos = 0;
    std::string token;
    std::string tempLine = line;
    while ((pos = tempLine.find(delimiter)) != std::string::npos)
    {
        token = tempLine.substr(0, pos);
        result.push_back(token);
        tempLine.erase(0, pos + delimiter.size());
    }
    result.push_back(tempLine);
    return result;
}

void Factory::create_mesh(const objectInit &object)
{

    TransformComponent transform;
    transform.position = object.position;
    transform.eulers = object.eulers;
    transform.zoom_factor = object.zoom_factor;
    transform_components[EntityMade] = transform;

    PhysicsComponent physics;
    physics.velocity = object.velocity;
    physics.eulerVelocity = object.eulerVelocity;
    physics_components[EntityMade] = physics;

    RenderComponent render = make_mesh(object.obj_path->c_str(), *object.scale, *object.onTexCoords);
    render.texture = new Texture(*object.texture_path);
    render_components[EntityMade++] = render;
}

RenderComponent Factory::make_mesh(const char *obj_path, const float &scale, const bool &onTexCoords)
{
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> faces;

    std::ifstream file(obj_path);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }

    std::string line;
    std::vector<std::string> words;
    words.reserve(8);

    while (std::getline(file, line))
    {
        words.clear();
        split(line, " ", words);

        if (!words.empty())
        {
            if (words[0] == "v" && words.size() >= 4)
            {
                float x = std::stof(words[1]) / scale;
                float y = std::stof(words[2]) / scale;
                positions.push_back(x);
                positions.push_back(y);
                positions.push_back(std::stof(words[3]) / scale);

                if (onTexCoords)
                {
                    texCoords.push_back((x + 1.0f) * 0.5f);
                    texCoords.push_back((y + 1.0f) * 0.5f);
                }
            }
            else if (words[0] == "vn" && words.size() >= 4)
            {
                normals.push_back(std::stof(words[1]));
                normals.push_back(std::stof(words[2]));
                normals.push_back(std::stof(words[3]));
            }
            else if (words[0] == "f")
            {
                for (size_t i = 1; i < words.size(); ++i)
                {
                    std::vector<std::string> parts;
                    split(words[i], "//", parts);
                    if (!parts.empty() && !parts[0].empty())
                    {
                        faces.push_back(std::stoi(parts[0]) - 1);
                    }
                }
            }
        }
    }
    VertexArray *vao = new VertexArray();
    IndexBuffer *fbo = new IndexBuffer(faces);
    VertexBuffer *tbo = new VertexBuffer(texCoords);

    VertexBuffer *vbo = new VertexBuffer(positions);
    VertexBuffer *nbo = new VertexBuffer(normals);

    VertexBufferLayout positions_layout;
    VertexBufferLayout normals_layout;
    VertexBufferLayout texCoord_layout;

    positions_layout.AddFloat(3);
    normals_layout.AddFloat(3);
    texCoord_layout.AddFloat(2);

    vao->Bind();
    vao->receiveBuffer(0, *vbo, positions_layout);
    vao->receiveBuffer(1, *nbo, normals_layout);
    vao->receiveBuffer(2, *tbo, texCoord_layout);

    RenderComponent render;
    render.vao = vao;
    render.fbo = fbo;
    return render;
}

void Factory::create_cube(glm::vec3 size, const std::string &texture_path,               // entity properties
                          glm::vec3 &position, glm::vec3 &eulers, glm::f32 &zoom_factor, // transform properties
                          glm::vec3 &velocity, glm::vec3 &eulerVelocity)                 // physics properties
{
    TransformComponent transform;
    transform.position = &position;
    transform.eulers = &eulers;
    transform_components[EntityMade] = transform;

    PhysicsComponent physics;
    physics.velocity = &velocity;
    physics.eulerVelocity = &eulerVelocity;
    physics_components[EntityMade] = physics;

    RenderComponent render = make_cube(size);
    render.texture = new Texture(texture_path);
    render_components[EntityMade++] = render;
}

RenderComponent Factory::make_cube(glm::vec3 &size)
{
    float l = size.x;
    float w = size.y;
    float h = size.z;

    std::vector<float> positions = {
        l, w, -h,
        l, -w, -h,
        -l, -w, -h,
        -l, w, -h,
        -l, -w, h,
        l, -w, h,
        l, w, h,
        -l, w, h};

    std::vector<float> normals = {
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f};

    std::vector<float> texCoords = {
        0.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 0.0f};

    std::vector<unsigned int> faces = {
        0, 1, 3,
        1, 2, 3,
        4, 5, 7,
        5, 6, 7,
        3, 2, 7,
        2, 4, 7,
        0, 1, 6,
        0, 6, 5,
        1, 2, 5,
        2, 4, 5,
        0, 3, 7,
        0, 7, 6};

    VertexArray *vao = new VertexArray();

    IndexBuffer *fbo = new IndexBuffer(faces);

    VertexBuffer *vbo = new VertexBuffer(positions);
    VertexBuffer *nbo = new VertexBuffer(normals);
    VertexBuffer *tbo = new VertexBuffer(texCoords);

    VertexBufferLayout positions_layout;
    VertexBufferLayout normals_layout;
    VertexBufferLayout texCoord_layout;

    positions_layout.AddFloat(3);
    normals_layout.AddFloat(3);
    texCoord_layout.AddFloat(2);

    vao->Bind();
    vao->receiveBuffer(0, *vbo, positions_layout);
    vao->receiveBuffer(1, *nbo, normals_layout);
    vao->receiveBuffer(2, *tbo, texCoord_layout);

    RenderComponent render;
    render.vao = vao;
    render.fbo = fbo;
    return render;
}