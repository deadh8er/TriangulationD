// TriangulationD.cpp: определяет точку входа для приложения.
//

#define _CRT_SECURE_NO_WARNINGS
#include "TriangulationD.h"

char* logfile;

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

int readflie(char* inputfile, int M, vector<vector<float> >& triangle, int argc) {
    vector <float> strf1(M); //вектор для одной точки
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
        sscanf(s.c_str(), "%f %f", &strf1[0], &strf1[1]);
        triangle.push_back(strf1);
    }
    cin.close();
    return i;
}

void writefile(char* outputfile, int N1, int N2, int M, vector<vector<float> >& triangle) {
    ofstream out(outputfile);
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

int main(int argc, char* argv[])
{
    vector < vector < float > > triangle; //triangle - двумерный вектор входных точек
    int N1, N2, M = 2; //N1 - количество строк точек внешнего контура, N2 - количество строк всех точек в файле (только один внутренний контур), M - столбцов в triangle

    if (argc == 1) {
        printf("The input file is undefined\nTerminate\n"); exit(ERROR_NOT_SPECIFIED_FILE);
    }

    if (argc == 4) {
        logfile = argv[3];

        ofstream out(logfile); out.close();

        writelog("The log file is defined");
        printf("The log file is defined\n");
    }

    char* inputfile = argv[1];
    N1 = readflie(inputfile, M, triangle, argc); //читает файл и записывает в triangle, возврашает число строк
    if (argc == 4) writelog("Mesh loaded into memory");
    printf("Mesh loaded into memory\n");
    N2 = triangle.size();

    if (argc > 2) {
        char* outputfile = argv[2];
        writefile(outputfile, N1, N2, M, triangle); //записывает в файл полученную сетку
        if (argc == 4) writelog("Mesh is written to the output file");
        printf("Mesh is written to the output file\n");
    }

    //for (int i = 0; i < N2; i++) {
    //    for (int j = 0; j < M; j++) {
    //        cout << triangle[i][j] << " ";
    //    }
    //    cout << endl;
    //}

    return 0;
}