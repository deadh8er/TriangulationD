// TriangulationD.cpp: определяет точку входа для приложения.
//

#define _CRT_SECURE_NO_WARNINGS
#include "TriangulationD.h"

char *logfile, *inputfile, *outputfile;

struct Nodes
{
    double x; // Координата X
    double y; // Координата Y
};
struct Triangles
{
    int nodes[3]; // Узлы треугольника
    int triangles[3]; // Соседние треугольники (если значение равно -1, то соседа нет)
};


enum Result
{
    SUCCESS = 0,
    ERROR_OPENING_FILE = -1,
    ERROR_READING_FILE = -2,
    ERROR_NOT_SPECIFIED_FILE = -3
};

using namespace std;


// Запись лога
void writelog(char* message) {
    ofstream out(logfile, std::ios::app);
    out << message << endl;
    out.close();
}


// Читает файл и записывает точки в AllDataNodes, возвращает количество точек
int readflie(int M, vector<Nodes>& AllDataNodes, int argc) {
    string s;
    int circuit = 0;
    int i = 0;
    ifstream cin(inputfile);
    if (!cin) {
        if (argc == 4) writelog("Can't open input file\nTerminate");
        printf("Can't open input file\nTerminate\n"); exit(ERROR_OPENING_FILE);
    }

    while (getline(cin, s)) {
        if (s == "EXTERNAL CIRCUIT") {
            circuit = 1;
            continue;
        }
        else if (s == "INTERNAL CIRCUIT") {
            circuit = 2;
            continue;
        }
        if (circuit == 1) i++;
        Nodes lineData;
        sscanf(s.c_str(), "%lf %lf", &lineData.x, &lineData.y);
        AllDataNodes.push_back(lineData);
    }
    cin.close();
    return i;
}


// Записывает в файл полученную сетку
void writefile(int N, int M, vector<Nodes>& AllDataNodes, vector<Triangles>& AllDataTriangles) {
    ofstream out(outputfile);
    out << "# vtk DataFile Version 2.0";
    out << endl << "TriangulationD result";
    out << endl << "ASCII";
    out << endl << endl << "DATASET POLYDATA";
    out << endl << "POINTS " << N << " float" << endl;
    for (int i = 0; i < N; i++) {
        out << AllDataNodes[i].x << " " << AllDataNodes[i].y << " " << 0 << endl;
    }
    out << endl << "LINES " << AllDataTriangles.size() << " " << AllDataTriangles.size() * 4 << endl;
    for (int i = 0; i < AllDataTriangles.size(); i++) {
        out << AllDataTriangles[i].nodes[0] << " " << AllDataTriangles[i].nodes[1] << " " << AllDataTriangles[i].nodes[2] << " " << AllDataTriangles[i].nodes[0] << endl;
    }
    out.close();
}


// Какая из точек лежит дальше относительно центра?
bool dis(Nodes a, Nodes b) {
    return sqrt(pow(a.x, 2) + pow(a.y, 2)) < sqrt(pow(b.x, 2) + pow(b.y, 2));
}

