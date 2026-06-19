    #include <iostream>
    #include <vector>
    #include <fstream>
    #include <cstdlib>
    #include <ctime>

    using namespace std;

    int n_rows, n_cols, n_rule, run_time, growth;
    double density;
    string file_name, mask_file_name;
    bool has_mask = false;

    int seed = time(0);

    struct SandDune {
        int rows, cols, n_rule, growth;
        vector<vector<int>> grid;
        vector<vector<bool>> mask;  
        int slipage_count = 0;
        ofstream display_file;
        ofstream slipage_file;
        const string mask_path;

        SandDune(int r, int c, int n, int g, string file_name, const string& _mask_path = "")
            : rows(r), cols(c), n_rule(n), growth(g),
            display_file(file_name + ".txt"),
            slipage_file(file_name + "_slipage.txt"),
            grid(r, vector<int>(c, 0)),
            mask(r, vector<bool>(c, false)),
            mask_path(_mask_path){
            srand(seed);
            
        }

        void Initialize() {
            for (int i = 0; i < rows; ++i)
                for (int j = 0; j < cols; ++j)
                    grid[i][j] = 0;
            if (!mask_path.empty()) loadMask(mask_path);                   
        }

        void loadMask(const string& path) {
            ifstream f(path);
            if (!f) {
                cerr << "Could not open mask file: " << path << endl;
                exit(1);
            }
            for (int i = 0; i < rows; ++i)
                for (int j = 0; j < cols; ++j) {
                    int v;
                    if (!(f >> v)) {
                        cerr << "Mask file is smaller than " << rows << "x" << cols << endl;
                        exit(1);
                    }
                    mask[i][j] = (v != 0);
    
                    if (mask[i][j]) grid[i][j] = -1;
                }
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

            for (int i = 0; i < growth; i++) {
                int randomRow = rand() % rows;
                int randomCol = rand() % cols;

                do {
                    randomRow = rand() % rows;
                    randomCol = rand() % cols;
                } while (mask[randomRow][randomCol]);
                
                grid[randomRow][randomCol] += 1;
            }

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
                display();
                vector<vector<int>> delta(rows, vector<int>(cols, 0));

                for (auto& cell : unstable) {
                    int i = cell.first, j = cell.second;
                    delta[i][j] -= n_rule;   
                    slipage_count++;
                    for (int k = 0; k < n_rule; ++k) {
                        int ni = i + dx[k];
                        int nj = j + dy[k];
                        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols && !mask[ni][nj])
                            delta[ni][nj] += 1;
                    }
                }

                for (int i = 0; i < rows; ++i)
                    for (int j = 0; j < cols; ++j)
                        grid[i][j] += delta[i][j];

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

        void run(int time) {
            for (int t = 0; t < time; ++t) {
                step(t);
                display();
            }
        }
    };

    void getInput (int argc, char** argv) {
    
        if (argc == 7 || argc == 8) {
            n_rows = atoi(argv[1]);
            n_cols = atoi(argv[2]);
            n_rule = atoi(argv[3]);
            run_time = atoi(argv[4]);
            file_name = argv[5];
            growth = atoi(argv[6]);
            if (n_rule < 1) {
                cout << "n_rule must be greater than 0." << endl;
                exit(1);
            }
            if (argc == 8) {
                has_mask = true;
                mask_file_name = argv[7];
            }
            return;
        }
    
        cout << "Usage: simulator n_rows n_cols n_rule run_time file_name [mask_file]" << endl;
        exit(1);
    }
    
    int main(int argc, char** argv) {
    
        getInput(argc, argv);

        SandDune sand_dune(n_rows, n_cols, n_rule, growth, file_name, has_mask ? mask_file_name : "");
        sand_dune.Initialize();
        sand_dune.run(run_time);
        return 0;
    }
