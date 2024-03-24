#pragma once

#include <Eigen/Dense>
#include <vector>
#include "glm/glm.hpp"
#include <iostream>
#include <map>

// Simulation Modifiers
struct Particle {
    glm::vec3 position;
    glm::vec2 textureCoordinates;
    glm::vec3 normal;
};

struct Spring {
    Particle* particle1;
    Particle* particle2;
    float restLength;
    float ks;
    float kd;
};

class Cloth {

public:
    // Forces
    glm::vec3 gravity = glm::vec3(0.0f, -0.00980665f, 0.0f);

    // Timestep
    float timestep = 1 / 20.0f;
    float DAMPING = -0.0125f;

    std::vector<Particle> initializeClothVertexArray(int clothWidth, int clothHeight);
    std::vector<unsigned int> generateIndices(int clothWidth, int clothHeight);
    Spring initializeSpring(Particle& particle1, Particle& particle2, float ks, float kd);
    void calculateForces(std::vector<Particle>& particles, std::map<Particle*, glm::vec3>& velocities, std::map<Particle*, glm::vec3>& forces, std::vector<Spring> springs, float& windforce, std::vector<Particle>& sphereVertices);
    void inverseSpringForce(std::vector<Particle>& particles, std::map<Particle*, glm::vec3>& velocities, std::map<Particle*, glm::vec3>& forces, std::vector<Spring> springs);
    void semiImplicit(std::vector<Particle>& particles, std::map<Particle*, glm::vec3>& velocities, std::map<Particle*, glm::vec3> forces, float mass);
    void checkIntersection(std::vector<Particle>& particles, std::vector<Particle>& sphereVertices, std::map<Particle*, glm::vec3>& forces);

};