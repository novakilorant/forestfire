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
    vector<vector<int>> grid;
    vector<vector<int>> firegrid;  
    int slipage_count = 0;
    ofstream display_file;
    ofstream fire_file;
    ofstream slipage_file;

    SandDune(int r, int c, int n, string file_name)
        : rows(r), cols(c), n_rule(n),
          display_file(file_name + ".txt"),
          fire_file(file_name + "_fire.txt"),
          slipage_file(file_name + "_slipage.txt"),
          grid(r, vector<int>(c, 0)),
          firegrid(r, vector<int>(c, 0)) {srand(seed);}

    void Initialize() {
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                grid[i][j] = 0;
    }

    void check_for_invalid_cell() {
        for (int i = 0; i < rows; ++i)
                for (int j = 0; j < cols; ++j) {
                    if (grid[i][j] > n_rule) {
                        slipage_file << "Fatal error: cell (" << i << ", " << j << ") has more than " << n_rule << " grains of sand." << endl;
                        exit(1);
                    }
                }
            }


    void step(int time) {
        int randomRow = rand() % rows;
        int randomCol = rand() % cols;
        grid[randomRow][randomCol] += 1;

        display();

        resolveAvalanche();

        check_for_invalid_cell();
        
        slipage_file << time << "\t" << slipage_count << '\n';
        slipage_count = 0;
    }

    void resolveAvalanche() {
        static const int dx[4] = {1, -1, 0, 0};
        static const int dy[4] = {0, 0, 1, -1};

        vector<pair<int,int>> unstable;
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                if (grid[i][j] >= n_rule)
                    unstable.push_back({i, j});

        while (!unstable.empty()) {
            vector<vector<int>> delta(rows, vector<int>(cols, 0));
            for (auto& row : firegrid) fill(row.begin(), row.end(), 0);

            for (auto& cell : unstable) {
                int i = cell.first, j = cell.second;
                delta[i][j] -= grid[i][j];   
                firegrid[i][j] = 1;          
                slipage_count++;
                for (int k = 0; k < 4; ++k) {
                    int ni = i + dx[k];
                    int nj = j + dy[k];
                    if (ni >= 0 && ni < rows && nj >= 0 && nj < cols)
                        delta[ni][nj] += 1;
                }
            }

            for (int i = 0; i < rows; ++i)
                for (int j = 0; j < cols; ++j)
                    grid[i][j] += delta[i][j];

            display();
            //displayFire();

            unstable.clear();
            for (int i = 0; i < rows; ++i)
                for (int j = 0; j < cols; ++j)
                    if (grid[i][j] >= n_rule)
                        unstable.push_back({i, j});
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

    void displayFire() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (j == cols - 1) fire_file << firegrid[i][j];
                else fire_file << firegrid[i][j] << "\t";
            }
            fire_file << '\n';
        }
    }

    void run(int time) {
        for (int t = 0; t < time; ++t) {
            step(t);
            display();
            displayFire();  
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
