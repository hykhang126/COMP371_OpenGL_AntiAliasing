#include "Cloth.h"

#define WITH_SPHERE

// Initialize a vertex array for a variable-sized cloth with x and y texture coordinates
std::vector<Particle> Cloth::initializeClothVertexArray(int clothWidth, int clothHeight) {
    std::vector<Particle> clothVertices;

    for (int i = 0; i < clothHeight; ++i) {
        for (int j = 0; j < clothWidth; ++j) {
            float x = static_cast<float>(j) / static_cast<float>(clothWidth - 1);
            float y = static_cast<float>(i) / static_cast<float>(clothHeight - 1);

            Particle particle;
            particle.position = glm::vec3(x, y, -1.0f);
            particle.textureCoordinates = glm::vec2(x, y);
            particle.normal = glm::vec3(0.0f, 0.0f, 0.0f);

            clothVertices.push_back(particle);
        }
    }

    // Normals
    for (int i = 0; i < clothHeight - 1; ++i) {
        for (int j = 0; j < clothWidth - 1; ++j) {
            // Indices of the current quad
            int index1 = i * clothWidth + j;
            int index2 = index1 + 1;
            int index3 = (i + 1) * clothWidth + j;
            int index4 = index3 + 1;

            // Compute face normals for two triangles forming the quad
            glm::vec3 normal1 = glm::cross(clothVertices[index2].position - clothVertices[index1].position,
                clothVertices[index3].position - clothVertices[index1].position);
            glm::vec3 normal2 = glm::cross(clothVertices[index2].position - clothVertices[index3].position,
                clothVertices[index4].position - clothVertices[index3].position);

            // Accumulate normals to each vertex
            clothVertices[index1].normal += normal1 + normal2;
            clothVertices[index2].normal += normal1 + normal2;
            clothVertices[index3].normal += normal1 + normal2;
            clothVertices[index4].normal += normal1 + normal2;
        }
    }

    // Normalize normals
    for (auto& particle : clothVertices) {
        particle.normal = glm::normalize(particle.normal);
    }

    return clothVertices;
}

std::vector<unsigned int> Cloth::generateIndices(int clothWidth, int clothHeight) {
    std::vector<unsigned int> indices;

    for (int i = 0; i < clothHeight - 1; ++i) {
        for (int j = 0; j < clothWidth - 1; ++j) {
            // Indices for the first triangle in the quad
            unsigned int topLeft = i * clothWidth + j;
            unsigned int topRight = i * clothWidth + j + 1;
            unsigned int bottomLeft = (i + 1) * clothWidth + j;

            indices.push_back(topLeft);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);

            // Indices for the second triangle in the quad
            unsigned int bottomRight = (i + 1) * clothWidth + j + 1;

            indices.push_back(topRight);
            indices.push_back(bottomRight);
            indices.push_back(bottomLeft);
        }
    }

    return indices;
}

Spring Cloth::initializeSpring(Particle& particle1, Particle& particle2, float ks, float kd)
{
    Spring spring;
    spring.particle1 = &particle1;
    spring.particle2 = &particle2;
    spring.ks = ks;
    spring.kd = kd;
    glm::vec3 deltaP = particle1.position - particle2.position;
    spring.restLength = std::sqrt(glm::dot(deltaP, deltaP));
    return spring;
}

