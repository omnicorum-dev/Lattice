//
// Created by Nico Russo on 4/29/26.
//

#include <primitives.h>

namespace Lattice {

    Model3D loadOBJ(const std::filesystem::path& filePath) {
        Model3D obj;

        std::ifstream file(filePath);
        if (!file.is_open()) {
            omni::LOG_ERROR("Could not open file {}", filePath.string());
            return {};
        }

        std::string line;

        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string prefix;
            ss >> prefix;

            // ---- Vertex positions ----
            if (prefix == "v") {
                glm::f32vec3 v;
                ss >> v.x >> v.y >> v.z;

                obj.vertices.push_back(v);
            }

            // ---- Vertex normals ----
            if (prefix == "vn") {
                glm::f32vec3 n;
                ss >> n.x >> n.y >> n.z;

                obj.normals.push_back(n);
            }

            // ---- Texture coordinates ----
            else if (prefix == "vt") {
                glm::f32vec2 uv;
                ss >> uv.x >> uv.y;
                obj.uvs.push_back(uv);
            }

            // ---- Faces ----
            else if (prefix == "f") {
                std::vector<Lattice::Model3D::FaceIndex> face;
                std::string vert;

                while (ss >> vert) {
                    // formats:
                    // v
                    // v/vt
                    // v/vt/vn
                    // v//vn

                    int vIndex  = -1;
                    int vnIndex = -1;
                    int vtIndex = -1;

                    size_t s1 = vert.find('/');
                    size_t s2 = std::string::npos;

                    if (s1 == s2) {
                        vIndex = std::stoi(vert) - 1;
                    } else {
                        vIndex = std::stoi(vert.substr(0, s1)) - 1;

                        s2 = vert.find('/', s1 + 1);

                        std::string vtStr =
                            (s2 == std::string::npos)
                            ? vert.substr(s1 + 1)
                            : vert.substr(s1 + 1, s2 - s1 - 1);

                        if (!vtStr.empty())
                            vtIndex = std::stoi(vtStr) - 1;
                    }

                    face.push_back({
                        vIndex, vtIndex
                    });
                }

                if (face.size() >= 3)
                    obj.faces.push_back(face);
            }
        }

