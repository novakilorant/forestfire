#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

int n_rows, n_cols, n_rule, run_time;
double density;
string file_name;

int seed = time(0);

struct SandDune {
    int rows, cols, n_rule;
    vector<vector<int>> grid, firegrid;
    int slipage_count = 0;
    ofstream display_file;
    ofstream slipage_file;

    SandDune(int r, int c, int n, string file_name)
        : rows(r), cols(c), n_rule(n), display_file(file_name + ".txt"), slipage_file(file_name + "_slipage.txt"),
        grid(r, vector<int>(c, 0)) {srand(seed);}

    void Initialize() {
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                grid[i][j] = 0;
    }

    void step(int time) {
        int randomRow = rand() % rows;
        int randomCol = rand() % cols;
        grid[randomRow][randomCol] += 1;
        if (grid[randomRow][randomCol] == n_rule) induceSlipage(randomRow, randomCol);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j) {
                if (grid[i][j] > n_rule) {
                    slipage_file << "Fatal error: cell (" << i << ", " << j << ") has more than " << n_rule << " grains of sand." << endl;
                    exit(1);
                }
            }
        slipage_file << time << "\t" << slipage_count << '\n';
        slipage_count = 0;
    }

    void induceSlipage(int i, int j) {
        //display();
        grid[i][j] = 0;
        slipage_count++;
        int dx[4] = {1, -1, 0, 0};
        int dy[4] = {0, 0, 1, -1};
        for (int k = 0; k < 4; ++k) {
            int ni = i + dx[k];
            int nj = j + dy[k];
            if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                grid[ni][nj] += 1;
                if (grid[ni][nj] == n_rule) induceSlipage(ni, nj);
            }
        }

    }

    void display() {        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (j == cols - 1) display_file << grid[i][j];
                else display_file << grid[i][j] << "\t";
            }
            display_file << '\n';
        }
    }

    void run(int time) {
        for (int t = 0; t < time; ++t) {
            step(t);
            display();
        }
    }
};

void getInput (int argc, char** argv) {


    if (argc == 6) {
        n_rows = atoi(argv[1]);
        n_cols = atoi(argv[2]);
        n_rule = atoi(argv[3]);
        run_time = atoi(argv[4]);
        file_name = argv[5];
        if (n_rule < 1) {
            cout << "n_rule must be greater than 0." << endl;
            exit(1);
        }
        return;
    }

}

int main(int argc, char** argv) {

    getInput(argc, argv);

    SandDune sand_dune(n_rows, n_cols, n_rule, file_name);
    sand_dune.Initialize();
    sand_dune.run(run_time);
    return 0;
}