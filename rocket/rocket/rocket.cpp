#include <iostream>
#include <fstream>
#include <cmath>

const double pi = 3.1415;
const double dt = 0.01;
const double g = 9.81;

class Environment {
public:
    double G, m_e, R_e, rho0, T, R, m_air;

    Environment(double G, double m_e, double R_e, double rho0, double T, double R, double m_air)
        : G(G), m_e(m_e), R_e(R_e), rho0(rho0), T(T), R(R), m_air(m_air) {}

    double computeAirDensity(double y) const {
        
        return rho0 * exp((-m_air * g * y) / (R * T));
    }
};

class FlyingObject {
public:
    double mass, radius, shape_coeff, x, y, vx, vy, M, U0, alpha;

    FlyingObject(double mass, double radius, double shape_coeff, double v0, double alpha0, double M, double U0)
        : mass(mass), radius(radius), shape_coeff(shape_coeff), M(M), U0(U0), alpha(alpha0) {
        vx = v0 * cos(alpha0);
        vy = v0 * sin(alpha0);
        x = 0;
        y = 0;
    }

    void computeForces(double vx, double vy, double y, double& ax, double& ay, const Environment& env) {
        double rho = env.computeAirDensity(y);
        

        ax = (-rho * pow(radius, 2) * pi * shape_coeff * vx + M * U0 * cos(alpha)) / mass;
        ay = (-g * mass - rho * pow(radius, 2) * pi * shape_coeff * vy + M * U0 * sin(alpha)) / mass;
    }
};

void updateMotion(FlyingObject& obj, const Environment& env, double& t, std::ofstream& file) {
    while (true) {
        double k1vx, k2vx, k3vx, k4vx;
        double k1vy, k2vy, k3vy, k4vy;
        double k1x, k2x, k3x, k4x;
        double k1y, k2y, k3y, k4y;
        double ax, ay;

        obj.computeForces(obj.vx, obj.vy, obj.y, ax, ay, env);

        k1vx = dt * ax;
        k1vy = dt * ay;
        k1x = dt * obj.vx;
        k1y = dt * obj.vy;

        obj.computeForces(obj.vx + k1vx / 2, obj.vy + k1vy / 2, obj.y + k1y / 2, ax, ay, env);
        k2vx = dt * ax;
        k2vy = dt * ay;
        k2x = dt * (obj.vx + k1vx / 2);
        k2y = dt * (obj.vy + k1vy / 2);

        obj.computeForces(obj.vx + k2vx / 2, obj.vy + k2vy / 2, obj.y + k2y / 2, ax, ay, env);
        k3vx = dt * ax;
        k3vy = dt * ay;
        k3x = dt * (obj.vx + k2vx / 2);
        k3y = dt * (obj.vy + k2vy / 2);

        obj.computeForces(obj.vx + k3vx, obj.vy + k3vy, obj.y + k3y, ax, ay, env);
        k4vx = dt * ax;
        k4vy = dt * ay;
        k4x = dt * (obj.vx + k3vx);
        k4y = dt * (obj.vy + k3vy);

        obj.vx += (k1vx + 2 * k2vx + 2 * k3vx + k4vx) / 6;
        obj.vy += (k1vy + 2 * k2vy + 2 * k3vy + k4vy) / 6;
        obj.x += (k1x + 2 * k2x + 2 * k3x + k4x) / 6;
        obj.y += (k1y + 2 * k2y + 2 * k3y + k4y) / 6;

        obj.mass = obj.mass - obj.M * dt;
        obj.alpha = atan2(obj.vy, obj.vx);

        file << t << "\t" << obj.x << "\t" << obj.y << "\n";
        std::cout << "t: " << t << " | x: " << obj.x << " | y: " << obj.y << std::endl;

        if (obj.y < 0) {
            break;
        }
        t += dt;
    }
}

int main() {
    std::setlocale(LC_ALL, "Russian");

    Environment earth(6.67e-11, 5.97e24, 6.37e6, 1.23, 237, 8.31, 0.02897);
    FlyingObject stone(5, 0.2, 0.5, 100, pi / 4, 0.01, 100);

    std::ofstream file("trajectory.txt");
    if (!file) {
        std::cout << "Ошибка открытия файла!" << std::endl;
        return 1;
    }

    double t = 0;
    std::cout << "Запуск на Земле:\n";
    updateMotion(stone, earth, t, file);
    std::cout << "Приземлился в x: " << stone.x << " м за t: " << t << " с\n";

    file.close();
    return 0;
}
