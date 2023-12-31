#include <vector>
#include <array>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>

using std::vector;
using std::array;
using std::sin;
using std::cos;

const char CLEAR = ' ', DRAW = '*'; // clear and draw character
const double T = .1; // rotation amount on every axis
const double CAMERA_DISTANCE = 1.25; // distance camera is away from cube

vector<vector<double>> matmul(vector<vector<double>> A, vector<vector<double>> B);
void draw_line(double x1, double y1, double x2, double y2, char* screen, int width, int height);

// rortation matricies multiplied to make the tranformation matrix R that rotates on each axis
vector<vector<double>> Rx = {{cos(T), -sin(T), 0}, {sin(T), cos(T), 0}, {0, 0, 1}};
vector<vector<double>> Ry = {{1, 0, 0}, {0, cos(T), -sin(T)}, {0, sin(T), cos(T)}};
vector<vector<double>> Rz = {{cos(T), 0, sin(T)}, {0, 1, 0}, {-sin(T), 0, cos(T)}};
vector<vector<double>> R = matmul(Rx, matmul(Ry, Rz));

int main(void) {
    vector<vector<vector<double>>> verts; // array of verticies stored as 3d vectors / 3 x 1 matricies
    // generate verticies (+-0.5, +-0.5, +-0.5)
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            for (int k = -1; k <= 1; k += 2) {
                vector<vector<double>> vert = {{double(i) / 2}, {double(j) / 2}, {double(k) / 2}};
                verts.push_back(vert);
            }
        }
    }
    vector<array<int, 2>> line_indicies; // vector of all indicies to connect to create each line
    // find each index by searching for what pairs make a line of length 1
    for (int i = 0; i < verts.size(); i++) {
        for (int j = i + 1; j < verts.size(); j++) {
            int delta_x, delta_y, delta_z;
            delta_x = abs(verts[i][0][0] - verts[j][0][0]);
            delta_y = abs(verts[i][1][0] - verts[j][1][0]);
            delta_z = abs(verts[i][2][0] - verts[j][2][0]);
            if (.99 <= delta_x + delta_y + delta_z && delta_x + delta_y + delta_z <= 1.01) {
                line_indicies.push_back({i, j});
            }
        }
    }
    // get the resulotion from the user
    int width, height;
    std::string temp_s;
    int temp_i;
    while (true) {
        std::cout << "Enter screen width: ";
        std::cin >> temp_s;
        temp_i = std::atoi(temp_s.c_str());
        if (temp_i > 0) {
            width = temp_i;
            break;
        }
        std::cout << "Must be greater than 0.\n";
    }
    while (true) {
        std::cout << "Enter screen height: ";
        std::cin >> temp_s;
        temp_i = std::atoi(temp_s.c_str());
        if (temp_i > 0) {
            height = temp_i;
            break;
        }
        std::cout << "Must be greater than 0.\n";
    }
    // initialize screen
    // screen at (x, y) -> screen[x + (width + 1) * y]
    char* screen = new char[width * height + height];
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            screen[i + (width + 1) * j] = CLEAR;
        }
    }
    for (int i = 0; i < height; i++) {
        screen[width + (width + 1) * i] = '\n';
    }
    while (1) {
        // clear the screen.
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                screen[i + (width + 1) * j] = CLEAR;
            }
        }
        // draw all the lines to the screen
        for (auto i : line_indicies) {
            double z1 = (verts[i[0]][2][0] + CAMERA_DISTANCE);
            double z2 = (verts[i[1]][2][0] + CAMERA_DISTANCE);
            draw_line((verts[i[0]][0][0] / z1 + 1) / 2 * width, (verts[i[0]][1][0] / z1 + 1) / 2 * height, // x1, y1
                      (verts[i[1]][0][0] / z2 + 1) / 2 * width, (verts[i[1]][1][0] /z2 + 1) / 2 * height, // x2, y2
                      screen, width, height);
        }
        // clear the teminal
        std::system("CLS");

        // transform verticies
        for (int i = 0; i < 8; i++) {
            verts[i] = matmul(R, verts[i]);
        }
        // print the screen to the terminal
        std::cout << screen << std::flush;
    }
    return 0;
}

// matrix multiplication
vector<vector<double>> matmul(vector<vector<double>> A, vector<vector<double>> B) {
    int n = A.size();
    int m = B[0].size();
    int common = A[0].size();
    vector<vector<double>> C(n, vector<double>(m, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < common; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

// takes cordnates and draws a line between the points on the m x n screen
// give cordinates as follows: 0 <= x < n, m and 0 <= y < n, m
void draw_line(double x1, double y1, double x2, double y2, char* screen, int width, int height) {
    int step_count = (int)std::round(std::sqrt(std::pow(x2-x1, 2) + std::pow(y2-y1, 2)));
    if (step_count == 0) return;
    double dx = (x2 - x1) / step_count, dy = (y2 - y1) / step_count;
    int x, y;
    for (int i = 0; i <= step_count; i++) {
        x = (int)std::round(x1 + dx * i);
        if (!(0 <= x && x < width)) return;
        y = (int)std::round(y1 + dy * i);
        if (!(0 <= y && y < height)) return;
        screen[x + (width + 1) * y] = DRAW;
    }
}