// Задание суперструктуры - треугольник
Triangles SuperstructureSet(vector<Nodes>& Superstructure) {
    Triangles triSuper;
    Nodes triPoint;
    int N = Superstructure.size();

    sort(Superstructure.begin(), Superstructure.end(), &dis); // Сортируем по удалённости

    vector<Nodes> maxPoint(4); // Квадрат, покрывающий все точки 

    maxPoint[0].x = abs(Superstructure[N - 1].x);
    maxPoint[0].y = abs(Superstructure[N - 1].y);
    maxPoint[1].x = abs(Superstructure[N - 1].x);
    maxPoint[1].y = -abs(Superstructure[N - 1].y);
    maxPoint[2].x = -abs(Superstructure[N - 1].x);
    maxPoint[2].y = -abs(Superstructure[N - 1].y);
    maxPoint[3].x = -abs(Superstructure[N - 1].x);
    maxPoint[3].y = abs(Superstructure[N - 1].y);
    
    double ribLength = sqrt(pow(maxPoint[0].x - maxPoint[1].x, 2) + pow(maxPoint[0].y - maxPoint[1].y, 2)); // Длина ребра квадрата

    // Треугольник по вписанной окружности, описанной для квадрата
    triPoint.x = ribLength * (1 - 3 * sqrt(2)) / 2;
    triPoint.y = ribLength * (1 - 3 ) / 2;
    Superstructure.push_back(triPoint);

    triPoint.x = ribLength * (1 + 3 * sqrt(2)) / 2;
    triPoint.y = ribLength * (1 - 3) / 2;
    Superstructure.push_back(triPoint);

    triPoint.x = ribLength / 2;
    triPoint.y = ribLength * (1 + 3 * sqrt(3)) / 2;
    Superstructure.push_back(triPoint);

    for (int i = 0; i < 3; i++) {
        triSuper.triangles[i] = -1;
        triSuper.nodes[i] = N + i;
    }

    return triSuper; // Суперструктура
}

// Вычисляет положение точки D относительно прямой AB
double g(Nodes a, Nodes b, Nodes d) {
    return (d.x - a.x) * (b.y - a.y) - (d.y - a.y) * (b.x - a.x);
}

// Лежат ли точки C и D с одной стороны прямой (AB)?
bool f(Nodes a, Nodes b, Nodes c, Nodes d) {
    return g(a, b, c) * g(a, b, d) >= 0;
}


// Записывает соседние треугольники для треугольника k
void ComparisonTriangles(int k, vector<Triangles>& AllDataTriangles) {
    AllDataTriangles[k].triangles[0] = -1;
    AllDataTriangles[k].triangles[1] = -1;
    AllDataTriangles[k].triangles[2] = -1;
    int counter[4] = { 0,1,2,0 };
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < AllDataTriangles.size(); i++) {
            for (int n = 0; n < 3; n++) {
                if ((AllDataTriangles[k].nodes[counter[j]] == AllDataTriangles[i].nodes[counter[n]] && AllDataTriangles[k].nodes[counter[j + 1]] == AllDataTriangles[i].nodes[counter[n + 1]]) || (AllDataTriangles[k].nodes[counter[j + 1]] == AllDataTriangles[i].nodes[counter[n]] && AllDataTriangles[k].nodes[counter[j]] == AllDataTriangles[i].nodes[counter[n + 1]])) {
                    if (i == k) break;
                    AllDataTriangles[k].triangles[j] = i;
                }
            }
        }
    }
}

// Разбиение треугольника на три. m - номер добавляемой точки, k - номер треугольника, которому принадлежит точка.
void NewTriangles(int m, int k, vector<Nodes>& AllDataNodes, vector<Triangles>& AllDataTriangles) {
    Triangles triangLine;
    triangLine.nodes[0] = AllDataTriangles[k].nodes[0];
    triangLine.nodes[1] = AllDataTriangles[k].nodes[1];
    triangLine.nodes[2] = m;
    AllDataTriangles.push_back(triangLine);

    triangLine.nodes[0] = AllDataTriangles[k].nodes[0];
    triangLine.nodes[1] = m;
    triangLine.nodes[2] = AllDataTriangles[k].nodes[2];
    AllDataTriangles.push_back(triangLine);

    triangLine.nodes[0] = m;
    triangLine.nodes[1] = AllDataTriangles[k].nodes[1];
    triangLine.nodes[2] = AllDataTriangles[k].nodes[2];
    AllDataTriangles.push_back(triangLine);

    AllDataTriangles.erase(AllDataTriangles.begin() + k);

    for (int i = AllDataTriangles.size() - 1; i >= 0; i--) {
        ComparisonTriangles(i, AllDataTriangles);
    }
}

