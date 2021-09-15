// TriangulationD.cpp: определяет точку входа для приложения.
//

#define _CRT_SECURE_NO_WARNINGS
#include "TriangulationD.h"

char  *inputfile, *outputfile, *logfile;

struct Node
{
    double x; // Координата X
    double y; // Координата Y
};
struct Triangle
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
int readfile( vector<Node>& AllDataNodes, int argc) {
    string s;
    int circuit = 0;
    int i = 0;
    ifstream cin(inputfile);
    if (!cin) {
        if (argc >= 4) writelog("Can't open input file\nTerminate");
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
        Node lineData;
        sscanf(s.c_str(), "%lf %lf", &lineData.x, &lineData.y);
        AllDataNodes.push_back(lineData);
    }
    cin.close();
    return i;
}


// Записывает в файл полученную сетку
void writefile(int N, int M, vector<Node>& AllDataNodes, vector<Triangle>& AllDataTriangles) {
    ofstream out(outputfile);
    out << "# vtk DataFile Version 2.0";
    out << endl << "TriangulationD result";
    out << endl << "ASCII";
    out << endl << endl << "DATASET POLYDATA";
    out << endl << "POINTS " << N << " float" << endl;
    for (int i = 0; i < N; i++) {
        out << AllDataNodes[i].x << " " << AllDataNodes[i].y << " " << 0 << endl;
    }
    out << endl << "LINES " << AllDataTriangles.size() << " " << AllDataTriangles.size() * 5 << endl;
    for (int i = 0; i < AllDataTriangles.size(); i++) {
        out << "4 " << AllDataTriangles[i].nodes[0] << " " << AllDataTriangles[i].nodes[1] << " " << AllDataTriangles[i].nodes[2] << " " << AllDataTriangles[i].nodes[0] << endl;
    }
    out.close();
}


// Какая из точек лежит дальше относительно центра?
bool dis(Node a, Node b) {
    return sqrt(pow(a.x, 2) + pow(a.y, 2)) < sqrt(pow(b.x, 2) + pow(b.y, 2));
}

