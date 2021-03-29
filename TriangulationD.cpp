// TriangulationD.cpp: определяет точку входа для приложения.
//

#define _CRT_SECURE_NO_WARNINGS
#include "TriangulationD.h"

using namespace std;

int readflie(int M, vector<vector<float> >& triangle) {
    vector <float> strf1(M); //вектор для одной точки
    string s;
    int circuit = 0;
    int i = 0;
    ifstream cin("read.dat");
    if (!cin) { printf("Can't open file read.dat\n Terminate\n"); exit(-1); }

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
        sscanf(s.c_str(), "%f %f", &strf1[0], &strf1[1]);
        triangle.push_back(strf1);
    }
    cin.close();
    return i;
}

void writefile(int N1, int N2, int M, vector<vector<float> >& triangle) {
    ofstream out("write.vtu");
    out << "# vtk DataFile Version 2.0";
    out << endl << "TriangulationD result";
    out << endl << "ASCII";
    out << endl << endl << "DATASET POLYDATA";
    out << endl << "POINTS " << N2 << " float" << endl;
    for (int i = 0; i < N2; i++) {
        out << triangle[i][0] << " " << triangle[i][1] << " " << 0 << endl;
    }
    out << endl << "LINES " << 2 << " " << N2 + 4;
    out << endl << N1 + 1;
    for (int i = 0; i < N1; i++) {
        out << " " << i;
    }
    out << " " << 0;
    out << endl << N2 - N1 + 1;
    for (int i = N1; i < N2; i++) {
        out << " " << i;
    }
    out << " " << N1;
    out.close();
}

int main()
{
    vector < vector < float > > triangle; //triangle - двумерный вектор входных точек
    int N1, N2, M = 2; //N1 - количество строк точек внешнего контура, N2 - количество строк всех точек в файле (только один внутренний контур), M - столбцов в triangle

    N1 = readflie(M, triangle); //читает файл и записывает в triangle, возврашает число строк
    N2 = triangle.size();
    writefile(N1, N2, M, triangle);

    //for (int i = 0; i < N2; i++) {
    //    for (int j = 0; j < M; j++) {
    //        cout << triangle[i][j] << " ";
    //    }
    //    cout << endl;
    //}

    return 0;
}
