// TriangulationD.cpp: определяет точку входа для приложения.
//

#define _CRT_SECURE_NO_WARNINGS
#include "TriangulationD.h"

char *logfile, *inputfile, *outputfile;

struct Nodes
{
    double x; //координата X
    double y; //координата Y
};
struct Triangles
{
    int nodes[3]; //узлы треугольника
    int triangles[3]; //сосоедние треугольники
};


enum Result
{
    SUCCESS = 0,
    ERROR_OPENING_FILE = -1,
    ERROR_READING_FILE = -2,
    ERROR_NOT_SPECIFIED_FILE = -3
};

using namespace std;

void writelog(char* message) {
    ofstream out(logfile, std::ios::app);
    out << message << endl;
    out.close();
}

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

void writefile(int N, int M, vector<Nodes>& AllDataNodes, vector<Triangles> AllDataTriangles) {
    ofstream out(outputfile);
    out << "# vtk DataFile Version 2.0";
    out << endl << "TriangulationD result";
    out << endl << "ASCII";
    out << endl << endl << "DATASET POLYDATA";
    out << endl << "POINTS " << N << " float" << endl;
    for (int i = 0; i < N; i++) {
        out << AllDataNodes[i].x << " " << AllDataNodes[i].y << " " << 0 << endl;
    }
    out.close();
}

bool dis(Nodes a, Nodes b) {
    return sqrt(pow(a.x, 2) + pow(a.y, 2)) < sqrt(pow(b.x, 2) + pow(b.y, 2));
}

Triangles SuperstructureSet(vector<Nodes>& Superstructure) {
    Triangles triSuper;
    Nodes triPoint;
    int N = Superstructure.size();

    sort(Superstructure.begin(), Superstructure.end(), &dis); //сортируем по удалённости

    vector<Nodes> maxPoint(4); //квадрат, покрывающий все точки 

    maxPoint[0].x = abs(Superstructure[N - 1].x);
    maxPoint[0].y = abs(Superstructure[N - 1].y);
    maxPoint[1].x = abs(Superstructure[N - 1].x);
    maxPoint[1].y = -abs(Superstructure[N - 1].y);
    maxPoint[2].x = -abs(Superstructure[N - 1].x);
    maxPoint[2].y = -abs(Superstructure[N - 1].y);
    maxPoint[3].x = -abs(Superstructure[N - 1].x);
    maxPoint[3].y = abs(Superstructure[N - 1].y);

    double ribLength = sqrt(pow(maxPoint[0].x - maxPoint[1].x, 2) + pow(maxPoint[0].y - maxPoint[1].y, 2));

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
        triSuper.triangles[i] = 0;
        triSuper.nodes[i] = N + i;
    }

    return triSuper;
}

void TriangulationD(int N, int M, vector<Nodes>& AllDataNodes, vector<Triangles> AllDataTriangles) {
    Nodes midPoint = { 0,0 }; //средняя точка
    for (int i = 0; i < N; i++) {
        midPoint.x += AllDataNodes[i].x;
        midPoint.y += AllDataNodes[i].y;
    }
    midPoint.x = midPoint.x / N;
    midPoint.y = midPoint.y / N;

    for (int i = 0; i < N; i++) {
        AllDataNodes[i].x -= midPoint.x;
        AllDataNodes[i].y -= midPoint.y;
    } //новая система координат

    Triangles triangLine;
    triangLine = SuperstructureSet(AllDataNodes); //задание суперстуктуры - треугольник
    AllDataTriangles.push_back(triangLine);


}

int main(int argc, char* argv[])
{
    vector<Nodes> AllDataNodes;
    vector<Triangles> AllDataTriangles;
    int N, M = 2; //N - количкество точек

    //if (argc == 1) {
    //    printf("The input file is undefined\nTerminate\n"); exit(ERROR_NOT_SPECIFIED_FILE);
    //}

    if (argc == 4) {
        logfile = argv[3];

        ofstream out(logfile); out.close();

        writelog("The log file is defined");
        printf("The log file is defined\n");
    }

    //inputfile = argv[1];
    inputfile = "read.dat";
    N = readflie(M, AllDataNodes, argc); //читает файл и записывает в AllDataNodes, возврашает число точек внешнего конутра
    if (argc == 4) writelog("Mesh loaded into memory");
    printf("Mesh loaded into memory\n");
    N = AllDataNodes.size(); //общее число точек

    TriangulationD(N, M, AllDataNodes, AllDataTriangles);

    if (argc > 2) {
        outputfile = argv[2];
        N = AllDataNodes.size();
        writefile(N, M, AllDataNodes, AllDataTriangles); //записывает в файл полученную сетку
        if (argc == 4) writelog("Mesh is written to the output file");
        printf("Mesh is written to the output file\n");
    }

    return 0;
}