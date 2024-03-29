#include <vector>
#include <array>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>

using std::vector;
using std::array;
using std::sin;
using std::cos;

static vector<vector<double>> matmul(vector<vector<double>> A, vector<vector<double>> B);
static void draw_line(double x1, double y1, double x2, double y2, char* screen, int width, int height);
static void bianary_purmutations(vector<vector<vector<double>>>& verts, int n);

const char CLEAR = ' ', DRAW = '*'; // clear and draw character

int main(void) {
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
        std::cout << "\nMust be greater than 0.\n";
    }
    while (true) {
        std::cout << "Enter screen height: ";
        std::cin >> temp_s;
        temp_i = std::atoi(temp_s.c_str());
        if (temp_i > 0) {
            height = temp_i;
            break;
        }
        std::cout << "\nMust be greater than 0.\n";
    }
    int n;
    // get number of dimensions from user
    while (true) {
        std::cout << "Enter dimension count: ";
        std::cin >> temp_s;
        temp_i = std::atoi(temp_s.c_str());
        if (temp_i >= 3) {
            n = temp_i;
            break;
        }
        std::cout << "\nDimension count must be atleast 3.\n";
    }
    int sleep_time_ms;
    // get sleep time per frame from user
    while (true) {
        std::cout << "Enter sleep time per frame (ms): ";
        std::cin >> temp_s;
        temp_i = std::atoi(temp_s.c_str());
        if (temp_i >= 0) {
            sleep_time_ms = temp_i;
            break;
        }
        std::cout << "\nSleep time can not be negative.\n";
    }
    // get the rotation per frame from the user
    std::cout << "Enter rotation amount per frame (this should be a small number < 1): ";
    std::cin >> temp_s;
    double t = std::atof(temp_s.c_str());


    // calculate camera distance
    double camera_distance = std::sqrt(n/4)/6 + 1.25;
    // initlaize the rotation matrix;
    vector<vector<double>> I(n, vector<double>(n, 0));
    for (int i = 0; i < n; i++) I[i][i] = 1;  
    vector<vector<double>> R = I;
    vector<vector<double>> Rn;
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            Rn = I;
            Rn[i][i] = cos(t);
            Rn[i][j] = -sin(t);
            Rn[j][i] = sin(t);
            Rn[j][j] = cos(t);
            R = matmul(R, Rn);
        }
    }
    vector<vector<vector<double>>> verts; // array of verticies stored as nd vectors / n x 1 matricies
    // generate verticies (+-0.5, +-0.5,..., +-0.5)
    bianary_purmutations(verts, n);
    vector<array<int, 2>> line_indicies; // vector of all indicies to connect to create each line
    // find each index by searching for what pairs make a line of length 1
    for (int i = 0; i < verts.size(); i++) {
        for (int j = i + 1; j < verts.size(); j++) {
            double len = 0;
            for (int k = 0; k < n; k++) {
                len += std::abs(verts[i][k][0] - verts[j][k][0]);
            }
            if (std::round(len) == 1) line_indicies.push_back({i, j});
        }
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
            double z1 = 1;
            double z2 = 1;
            for (int j = 0; j < n - 2; j++) {
                z1 *= (verts[i[0]][2 + j][0] + camera_distance);
                z2 *= (verts[i[1]][2 + j][0] + camera_distance);
            }
            draw_line((verts[i[0]][0][0] / z1 + 1) / 2 * width, (verts[i[0]][1][0] / z1 + 1) / 2 * height, // x1, y1
                      (verts[i[1]][0][0] / z2 + 1) / 2 * width, (verts[i[1]][1][0] /z2 + 1) / 2 * height, // x2, y2
                      screen, width, height);
        }
        // clear the teminal
        std::system("clear");

        // transform verticies
        for (int i = 0; i < verts.size(); i++) {
            verts[i] = matmul(R, verts[i]);
        }
        // print the screen to the terminal
        std::cout << screen << std::flush;

        if (sleep_time_ms > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
        }
    }
    return 0;
}

// matrix multiplication
static vector<vector<double>> matmul(vector<vector<double>> A, vector<vector<double>> B) {
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
static void draw_line(double x1, double y1, double x2, double y2, char* screen, int width, int height) {
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
    return;
}

// appends all n x 1 purmutations of -.5 and .5 to verts
static void bianary_purmutations(vector<vector<vector<double>>>& verts, int n) {
    int p = 1 << n;
    for (int i = 0; i < p; i++) {
        vector<vector<double>> a;
        for (int j = 0; j < n; j++) {
            a.push_back({(double)(i >> j & 1)});
            a.back()[0] -= .5;
        }
        verts.push_back(a);
    }
    return;
}
