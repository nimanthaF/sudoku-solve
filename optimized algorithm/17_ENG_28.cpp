#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include <chrono>

using namespace std;

vector<vector<vector<string>>> possible_values;
vector<vector<string>> sudoku_puzzle;
vector<string> allowedValues;

int size_of_puzzle = 0;
int size_of_subsquare = 0;

string file_name;
string inputfile_prefix;
string output_file;

bool condition = false;

//cheking the row for whether insterted value is present in the current raw
bool row_check(string input_value, int row) {

    int col = 0;
    while (col<size_of_puzzle) {
        if (sudoku_puzzle[row][col] == input_value) {
            return true;
        }
        col += 1;
    }
    return false;
}


//checking an inserted value is present in the current square
bool square_check(string input_value, int row, int col) {
    int row_off = row % size_of_subsquare;
    int lower_limit_row = row - row_off;
    int  upper_limit_row = row + (size_of_subsquare - row_off);

    int col_off = col % size_of_subsquare;
    int lower_limit_col = col - col_off;
    int upper_limit_col = col + (size_of_subsquare - col_off);
    for (row = lower_limit_row; row < upper_limit_row; row++) {
        for (col = lower_limit_col; col < upper_limit_col; col++) {
            if (sudoku_puzzle[row][col] == input_value) {
                return true;
            }
        }
    }

    return false;

}

//checking for an inserted value is present in the current column
bool column_check(string input_value, int col) {

    int row = 0;
    while (row< size_of_puzzle)
    {
        if (sudoku_puzzle[row][col] == input_value) {
            return true;
        }
        row += 1;
    }
    return false;
}


//finding list of possible values for each cells
void allowed_values_for_cells() {
    for (int row = 0; row < size_of_puzzle; row++) {
        for (int col = 0; col < size_of_puzzle; col++) {

            if (sudoku_puzzle[row][col] == "0") {
                vector<string> allowed_values;
                for (string val : possible_values[row][col]) {

                    if (!row_check(val, row) && !column_check(val, col) && !square_check(val, row, col)) {
                        allowed_values.push_back(val);
                    }
                }
                possible_values[row][col] = allowed_values;
            }
            else {
                possible_values[row][col] = {};
            }
        }
    }
}


//filling cells which are having single possible values
int filling_single_possible_values() {
    int temp = 0;

    int row = 0;
    int col = 0;
    while (row<size_of_puzzle) {
        vector<string> allowed;
        while (col<size_of_puzzle) {
            if (sudoku_puzzle[row][col] == "0" && possible_values[row][col].size() == 1) {

                sudoku_puzzle[row][col] = possible_values[row][col][0];

                possible_values[row][col] = {};
                temp += 1;
            }
            col += 1;
        }
        row += 1;
    }
    return temp;
}


//finding single possible values for rows, columns and squares
int finding_single_values() {
    int temp = 0;

    for (int row = 0; row < size_of_puzzle; row++) {
        map<string, vector<int>> pos;

        int col = 0;
        while (col < size_of_puzzle) {
            for (string val : possible_values[row][col]) {
                pos[val].push_back(col);
            }
            col += 1;
        }

        map<string, vector<int>>::iterator it;

        for (it = pos.begin(); it != pos.end(); it++)
        {
            if ((it->second).size() == 1) {
                int x = (it->second)[0];
                sudoku_puzzle[row][x] = it->first;
                possible_values[row][x] = {};
                temp += 1;
            }
        }
    }
    allowed_values_for_cells();


    for (int col = 0; col < size_of_puzzle; col++) {
        map<string, vector<int>> pos;

        int row = 0;
        while (row < size_of_puzzle) {
            for (string val : possible_values[row][col]) {
                pos[val].push_back(row);
            }
            row += 1;
        }
        
        map<string, vector<int>>::iterator it;

        for (it = pos.begin(); it != pos.end(); it++)
        {
            if ((it->second).size() == 1) {
                int y = (it->second)[0];
                sudoku_puzzle[y][col] = it->first;
                possible_values[y][col] = {};
                temp += 1;
            }
        }
    }
    allowed_values_for_cells();

    for (int row = 0; row < size_of_subsquare; row++) {
        for (int col = 0; col < size_of_subsquare; col++) {
            map<string, vector<vector<int>>> pos;
            for (int r = row * size_of_subsquare; r < (row + 1) * size_of_subsquare; r++) {
                for (int c = col * size_of_subsquare; c < (col + 1) * size_of_subsquare; c++) {
                    for (string val : possible_values[r][c]) {
                        vector<int> temp_val = { r,c };
                        pos[val].push_back(temp_val);
                    }

                }
            }
            map<string, vector<vector<int>>>::iterator it;

            for (it = pos.begin(); it != pos.end(); it++)
            {
                if ((it->second).size() == 1) {
                    vector<int> tem = (it->second)[0];
                    sudoku_puzzle[tem[0]][tem[1]] = it->first;
                    possible_values[tem[0]][tem[1]] = {};
                    temp += 1;
                }
            }
        }
    }

    return temp;
}