// Задание суперструктуры - треугольник
Triangle SuperstructureSet(vector<Node>& Superstructure) {
    Triangle triSuper;
    Node triPoint;
    int N = Superstructure.size();

    sort(Superstructure.begin(), Superstructure.end(), &dis); // Сортируем по удалённости

    vector<Node> maxPoint(4); // Квадрат, покрывающий все точки 

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
double g(Node a, Node b, Node d) {
    return (d.x - a.x) * (b.y - a.y) - (d.y - a.y) * (b.x - a.x);
}

// Лежат ли точки C и D с одной стороны прямой (AB)?
bool f(Node a, Node b, Node c, Node d) {
    return g(a, b, c) * g(a, b, d) >= 0;
}


// Записывает соседние треугольники для треугольника k
void ComparisonTriangles(int k, vector<Triangle>& AllDataTriangles) {
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

// Находим координаты центра описанной окружности треугольника
Node FindCenterCircle(Triangle tri, vector<Node>& AllDataNodes) {
    double Ax = AllDataNodes[tri.nodes[0]].x;
    double Ay = AllDataNodes[tri.nodes[0]].y;
    double Bx = AllDataNodes[tri.nodes[1]].x;
    double By = AllDataNodes[tri.nodes[1]].y;
    double Cx = AllDataNodes[tri.nodes[2]].x;
    double Cy = AllDataNodes[tri.nodes[2]].y;

    Node Center;
    double D = 2 * (Ax * (By - Cy) + Bx * (Cy - Ay) + Cx * (Ay - By));
    Center.x = ((Ax * Ax + Ay * Ay) * (By - Cy) + (Bx * Bx + By * By) * (Cy - Ay) + (Cx * Cx + Cy * Cy) * (Ay - By)) / D;
    Center.y = ((Ax * Ax + Ay * Ay) * (Cx - Bx) + (Bx * Bx + By * By) * (Ax - Cx) + (Cx * Cx + Cy * Cy) * (Bx - Ax)) / D;

    //Center.x = -0.5 * (Ay * (pow(Bx, 2) + pow(By, 2) - pow(Cx, 2) - pow(Cy, 2)) + By * (pow(Cx, 2) + pow(Cy, 2) - pow(Ax, 2) - pow(Ay, 2)) + Cy * (pow(Ax, 2) + pow(Ay, 2) - pow(Bx, 2) - pow(By, 2))) / (Ax * (By - Cy) + Bx * (Cy - Ay) + Cx * (Ay - By));
    //Center.y = 0.5 * (Ax * (pow(Bx, 2) + pow(By, 2) - pow(Cx, 2) - pow(Cy, 2)) + Bx * (pow(Cx, 2) + pow(Cy, 2) - pow(Ax, 2) - pow(Ay, 2)) + Cx * (pow(Ax, 2) + pow(Ay, 2) - pow(Bx, 2) - pow(By, 2))) / (Ax * (By - Cy) + Bx * (Cy - Ay) + Cx * (Ay - By));

    return Center;
}

// Построение новых треугольников. Критерий: новый узел в описанной окружности. m - номер добавляемой точки, k - номер треугольника, которому принадлежит точка.
void NewTriangles(int m, int k, vector<Node>& AllDataNodes, vector<Triangle>& AllDataTriangles) {
    /*
    {
        outputfile = "test.vtk";
        int N = AllDataNodes.size();
        writefile(N, 2, AllDataNodes, AllDataTriangles);
    }
    */
    vector<int> triDelete = { k }; // Список треугольников, у которых внутрь описанных окружностей попадает новый узел

    for (int i = 0; i < 3; i++) {
        if (AllDataTriangles[k].triangles[i] == -1) continue;
        Node Center = FindCenterCircle(AllDataTriangles[AllDataTriangles[k].triangles[i]], AllDataNodes);
        if (sqrt(pow(AllDataNodes[m].x - Center.x, 2) + pow(AllDataNodes[m].y - Center.y, 2)) < sqrt(pow(AllDataNodes[AllDataTriangles[AllDataTriangles[k].triangles[i]].nodes[0]].x - Center.x, 2) + pow(AllDataNodes[AllDataTriangles[AllDataTriangles[k].triangles[i]].nodes[0]].y - Center.y, 2))) triDelete.push_back(AllDataTriangles[k].triangles[i]);
    }

    // По triDelete строится многоугольник Вороного (ребро считается лишним, если оно общее между двумя треугольниками)

    if (triDelete.size() == 1) {
        Triangle triangLine;
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
    else if (triDelete.size() == 2) {
        vector<int> ptVector;
        // Найдём общее ребро
        for (int i = 0; i < 3; i++) {
            if (AllDataTriangles[triDelete[0]].nodes[0] == AllDataTriangles[triDelete[1]].nodes[i]) {
                ptVector.push_back(AllDataTriangles[triDelete[0]].nodes[0]);
            }
            if (AllDataTriangles[triDelete[0]].nodes[1] == AllDataTriangles[triDelete[1]].nodes[i]) {
                ptVector.push_back(AllDataTriangles[triDelete[0]].nodes[1]);
            }
            if (AllDataTriangles[triDelete[0]].nodes[2] == AllDataTriangles[triDelete[1]].nodes[i]) {
                ptVector.push_back(AllDataTriangles[triDelete[0]].nodes[2]);
            }
        }

        sort(triDelete.begin(), triDelete.end());
        // Сторим новые треугольники
        for (int i = 1; i >= 0; i--) {
            for (int j = 0; j < 3; j++) {
                if (AllDataTriangles[triDelete[i]].nodes[j] != ptVector[0] && AllDataTriangles[triDelete[i]].nodes[j] != ptVector[1]) {
                    Triangle triangLine;
                    triangLine.nodes[0] = ptVector[0];
                    triangLine.nodes[1] = AllDataTriangles[triDelete[i]].nodes[j];
                    triangLine.nodes[2] = m;
                    AllDataTriangles.push_back(triangLine);

                    triangLine.nodes[0] = ptVector[1];
                    triangLine.nodes[1] = AllDataTriangles[triDelete[i]].nodes[j];
                    triangLine.nodes[2] = m;
                    AllDataTriangles.push_back(triangLine);
                }
            }

            AllDataTriangles.erase(AllDataTriangles.begin() + triDelete[i]);
        }
        for (int i = AllDataTriangles.size() - 1; i >= 0; i--) {
            ComparisonTriangles(i, AllDataTriangles);
        }
    }
    else if (triDelete.size() == 3) {
        vector<int> ptVector2;
        for (int p = 1; p <= 2; p++) {
            vector<int> ptVector;
            // Найдём общее ребро
            for (int i = 0; i < 3; i++) {
                if (AllDataTriangles[triDelete[0]].nodes[0] == AllDataTriangles[triDelete[p]].nodes[i]) {
                    ptVector.push_back(AllDataTriangles[triDelete[0]].nodes[0]);
                }
                if (AllDataTriangles[triDelete[0]].nodes[1] == AllDataTriangles[triDelete[p]].nodes[i]) {
                    ptVector.push_back(AllDataTriangles[triDelete[0]].nodes[1]);
                }
                if (AllDataTriangles[triDelete[0]].nodes[2] == AllDataTriangles[triDelete[p]].nodes[i]) {
                    ptVector.push_back(AllDataTriangles[triDelete[0]].nodes[2]);
                }
            }
            ptVector2.insert(ptVector2.end(), ptVector.begin(), ptVector.end());

            // Сторим новые треугольники
            for (int j = 0; j < 3; j++) {
                if (AllDataTriangles[triDelete[p]].nodes[j] != ptVector[0] && AllDataTriangles[triDelete[p]].nodes[j] != ptVector[1]) {
                    Triangle triangLine;
                    triangLine.nodes[0] = ptVector[0];
                    triangLine.nodes[1] = AllDataTriangles[triDelete[p]].nodes[j];
                    triangLine.nodes[2] = m;
                    AllDataTriangles.push_back(triangLine);

                    triangLine.nodes[0] = ptVector[1];
                    triangLine.nodes[1] = AllDataTriangles[triDelete[p]].nodes[j];
                    triangLine.nodes[2] = m;
                    AllDataTriangles.push_back(triangLine);
                }
            }
        }

        sort(ptVector2.begin(), ptVector2.end());
        for (int i = 0; i < 3; i++) {
            if (ptVector2[i] == ptVector2[i + 1]){
                ptVector2.erase(ptVector2.begin() + (i + 1)); ptVector2.erase(ptVector2.begin() + i); break;
            }
        }
        Triangle triangLine;
        triangLine.nodes[0] = ptVector2[0];
        triangLine.nodes[1] = ptVector2[1];
        triangLine.nodes[2] = m;
        AllDataTriangles.push_back(triangLine);

        sort(triDelete.begin(), triDelete.end());
        for (int i = 2; i >= 0; i--) {
            AllDataTriangles.erase(AllDataTriangles.begin() + triDelete[i]);
        }

        for (int i = AllDataTriangles.size() - 1; i >= 0; i--) {
            ComparisonTriangles(i, AllDataTriangles);
        }
    }
    else {
        printf("Unknown case of splitting.\n"); exit(-100);
    }
}

// Построение триангуляции алгоритмом «Удаляй и строй» 
void TriangulationD(int N, int M, vector<Node>& AllDataNodes, vector<Triangle>& AllDataTriangles) {
    Node midPoint = { 0,0 }; // Средняя точка
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

    Triangle triangLine;
    triangLine = SuperstructureSet(AllDataNodes); // Задание суперстуктуры - треугольник
    AllDataTriangles.push_back(triangLine);

    for (int i = 0; i < N; i++) {
        if (i != 0) {
            bool belongPoint = false; // Принадлежит ли точка треугольнику?

            for (int j = 0; j < AllDataTriangles.size(); j++) {
                Node pt1 = AllDataNodes[AllDataTriangles[j].nodes[0]];
                Node pt2 = AllDataNodes[AllDataTriangles[j].nodes[1]];
                Node pt3 = AllDataNodes[AllDataTriangles[j].nodes[2]];
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

    // Возврат системы координат
    for (int i = 0; i < N + 3; i++) {
        AllDataNodes[i].x += midPoint.x;
        AllDataNodes[i].y += midPoint.y;
    }
}

void SuperstructureDelete(int N, int M, vector<Node>& AllDataNodes, vector<Triangle>& AllDataTriangles) {
    int triSize = AllDataTriangles.size();
    vector<int> triDelete;
    for (int i = 0; i < triSize; i++) {
        bool flag = false;

        for (int j = 0; j < 3; j++) {
            if (AllDataTriangles[i].nodes[j] == N || AllDataTriangles[i].nodes[j] == N + 1 || AllDataTriangles[i].nodes[j] == N + 2) {
                flag = true; break;
            }
        }

        if (flag)
            triDelete.push_back(i);
    }

    sort(triDelete.begin(), triDelete.end());
    for (int i = triDelete.size() - 1; i >= 0; i--) {
        AllDataTriangles.erase(AllDataTriangles.begin() + triDelete[i]);
    }

    for (int i = N + 2; i >= N; i--) {
        AllDataNodes.erase(AllDataNodes.begin() + i);
    }
}

int main(int argc, char* argv[])
{
    //inputfile = "read.dat";
    //outputfile = "out.vtk";
    
    vector<Node> AllDataNodes;
    vector<Triangle> AllDataTriangles;
    int N, M = 2; // N - количество точек в файле
    
    if (argc == 1) {
        printf("The input file is undefined\nTerminate\n"); exit(ERROR_NOT_SPECIFIED_FILE);
    }

    if (argc >= 4) {
        logfile = argv[3];

        ofstream out(logfile); out.close();

        writelog("The log file is defined");
        printf("The log file is defined\n");
    }
    
    inputfile = argv[1];
    
    N = readfile(AllDataNodes, argc);
    if (argc >= 4) writelog("Mesh loaded into memory");
    printf("Mesh loaded into memory\n");
    N = AllDataNodes.size();

    TriangulationD(N, M, AllDataNodes, AllDataTriangles);
    SuperstructureDelete(N, M, AllDataNodes, AllDataTriangles);

    if (argc >= 3) {
        outputfile = argv[2];
        N = AllDataNodes.size();
        writefile(N, M, AllDataNodes, AllDataTriangles);
        if (argc >= 4) writelog("Mesh is written to the output file");
        printf("Mesh is written to the output file\n");
    }

    return 0;
}