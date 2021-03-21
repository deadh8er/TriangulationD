// TriangulationD.cpp: определяет точку входа для приложения.
//

#define _CRT_SECURE_NO_WARNINGS
#include "TriangulationD.h"

using namespace std;

int readflie(int M, vector<vector<float> >& triangle) {
    vector <float> strf1(M);
    string s;
    int i = 0;
    ifstream cin("read.dat");
    if (!cin) { printf("Can't open file read.dat\n Terminate\n"); exit(-1); }

    while (getline(cin, s)) {
        i++;
        sscanf(s.c_str(), "%f %f", &strf1[0], &strf1[1]);
        triangle.push_back(strf1);
    }
    cin.close();
    return i;
}

void writefile(int N, int M, vector<vector<float> >& triangle) {
    ofstream out("write.vtu");
    out << "# vtk DataFile Version 2.0" << endl;
    out << "TriangulationD result" << endl;
    out << "ASCII" << endl;
    out << "DATASET POLYDATA" << endl;
    out << "POINTS " << N << " float" << endl;
    for (int i = 0; i < N; i++) {
        out << triangle[i][0] << " " << triangle[i][1] << " " << 0 << endl;
    }
    out.close();
}

int main()
{
    vector < vector < float > > triangle; //triangle - двумерный вектор входных точек
    int N, M = 2; //N - строк, M - столбцов в triangle

    N = readflie(M, triangle); //читает файл и записывает в triangle, возврашает число строк

    writefile(N, M, triangle);

    //for (int i = 0; i < N; i++) {
    //    for (int j = 0; j < M; j++) {
    //        cout << triangle[i][j] << " ";
    //    }
    //    cout << endl;
    //}

    return 0;
}