        return obj;
    }

    // ==================== TRANSFORM ====================

    glm::mat4 Transform::toMatrix() const {
        const glm::mat4 T = glm::translate(glm::mat4{1.f}, position);
        const glm::mat4 R = glm::toMat4(rotation);
        const glm::mat4 S = glm::scale(glm::mat4{1.f}, scale);
        return T * R * S;
    }

    Transform Transform::operator*(const Transform &child) const {
        Transform out;
        out.position = position + rotation * (scale * child.position);
        out.rotation = rotation * child.rotation;
        out.scale = scale * child.scale;
        return out;
    }

    Transform Transform::inverse() const {
        Transform out;
        const glm::quat invRot   = glm::inverse(rotation);
        const glm::vec3 invScale = 1.f / scale;
        out.rotation = invRot;
        out.scale    = invScale;
        out.position = invRot * (invScale * -position);
        return out;
    }

    glm::vec3 Transform::transformPoint(const glm::vec3& point) const {
        return position + rotation * (scale * point);
    }

    glm::vec3 Transform::transformDirection(const glm::vec3& direction) const {
        // Direction ignores translation; scale affects magnitude, so we normalize
        return glm::normalize(rotation * (scale * direction));
    }

    glm::vec3 Transform::localDirection(const glm::vec3& direction) const {
        return rotation * direction;
    }

    glm::vec3 Transform::localDirectionInverse(const glm::vec3& direction) const {
        return glm::inverse(rotation) * direction;
    }

    glm::vec3 Transform::yawDirection(const glm::vec3& direction) const {
        // Get the fully-rotated local direction
        const glm::vec3 local = rotation * direction;

        // Flatten onto XZ plane (kill Y component) and renormalize
        const glm::vec3 flat = glm::vec3{local.x, 0.f, local.z};

        // Guard against looking straight up/down
        if (glm::length(flat) < 1e-6f) return local;

        return glm::normalize(flat);
    }

    void Transform::rotateAroundAxis(const glm::vec3 &axis, const float angleRadians) {
        rotation = glm::angleAxis(angleRadians, axis) * rotation;
    }

    void Transform::lookAt(const glm::vec3 &target) {
        const glm::vec3 dir = glm::normalize(position - target);

        // Degenerate case — target is directly above or below, up axis is ambiguous.
        if (glm::abs(glm::dot(dir, Dir3D::UP)) > 0.9999f) return;

        rotation = glm::quatLookAt(dir, Dir3D::UP);
    }

    glm::vec3 Transform::toEuler() const {
        return glm::eulerAngles(rotation);
    }

    Transform Transform::fromEuler(const glm::vec3 eulerRadians,
                                          const glm::vec3 position,
                                          const glm::vec3 scale) {
        return Transform {
            position,
            glm::quat{eulerRadians},
            scale
        };
    }

    Transform Transform::lerp(const Transform& a, const Transform& b, const float t) {
        return Transform {
            glm::mix(a.position, b.position, t),
            glm::slerp(a.rotation, b.rotation, t),
            glm::mix(a.scale, b.scale, t)
        };
    }

    float Transform::distanceTo(const Transform& other) const {
        return glm::distance(position, other.position);
    }

    glm::vec3 Transform::directionTo(const Transform& other) const {
        return glm::normalize(other.position - position);
    }


    // ==================== GENERIC FUNCTIONS ====================

    glm::vec3 rotate(const glm::vec3& p, const glm::quat& q) {
        return q * p;
    }

    glm::vec3 rotateInverse(const glm::vec3& p, const glm::quat& q) {
        return glm::conjugate(q) * p;
    }

    glm::vec3 scalePoint(const glm::vec3& p, const glm::vec3& scale) {
        return p * scale;
    }

    glm::vec3 applyTransform(const glm::vec3& p, const Transform& transform) {
        return transform.transformPoint(p);
    }

    // ==================== CAMERA3D ====================

    glm::mat4 Camera3D::viewMatrix() const {
        // View matrix is the inverse of the camera's world transform.
        // glm::lookAt is equivalent but building from the transform directly
        // avoids redundant decomposition.
        const glm::mat4 R = glm::toMat4(glm::conjugate(transform.rotation));
        const glm::mat4 T = glm::translate(glm::mat4{1.f}, -transform.position);
        return R * T;
    }

    glm::mat4 Camera3D::projectionMatrix(const float aspectRatio) const {
        //return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
        return glm::perspectiveLH_ZO(fov, aspectRatio, nearPlane, farPlane);
    }

    glm::mat4 Camera3D::viewProjectionMatrix(const float aspectRatio) const {
        return projectionMatrix(aspectRatio) * viewMatrix();
    }

    glm::vec3 Camera3D::yawDirection(const glm::vec3& direction) const {
        const float yaw = glm::yaw(transform.rotation);
        return glm::angleAxis(yaw, Dir3D::UP) * direction;
    }

    // ==================== RAY3D ====================

    glm::vec3 Ray3D::at(float t) const {
        return origin + direction * t;
    }

    float Ray3D::distanceTo(const glm::vec3& point) const {
        // Project point onto ray, then measure perpendicular distance.
        const glm::vec3 toPoint = point - origin;
        const float proj = glm::dot(toPoint, direction);
        const glm::vec3 closest = at(glm::max(proj, 0.f));
        return glm::distance(closest, point);
    }

    bool Ray3D::intersectsSphere(const glm::vec3 &center, float radius, float &t) const {
        // Solve quadratic: |origin + t*dir - center|^2 = radius^2
        const glm::vec3 oc = origin - center;
        const float a = glm::dot(direction, direction); // 1 if normalized
        const float b = 2.f * glm::dot(oc, direction);
        const float c = glm::dot(oc, oc) - radius * radius;
        const float discriminant = b * b - 4.f * a * c;

        if (discriminant < 0.f) return false;

        const float sqrtDisc = glm::sqrt(discriminant);
        const float t0 = (-b - sqrtDisc) / (2.f * a);
        const float t1 = (-b + sqrtDisc) / (2.f * a);

        // Take nearest positive t (in front of ray origin)
        if (t0 >= 0.f)      { t = t0; return true; }
        else if (t1 >= 0.f) { t = t1; return true; }
        return false;
    }

    bool Ray3D::intersectsPlane(const glm::vec3& normal,
                                   const float d, float& t) const {
        // Plane defined as dot(normal, p) = d.
        const float denom = glm::dot(normal, direction);

        // Ray is parallel to plane (or nearly so).
        if (glm::abs(denom) < 1e-6f) return false;

        t = (d - glm::dot(normal, origin)) / denom;
        return t >= 0.f;
    }

    bool Ray3D::intersectsAABB(const glm::vec3& min,
                                      const glm::vec3& max, float& t) const {
        // Slab method — intersect ray with each pair of axis-aligned planes.
        const glm::vec3 invDir = 1.f / direction;
        const glm::vec3 t0     = (min - origin) * invDir;
        const glm::vec3 t1     = (max - origin) * invDir;

        const glm::vec3 tMin = glm::min(t0, t1);
        const glm::vec3 tMax = glm::max(t0, t1);

        const float tEnter = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
        const float tExit  = glm::min(glm::min(tMax.x, tMax.y), tMax.z);

        if (tExit < 0.f || tEnter > tExit) return false;

        t = tEnter >= 0.f ? tEnter : tExit;
        return true;
    }

    Ray3D Ray3D::fromCamera(const Camera3D& cam,
                                   const glm::vec2 ndc,
                                   const float aspectRatio) {
        // Un-project NDC through the inverse view-projection matrix.
        const glm::mat4 invVP = glm::inverse(cam.viewProjectionMatrix(aspectRatio));

        // Two points on the ray in clip space (near and far plane).
        const glm::vec4 nearClip = invVP * glm::vec4{ndc.x, ndc.y, -1.f, 1.f};
        const glm::vec4 farClip  = invVP * glm::vec4{ndc.x, ndc.y,  1.f, 1.f};

        const glm::vec3 nearWorld = glm::vec3{nearClip} / nearClip.w;
        const glm::vec3 farWorld  = glm::vec3{farClip}  / farClip.w;

        return Ray3D {
            nearWorld,
            glm::normalize(farWorld - nearWorld)
        };
    }

    // ==================== AABB ====================

    glm::vec3 AABB::center() const {
        return (min + max) * 0.5f;
    }

    glm::vec3 AABB::size() const {
        return max - min;
    }

    bool AABB::contains(const glm::vec3& point) const {
        return glm::all(glm::greaterThanEqual(point, min)) &&
               glm::all(glm::lessThanEqual(point, max));
    }

    bool AABB::intersects(const AABB& other) const {
        return glm::all(glm::lessThanEqual(min, other.max)) &&
               glm::all(glm::greaterThanEqual(max, other.min));
    }

    AABB AABB::transformed(const Transform& t) const {
        // Transform all 8 corners and re-fit — necessary because rotation
        // and non-uniform scale mean the AABB axes no longer align.
        const glm::vec3 corners[8] = {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {min.x, max.y, min.z}, {max.x, max.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {min.x, max.y, max.z}, {max.x, max.y, max.z},
        };

        glm::vec3 newMin = t.transformPoint(corners[0]);
        glm::vec3 newMax = newMin;

        for (int i = 1; i < 8; i++) {
            const glm::vec3 p = t.transformPoint(corners[i]);
            newMin = glm::min(newMin, p);
            newMax = glm::max(newMax, p);
        }

        return AABB{newMin, newMax};
    }

    AABB AABB::fromModel(const Model3D& model) {
        if (model.vertices.empty()) return AABB{};

        glm::vec3 newMin = model.vertices[0];
        glm::vec3 newMax = model.vertices[0];

        for (const auto& v : model.vertices) {
            newMin = glm::min(newMin, v);
            newMax = glm::max(newMax, v);
        }

        return AABB{newMin, newMax};
    }

    namespace Primitives {

        Model3D::FaceIndex V(int v, int t, int n) {
            return {v, t, n};
        }

        std::vector<Model3D::FaceIndex> face(
            int a, int b, int c,
            int ta, int tb, int tc,
            int na, int nb, int nc)
        {
            return { V(a,ta,na), V(b,tb,nb), V(c,tc,nc) };
        }

        Model3D cube() {
            Model3D m;

            m.vertices = {
            {-0.5f,-0.5f,-0.5f}, {0.5f,-0.5f,-0.5f},
            {0.5f, 0.5f,-0.5f},  {-0.5f, 0.5f,-0.5f},
            {-0.5f,-0.5f, 0.5f}, {0.5f,-0.5f, 0.5f},
            {0.5f, 0.5f, 0.5f},  {-0.5f, 0.5f, 0.5f}
            };

            m.normals = {
            { 0, 0,-1}, { 0, 0, 1},
            {-1, 0, 0}, { 1, 0, 0},
            { 0,-1, 0}, { 0, 1, 0}
            };

            m.uvs = {{0,0},{1,0},{1,1},{0,1}};

            // BACK (-Z)
            m.faces.push_back({{0,3,0},{2,1,0},{1,2,0}});
            m.faces.push_back({{0,3,0},{3,0,0},{2,1,0}});

            // FRONT (+Z)
            m.faces.push_back({{4,0,1},{5,1,1},{6,2,1}});
            m.faces.push_back({{4,0,1},{6,2,1},{7,3,1}});

            // LEFT
            m.faces.push_back({{0,1,2},{7,2,2},{3,3,2}});
            m.faces.push_back({{0,1,2},{4,0,2},{7,2,2}});

            // RIGHT
            m.faces.push_back({{1,0,3},{6,2,3},{5,1,3}});
            m.faces.push_back({{1,0,3},{2,3,3},{6,2,3}});

            // BOTTOM
            m.faces.push_back({{0,0,4},{5,2,4},{4,1,4}});
            m.faces.push_back({{0,0,4},{1,3,4},{5,2,4}});

            // TOP
            m.faces.push_back({{3,0,5},{6,2,5},{2,1,5}});
            m.faces.push_back({{3,0,5},{7,3,5},{6,2,5}});

            return m;
        }

        Model3D sphere(float radius, uint32_t sectors, uint32_t stacks) {
            Model3D m;

            for (uint32_t i = 0; i <= stacks; i++) {
                float v = float(i) / float(stacks);
                float phi = v * glm::pi<float>();

                float y = cos(phi);
                float r = sin(phi);

                for (uint32_t j = 0; j <= sectors; j++) {
                    float u = float(j) / float(sectors);
                    float theta = u * 2.f * glm::pi<float>();

                    glm::vec3 p{
                        r * sin(theta),   // X
                        y,                // Y
                        -r * cos(theta)   // Z = IMPORTANT (-Z forward alignment)
                    };

                    m.vertices.push_back(p * radius);
                    m.normals.push_back(glm::normalize(p));
                    m.uvs.emplace_back(u, v);
                }
            }

            for (uint32_t i = 0; i < stacks; i++) {
                for (uint32_t j = 0; j < sectors; j++) {

                    int a = i * (sectors + 1) + j;
                    int b = a + 1;
                    int c = (i + 1) * (sectors + 1) + j;
                    int d = c + 1;

                    if (i != 0)
                        m.faces.push_back({{a,a,a},{b,b,b},{c,c,c}});  // swap b and c

                    if (i != stacks - 1)
                        m.faces.push_back({{b,b,b},{d,d,d},{c,c,c}});  // swap c and d
                }
            }

            return m;
        }

        Model3D grid(float width, float depth, uint32_t cellsX, uint32_t cellsZ) {
            Model3D m;

            float dx = width / cellsX;
            float dz = depth / cellsZ;

            for (uint32_t z = 0; z <= cellsZ; z++) {
                for (uint32_t x = 0; x <= cellsX; x++) {
                    m.vertices.emplace_back(
                        -width * 0.5f + x * dx,
                        0.f,
                        -depth * 0.5f + z * dz
                    );
                    m.normals.push_back({0,1,0});
                    m.uvs.emplace_back(
                        float(x)/cellsX,
                        float(z)/cellsZ
                    );
                }
            }

            for (uint32_t z = 0; z < cellsZ; z++) {
                for (uint32_t x = 0; x < cellsX; x++) {

                    int a = z * (cellsX + 1) + x;
                    int b = a + 1;
                    int c = (z + 1) * (cellsX + 1) + x;
                    int d = c + 1;

                    // CCW
                    m.faces.push_back({V(a,a,0), V(c,c,0), V(b,b,0)});
                    m.faces.push_back({V(b,b,0), V(c,c,0), V(d,d,0)});
                }
            }

            return m;
        }

        Model3D cylinder(float radius, float height, uint32_t sectors) {
            Model3D m;

            float h = height * 0.5f;
            float step = 2.f * glm::pi<float>() / sectors;

            // side vertices
            for (int y = 0; y < 2; y++) {
                float yy = y == 0 ? -h : h;

                for (uint32_t j = 0; j <= sectors; j++) {
                    float t = j * step;

                    glm::vec3 n{cos(t), 0, sin(t)};

                    m.vertices.emplace_back(n.x * radius, yy, n.z * radius);
                    m.normals.push_back(n);
                    m.uvs.emplace_back(float(j)/sectors, float(y));
                }
            }

            // sides
            for (uint32_t j = 0; j < sectors; j++) {
                int a = j;
                int b = j + 1;
                int c = j + sectors + 1;
                int d = c + 1;

                m.faces.push_back({V(a,a,a), V(c,c,c), V(b,b,b)});
                m.faces.push_back({V(b,b,b), V(c,c,c), V(d,d,d)});
            }

            // caps helper
            auto cap = [&](float y, glm::vec3 normal, bool flip) {
                int center = m.vertices.size();
                m.vertices.emplace_back(0,y,0);
                m.normals.push_back(normal);
                m.uvs.emplace_back(0.5,0.5);

                int start = m.vertices.size();

                for (uint32_t j = 0; j <= sectors; j++) {
                    float t = j * step;

                    m.vertices.emplace_back(cos(t)*radius, y, sin(t)*radius);
                    m.normals.push_back(normal);
                    m.uvs.emplace_back(cos(t)*0.5f+0.5f, sin(t)*0.5f+0.5f);
                }

                for (uint32_t j = 0; j < sectors; j++) {
                    int a = start + j;
                    int b = start + j + 1;

                    if (!flip)
                        m.faces.push_back({V(center,center,center), V(b,b,b), V(a,a,a)});
                    else
                        m.faces.push_back({V(center,center,center), V(a,a,a), V(b,b,b)});
                }
            };

            cap(-h, {0,-1,0}, true);
            cap( h, {0, 1,0}, false);

            return m;
        }

        Model3D cone(float radius, float height, uint32_t sectors) {
            Model3D m;

            float h = height * 0.5f;
            float step = 2.f * glm::pi<float>() / sectors;

            // Apex
            int apex = m.vertices.size();
            m.vertices.emplace_back(0, h, 0);
            m.normals.push_back({0,1,0});
            m.uvs.emplace_back(0.5f, 1.f);

            // Side rim vertices
            for (uint32_t j = 0; j <= sectors; j++) {
                float t = j * step;
                glm::vec3 p(cos(t)*radius, -h, sin(t)*radius);
                glm::vec3 n = glm::normalize(glm::vec3(cos(t), radius/height, sin(t)));
                m.vertices.push_back(p);
                m.normals.push_back(n);
                m.uvs.emplace_back(float(j)/sectors, 0.f);
            }

            // Sides
            for (uint32_t j = 0; j < sectors; j++) {
                int a = 1 + j;
                int b = 1 + j + 1;
                m.faces.push_back({V(apex,apex,apex), V(b,b,b), V(a,a,a)});
            }

            // Bottom cap
            int capCenter = m.vertices.size();
            m.vertices.emplace_back(0, -h, 0);
            m.normals.push_back({0,-1,0});
            m.uvs.emplace_back(0.5f, 0.5f);

            int capStart = m.vertices.size();
            for (uint32_t j = 0; j <= sectors; j++) {
                float t = j * step;
                m.vertices.emplace_back(cos(t)*radius, -h, sin(t)*radius);
                m.normals.push_back({0,-1,0});
                m.uvs.emplace_back(cos(t)*0.5f+0.5f, sin(t)*0.5f+0.5f);
            }

            for (uint32_t j = 0; j < sectors; j++) {
                int a = capStart + j;
                int b = capStart + j + 1;
                m.faces.push_back({V(capCenter,capCenter,capCenter), V(a,a,a), V(b,b,b)});
            }

            return m;
        }

        Model3D quad(float w, float h) {
            Model3D m;

            float hw = w * 0.5f;
            float hh = h * 0.5f;

            m.vertices = {
            {-hw,-hh,0}, {hw,-hh,0},
            {hw, hh,0},  {-hw,hh,0}
            };

            m.normals = {{0,0,1}};
            m.uvs = {{0,0},{1,0},{1,1},{0,1}};

            m.faces.push_back({V(0,0,0), V(2,0,0), V(1,0,0)});
            m.faces.push_back({V(0,0,0), V(3,0,0), V(2,0,0)});

            return m;
        }

    } // namespace Primitives

}