void Cloth::calculateForces(std::vector<Particle>& particles, std::map<Particle*, glm::vec3>& velocities, std::map<Particle*, glm::vec3>& forces, std::vector<Spring> springs, float& windforce, std::vector<Particle>& sphereVertices)
{
    // Resets forces
    for (int i = 0; i < particles.size(); i++)
    {
        glm::vec3 zeroes(0);
        forces[&particles[i]] = zeroes;
    }

    // Adds gravity and dampening force
    for (int i = 0; i < particles.size(); i++)
    {
        // Adds gravity
        forces[&particles[i]] += gravity;

        // Adds damping of velocity
        forces[&particles[i]] += DAMPING * velocities[&particles[i]];
    }

    // Wind
    glm::vec3 wind(0.0, 0, 0.1); // Example wind direction and strength
    float windCoefficient = windforce; // Adjust this as needed
    for (int i = 0; i < particles.size(); i++)
    {
        forces[&particles[i]] += wind * windCoefficient;
    }

#ifdef WITH_SPHERE
    this->checkIntersection(particles, sphereVertices, velocities);
#endif

    // Springs
    for (int i = 0; i < springs.size(); i++)
    {
        // Particles
        Particle& particle1 = *springs[i].particle1;
        Particle& particle2 = *springs[i].particle2;

        // Distance
        glm::vec3 deltaP = particle1.position - particle2.position;
        float dist = glm::length(deltaP);

        // Difference in velocities
        glm::vec3 deltaV = velocities[&particle1] - velocities[&particle2];

        float f1 = -springs[i].ks * (dist - springs[i].restLength);
        float f2 = springs[i].kd * (glm::dot(deltaV, deltaP) / dist);
        glm::vec3 springForce = (f1 + f2) * glm::normalize(deltaP);

        // Equal and opposite forces
        forces[&particle1] += springForce;
        forces[&particle2] -= springForce;
    }

    int CLOTH_WIDTH = 10, CLOTH_HEIGHT = 10;

    // Reset the normals
    for (int i = 0; i < particles.size(); i++)
    {
        particles[i].normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // Sets normals
    for (int i = 0; i < CLOTH_WIDTH - 1; ++i) {
        for (int j = 0; j < CLOTH_HEIGHT - 1; ++j) {
            // Indices of the current quad
            int index1 = i * CLOTH_WIDTH + j;
            int index2 = index1 + 1;
            int index3 = (i + 1) * CLOTH_WIDTH + j;
            int index4 = index3 + 1;

            // Compute face normals for two triangles forming the quad
            glm::vec3 normal1 = glm::cross(particles[index2].position - particles[index1].position,
                particles[index3].position - particles[index1].position);
            glm::vec3 normal2 = glm::cross(particles[index2].position - particles[index3].position,
                particles[index4].position - particles[index3].position);

            // Accumulate normals to each vertex
            particles[index1].normal += normal1 + normal2;
            particles[index2].normal += normal1 + normal2;
            particles[index3].normal += normal1 + normal2;
            particles[index4].normal += normal1 + normal2;
        }
    }

    // Normalize normals
    for (auto& particle : particles) {
        particle.normal = glm::normalize(particle.normal);
    }


}

void Cloth::inverseSpringForce(std::vector<Particle>& particles, std::map<Particle*, glm::vec3>& velocities, std::map<Particle*, glm::vec3>& forces, std::vector<Spring> springs)
{
    for (int i = 0; i < springs.size(); i++)
    {
        // Particles of spring i
        Particle& particle1 = *springs[i].particle1;
        Particle& particle2 = *springs[i].particle2;

        glm::vec3 deltaP = particle1.position - particle2.position;

        float dist = glm::length(deltaP);

        if (dist > springs[i].restLength)
        {
            dist -= (springs[i].restLength);
            dist /= 2.0f;
            deltaP = glm::normalize(deltaP);
            deltaP *= dist;

            // Top vertices of cloth
            velocities[&particle1] -= deltaP;
            velocities[&particle2] += deltaP;
        }
    }
}

void Cloth::semiImplicit(std::vector<Particle>& particles, std::map<Particle*, glm::vec3>& velocities, std::map<Particle*, glm::vec3> forces, float mass)
{
    for (int i = 0; i < particles.size(); i++)
    {
        float timeMass = timestep / mass;
        velocities[&particles[i]] += forces[&particles[i]] * (timeMass);
        particles[i].position += timestep * velocities[&particles[i]];
    }
}

void Cloth::checkIntersection(std::vector<Particle>& particles, std::vector<Particle>& sphereVertices, std::map<Particle*, glm::vec3>& velocities)
{
    for (int i = 0; i < particles.size(); i++)
    {
        for (int j = 0; j < sphereVertices.size(); j++)
        {
            glm::vec3 deltaP = particles[i].position - sphereVertices[j].position;
            float dist = glm::length(deltaP);
            glm::vec3 zeroes(-particles[i].position.x / 30, -particles[i].position.y / 30, -particles[i].position.z / 30);

            if (dist < 0.06)
                velocities[&particles[i]] = zeroes;
        }
    }
}