//optimization function
void optimization() {
    int criteria = 1;
    while (criteria) {
        allowed_values_for_cells();
        criteria = filling_single_possible_values();
        allowed_values_for_cells();
        int f2 = finding_single_values();
        criteria += f2;
    }
}

void set_cells(int row, int col);


//propagating to the next cell and checking the state of puzzle is solved
void propagate_next_cell(int row, int col) {

    if (col + 1 != size_of_puzzle) {
        set_cells(row, col + 1);
    }
    else if (row + 1 != size_of_puzzle) {
        set_cells(row + 1, 0);
    }
    else {
        condition = true;
        ofstream outfile;
        outfile.open(output_file);

        int i = 0;
        while (i < sudoku_puzzle.size()) {
            for (int j = 0; j < sudoku_puzzle[0].size(); j++) {
                outfile << sudoku_puzzle[i][j] << " ";
            }
            outfile << endl;
            i += 1;
        }

        outfile.close();

    }

}

//filling the cells by looping through them
void set_cells(int row, int col) {

    if (sudoku_puzzle[row][col] == "0") {
        for (string val : possible_values[row][col]) {
            if (!row_check(val, row) && !column_check(val, col) && !square_check(val, row, col)) {
                sudoku_puzzle[row][col] = val;
                propagate_next_cell(row, col);
            }

        }
        sudoku_puzzle[row][col] = "0";
    }
    else {
        propagate_next_cell(row, col);
    }

}


//calling the optimization function
void sudoku_solver(vector<vector<string>> sudoku_puzzle_vec) {
    clock_t tStart = clock();
    size_of_puzzle = sudoku_puzzle_vec.size();
    if (size_of_puzzle == 9) {
        size_of_subsquare = 3;
        allowedValues = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    }
    else if (size_of_puzzle == 16) {
        size_of_subsquare = 4;
        allowedValues = { "1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16" };
    }
    else {
        cout << "Invalid puzzle." << endl;;
        return;

    }

    int i = 0;
    while (i < size_of_puzzle) {
        if (sudoku_puzzle_vec[i].size() != size_of_puzzle) {
            cout << "Invalid puzzle." << endl;
            return;
        }
        i += 1;
    }

    
    for (int row = 0; row < size_of_puzzle; row++) {
        vector<vector<string>> rows_vec;
        for (int col = 0; col < size_of_puzzle; col++) {
            rows_vec.push_back(allowedValues);
        }
        possible_values.push_back(rows_vec);
    }
    allowed_values_for_cells();
    optimization();
    set_cells(0, 0);
    if (condition) {
        printf("Execution time : %.4fs\n", (double)(clock() - tStart) / (CLOCKS_PER_SEC));
    }
    else {
        cout << "Solution not found !" << endl;
        ofstream outfile;
        outfile.open(output_file);
        outfile << "No Solution";
        outfile.close();
    }

}


int main(int argc, char** argv)
{
    
    if (argc != 2) {
        cout << "Error incorrect number of arguments!" << endl;
        return -1;
    }

    file_name = argv[1];
    ifstream infile(file_name);
    vector<string> file_names_vec;
    stringstream ss(file_name);
    while (ss.good())
    {
        string sub_string;
        getline(ss, sub_string, '.');
        file_names_vec.push_back(sub_string);
    }
    inputfile_prefix = file_names_vec[0];
    output_file = inputfile_prefix + "_output.txt";
    while (infile)
    {
        string s;
        if (!getline(infile, s)) break;

        istringstream ss(s);
        vector <string> record;

        while (ss)
        {
            string s;
            if (!getline(ss, s, ' ')) break;

            istringstream conv(s);
            string value;
            conv >> value;
            record.push_back(value);
        }

        sudoku_puzzle.push_back(record);
    }

    if (!infile.eof())
    {
        cerr << "Error while reading the file!\n";
    }
    infile.close();

    sudoku_solver(sudoku_puzzle);

}