// Построение триангуляции алгоритмом «Удаляй и строй» 
void TriangulationD(int N, int M, vector<Nodes>& AllDataNodes, vector<Triangles>& AllDataTriangles) {
    Nodes midPoint = { 0,0 }; // Средняя точка
    for (int i = 0; i < N; i++) {
        midPoint.x += AllDataNodes[i].x;
        midPoint.y += AllDataNodes[i].y;
    }
    midPoint.x = midPoint.x / N;
    midPoint.y = midPoint.y / N;

    // Смещение системы координат
    for (int i = 0; i < N; i++) {
        AllDataNodes[i].x -= midPoint.x;
        AllDataNodes[i].y -= midPoint.y;
    }

    Triangles triangLine;
    triangLine = SuperstructureSet(AllDataNodes); // Задание суперстуктуры - треугольник
    AllDataTriangles.push_back(triangLine);

    for (int i = 0; i < N; i++) {
        if (i != 0) {
            bool belongPoint = false; // Принадлежит ли точка треугольнику?

            for (int j = 0; j < AllDataTriangles.size(); j++) {
                Nodes pt1 = AllDataNodes[AllDataTriangles[j].nodes[0]];
                Nodes pt2 = AllDataNodes[AllDataTriangles[j].nodes[1]];
                Nodes pt3 = AllDataNodes[AllDataTriangles[j].nodes[2]];
                belongPoint = f(pt1, pt2, pt3, AllDataNodes[i]) && f(pt2, pt3, pt1, AllDataNodes[i]) && f(pt3, pt1, pt2, AllDataNodes[i]);
                if (belongPoint) {
                    NewTriangles(i, j, AllDataNodes, AllDataTriangles);
                    break;
                }
            }
        }
        else {
            triangLine.nodes[0] = AllDataTriangles[0].nodes[0];
            triangLine.nodes[1] = AllDataTriangles[0].nodes[1];
            triangLine.nodes[2] = 0;
            triangLine.triangles[0] = -1;
            triangLine.triangles[1] = 1;
            triangLine.triangles[2] = 2;
            AllDataTriangles.push_back(triangLine);

            triangLine.nodes[0] = AllDataTriangles[0].nodes[0];
            triangLine.nodes[1] = 0;
            triangLine.nodes[2] = AllDataTriangles[0].nodes[2];
            triangLine.triangles[0] = 0;
            triangLine.triangles[1] = -1;
            triangLine.triangles[2] = 2;
            AllDataTriangles.push_back(triangLine);

            triangLine.nodes[0] = 0;
            triangLine.nodes[1] = AllDataTriangles[0].nodes[1];
            triangLine.nodes[2] = AllDataTriangles[0].nodes[2];
            triangLine.triangles[0] = 0;
            triangLine.triangles[1] = 1;
            triangLine.triangles[2] = -1;
            AllDataTriangles.push_back(triangLine);

            AllDataTriangles.erase(AllDataTriangles.begin()); // Удаление старого треугольника
        }

    }
}

int main(int argc, char* argv[])
{
    vector<Nodes> AllDataNodes;
    vector<Triangles> AllDataTriangles;
    int N, M = 2; // N - количество точек в файле

    if (argc == 1) {
        printf("The input file is undefined\nTerminate\n"); exit(ERROR_NOT_SPECIFIED_FILE);
    }

    if (argc == 4) {
        logfile = argv[3];

        ofstream out(logfile); out.close();

        writelog("The log file is defined");
        printf("The log file is defined\n");
    }

    inputfile = argv[1];
    //inputfile = "read.dat";
    N = readflie(M, AllDataNodes, argc);
    if (argc == 4) writelog("Mesh loaded into memory");
    printf("Mesh loaded into memory\n");
    N = AllDataNodes.size();

    TriangulationD(N, M, AllDataNodes, AllDataTriangles);

    if (argc > 2) {
        outputfile = argv[2];
        N = AllDataNodes.size();
        writefile(N, M, AllDataNodes, AllDataTriangles);
        if (argc == 4) writelog("Mesh is written to the output file");
        printf("Mesh is written to the output file\n");
    }

    return 0;
}