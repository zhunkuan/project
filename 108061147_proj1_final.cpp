#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <fstream>

using namespace std;

class playing_space{
    private:
        bool **space;                       // The matrix of playing space.
        int row;
        int col;
    public:
        playing_space():row(0), col(0){}
        playing_space(int ROW, int COL);    // Initializing the playing space.
        ~playing_space();                                    
        bool drop(class block &B);
        bool delete_row(int row_deleted);   // Delete the row if it is full.
        bool delete_check(int read_row);
        bool check_over();                  // Check if it is out of range.
        void print();                       // Make an output file.
        //void print_real_space();
};

class block{
    friend playing_space;
    protected:
        int stop_size;
            // The value of the horizontal width of the block.
        int *lowest_block_position;  
            // Every column of the block has a lowest element, so we can check out its  
            // distance from the reference point (by row), and the value of columns is "stop_size".
        int block_position[4][2];
            // Every block has four element, so we give four pairs of rows and columns
            // to represent the coordinates of them, which let the reference point be (0,0).      
        int now_row;
            // The row of the reference point.
        int initial_position;
            // The column of the reference point(first drop).
        int move;
            // The movement of the block.
    public:
    virtual ~block(){};
    int get_row();
};

playing_space::playing_space(int ROW, int COL):row(ROW + 4), col(COL){
    space = new bool*[row];                 // Row + 4 prevent the block from out of the matrix. 
    for (int i = 0; i < row; ++i)           // The playing space will be start at the fifth row,
        space[i] = new bool[COL];           // and the first four rows are used to detect if it
    for (int i = 0; i < row; ++i)           // is out of the playing space.
        for (int j = 0; j < COL; ++j)
            space[i][j] = 0;                // Initialize.
}

playing_space::~playing_space(){
    for (int i = 0; i < row; ++i)
        delete space[i];
    delete space;
}

bool playing_space::drop(block &B){
    bool invalid = false;
    if ((B.initial_position >= 0) && (B.initial_position <= col - B.stop_size)){
        bool stop = false;
        for (int i = B.now_row; (i < row - 1) && (!stop); i++)
        { 
            for (int j = 0; (j < B.stop_size) && (!stop); ++j)
            {
                if (space[i - B.lowest_block_position[j] + 1][B.initial_position + j])
                    stop = true;        // Here is the calculation of the first drop,
            }                           // by using the lowest block position of the columns.
            if (!stop) B.now_row++;
        }

        stop = false;
        if (B.move < 0){
            for (int i = 1; (i <= -B.move) && (!stop); i++){
                for (int j = 0; (j < 4) && (!stop); j++){
                    if (B.initial_position + B.block_position[j][1] - i < 0){
                        cout << "Invalid input." << endl;
                        invalid = true;     // If the movement < 0, that is, move left, we should check if it is
                        stop = true;        // out of range or will hit other blocks in the playing space.
                    } 
                    else if (space[B.now_row - B.block_position[j][0]][B.initial_position + B.block_position[j][1] - i]){
                        cout << "Invalid input." << endl;
                        invalid = true;
                        stop = true;
                    }
                }
            }
            if (!stop) B.initial_position += B.move;
                                            // If the block won't go out of the playing space or hit the others,
                                            // then move it to the next position. 
        }
        else if (B.move > 0){
            for (int i = 1; (i <= B.move) && (!stop); i++){
                for (int j = 0; (j < 4) && (!stop); j++){
                    if (B.initial_position + B.block_position[j][1] + i > col - 1){
                        cout << "Invalid input." << endl;
                        invalid = true;     // If the movement > 0, that is, move right, we should check if it is
                        stop = true;        // out of range or will hit other blocks in the playing space.
                    } 
                    else if (space[B.now_row - B.block_position[j][0]][B.initial_position + B.block_position[j][1] + i]){
                        cout << "Invalid input." << endl;
                        invalid = true;
                        stop = true;
                    }
                }
            }
            if (!stop) B.initial_position += B.move;
                                            // If the block won't go out of the playing space or hit the others,
                                            // then move it to the next position. 
        }
        else stop = true;                   // If the movement = 0, do nothing.

        if (B.now_row != row - 1){
            stop = false;
            for (int i = B.now_row; (i < row - 1) && (!stop); i++)
            { 
                for (int j = 0; (j < B.stop_size) && (!stop); ++j)
                {
                    if (space[i - B.lowest_block_position[j] + 1][B.initial_position + j])
                        stop = true;        // Here is the second drop (after moving horizontal),
                }                           // using the lowest block position of the columns.
                if (!stop) B.now_row++;
            }
        }

        for (int i = 0; i < 4; ++i)
            space[B.now_row - B.block_position[i][0]][B.initial_position + B.block_position[i][1]] = true;
                                            // Add the block into the playing space.
    }
    else{
        cout << "Invalid input." << endl;
        invalid = true;
    }
    return invalid;
}

bool playing_space::check_over(){                   // Use the first 4 row of row + 4 
    for (int i = 0; i < 4; ++i)                     // to determine whether it is out 
        for (int j = 0; j < col; ++j)               // of range or not.
            if (space[i][j]) return true;
    return false;
}

bool playing_space::delete_row(int row_deleted){
    bool delete_done = false;
    if (delete_done = delete_check(row_deleted)){
        for (int i  = row_deleted; i >= 1; --i)     // Let all the rows above the deleted row
            for (int j = 0; j < col; ++j)           // go down one row.
                space[i][j] = space[i - 1][j];
    }
    for (int i = 0; i < col; ++i)                   // We don't know what are the values of the 
        space[0][i] = false;                        // first row if we done the code above,
    return delete_done;                             // so we set it to 0.
}

bool playing_space::delete_check(int read_row){
    int sum = 0;
    for (int i = 0; i < col; ++i){                  // If the value of the read row are all 1,
        sum += space[read_row][i];                  // return true.
    }
    if (sum == col) return true;
    else return false;
}

void playing_space::print(){                        // Output the file.
    
    fstream file;
    
    file.open("108061147_proj1.final", ios::out);
    if (!file.good()){
        cout << "Output fails.\n";
        return;
    }

    char *arr;
    arr = new char[col + 1];
    
    for (int i = 4; i < row; ++i){        
        for (int j = 0; j < col; ++j){   
            if (space[i][j]) arr[j] = '1';
            else arr[j] = '0';
            file << arr[j] << " ";
        }
            arr[col] = '\n';
            file << arr[col];
    }
    file.close();
}

int block::get_row(){
    return now_row;
}

class T_block:public block{
    public:
    T_block(){}
    T_block(int initial_place, int type, int movement);
    ~T_block();

};

T_block::T_block(int initial_place, int type, int movement){
    
    now_row = 3;
    initial_position = initial_place;
    move = movement;
    
    if (type == 1)
    {        
        stop_size = 3;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 1;
        lowest_block_position[1] = 0;
        lowest_block_position[2] = 1;

        block_position[0][0] = 0;
        block_position[0][1] = 1;
        block_position[1][0] = 1;
        block_position[1][1] = 0;
        block_position[2][0] = 1;
        block_position[2][1] = 1;
        block_position[3][0] = 1; 
        block_position[3][1] = 2;
    }
    else if (type == 2)
    {
        stop_size = 2;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 1;
        lowest_block_position[1] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 1;
        block_position[1][0] = 1;
        block_position[1][1] = 0;
        block_position[2][0] = 1;
        block_position[2][1] = 1;
        block_position[3][0] = 2; 
        block_position[3][1] = 1;
    }
    else if (type == 3)
    {
        stop_size = 3;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 0;
        lowest_block_position[2] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 0;
        block_position[1][1] = 1;
        block_position[2][0] = 0;
        block_position[2][1] = 2;
        block_position[3][0] = 1; 
        block_position[3][1] = 1;
    }
    else if (type == 4)
    {
        stop_size = 2;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 1;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 1;
        block_position[1][1] = 0;
        block_position[2][0] = 1;
        block_position[2][1] = 1;
        block_position[3][0] = 2; 
        block_position[3][1] = 0;
    }
    else cout << "Invalid input." << endl;
}

T_block::~T_block(){delete lowest_block_position;}

class L_block:public block{
    public:
    L_block(){}
    L_block(int initial_place, int type, int movement);
    ~L_block();
};

L_block::L_block(int initial_place, int type, int movement){
    
    now_row = 3;
    initial_position= initial_place;
    move = movement;
    
    if (type == 1)
    {        
        stop_size = 2;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 0;
        block_position[1][1] = 1;
        block_position[2][0] = 1;
        block_position[2][1] = 0;
        block_position[3][0] = 2; 
        block_position[3][1] = 0;
    }
    else if (type == 2)
    {
        stop_size = 3;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 1;
        lowest_block_position[2] = 1;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 1;
        block_position[1][1] = 0;
        block_position[2][0] = 1;
        block_position[2][1] = 1;
        block_position[3][0] = 1; 
        block_position[3][1] = 2;
    }
    else if (type == 3)
    {
        stop_size = 2;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 2;
        lowest_block_position[1] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 1;
        block_position[1][0] = 1;
        block_position[1][1] = 1;
        block_position[2][0] = 2;
        block_position[2][1] = 0;
        block_position[3][0] = 2; 
        block_position[3][1] = 1;
    }
    else if (type == 4)
    {
        stop_size = 3;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 0;
        lowest_block_position[2] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 0;
        block_position[1][1] = 1;
        block_position[2][0] = 0;
        block_position[2][1] = 2;
        block_position[3][0] = 1; 
        block_position[3][1] = 2;
    }
    else cout << "Invalid input." << endl;
}

L_block::~L_block(){delete lowest_block_position;}

class J_block:public block{
    public:
    J_block(){}
    J_block(int initial_place, int type, int movement);
    ~J_block();
};

J_block::J_block(int initial_place, int type, int movement){
    
    now_row = 3;
    initial_position= initial_place;
    move = movement;
    
    if (type == 1)
    {        
        stop_size = 2;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 0;
        block_position[1][1] = 1;
        block_position[2][0] = 1;
        block_position[2][1] = 1;
        block_position[3][0] = 2; 
        block_position[3][1] = 1;
    }
    else if (type == 2)
    {
        stop_size = 3;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 0;
        lowest_block_position[2] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 0;
        block_position[1][1] = 1;
        block_position[2][0] = 0;
        block_position[2][1] = 2;
        block_position[3][0] = 1; 
        block_position[3][1] = 0;
    }
    else if (type == 3)
    {
        stop_size = 2;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 2;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 1;
        block_position[1][1] = 0;
        block_position[2][0] = 2;
        block_position[2][1] = 0;
        block_position[3][0] = 2; 
        block_position[3][1] = 1;
    }
    else if (type == 4)
    {
        stop_size = 3;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 1;
        lowest_block_position[1] = 1;
        lowest_block_position[2] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 2;
        block_position[1][0] = 1;
        block_position[1][1] = 0;
        block_position[2][0] = 1;
        block_position[2][1] = 1;
        block_position[3][0] = 1; 
        block_position[3][1] = 2;
    }
    else cout << "Invalid input." << endl;
}

J_block::~J_block(){delete lowest_block_position;}

class S_block:public block{
    public:
    S_block(){}
    S_block(int initial_place, int type, int movement);
    ~S_block();
};

S_block::S_block(int initial_place, int type, int movement){
    
    now_row = 3;
    initial_position= initial_place;
    move = movement;
    
    if (type == 1)
    {        
        stop_size = 3;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 0;
        lowest_block_position[2] = 1;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 0;
        block_position[1][1] = 1;
        block_position[2][0] = 1;
        block_position[2][1] = 1;
        block_position[3][0] = 1; 
        block_position[3][1] = 2;
    }
    else if (type == 2)
    {
        stop_size = 2;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 1;
        lowest_block_position[1] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 1;
        block_position[1][0] = 1;
        block_position[1][1] = 0;
        block_position[2][0] = 1;
        block_position[2][1] = 1;
        block_position[3][0] = 2; 
        block_position[3][1] = 0;
    }
    else cout << "Invalid input." << endl;
}

S_block::~S_block(){delete lowest_block_position;}

class Z_block:public block{
    public:
    Z_block(){}
    Z_block(int initial_place, int type, int movement);
    ~Z_block();
};

Z_block::Z_block(int initial_place, int type, int movement){
    
    now_row = 3;
    initial_position= initial_place;
    move = movement;
    
    if (type == 1)
    {        
        stop_size = 3;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 1;
        lowest_block_position[1] = 0;
        lowest_block_position[2] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 1;
        block_position[1][0] = 0;
        block_position[1][1] = 2;
        block_position[2][0] = 1;
        block_position[2][1] = 0;
        block_position[3][0] = 1; 
        block_position[3][1] = 1;
    }
    else if (type == 2)
    {
        stop_size = 2;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 1;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 1;
        block_position[1][1] = 0;
        block_position[2][0] = 1;
        block_position[2][1] = 1;
        block_position[3][0] = 2; 
        block_position[3][1] = 1;
    }
    else cout << "Invalid input." << endl;
}

Z_block::~Z_block(){delete lowest_block_position;}

class I_block:public block{
    public:
    I_block(){}
    I_block(int initial_place, int type, int movement);
    ~I_block();
};

I_block::I_block(int initial_place, int type, int movement){
    
    now_row = 3;
    initial_position= initial_place;
    move = movement;
    
    if (type == 1)
    {        
        stop_size = 1;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 1;
        block_position[1][1] = 0;
        block_position[2][0] = 2;
        block_position[2][1] = 0;
        block_position[3][0] = 3; 
        block_position[3][1] = 0;
    }
    else if (type == 2)
    {
        stop_size = 4;
        lowest_block_position = new int[stop_size];
        lowest_block_position[0] = 0;
        lowest_block_position[1] = 0;
        lowest_block_position[2] = 0;
        lowest_block_position[3] = 0;

        block_position[0][0] = 0;
        block_position[0][1] = 0;
        block_position[1][0] = 0;
        block_position[1][1] = 1;
        block_position[2][0] = 0;
        block_position[2][1] = 2;
        block_position[3][0] = 0; 
        block_position[3][1] = 3;
    }
    else cout << "Invalid input." << endl;
}

I_block::~I_block(){delete lowest_block_position;}

class O_block:public block{
    public:
    O_block(){}
    O_block(int initial_place, int movement);
    ~O_block();
};

O_block::O_block(int initial_place, int movement){
    
    now_row = 3;
    initial_position= initial_place;
    move = movement;

    stop_size = 2;
    lowest_block_position = new int[stop_size];
    lowest_block_position[0] = 0;
    lowest_block_position[1] = 0;

    block_position[0][0] = 0;
    block_position[0][1] = 0;
    block_position[1][0] = 0;
    block_position[1][1] = 1;
    block_position[2][0] = 1;
    block_position[2][1] = 0;
    block_position[3][0] = 1; 
    block_position[3][1] = 1;
}

O_block::~O_block(){delete lowest_block_position;}

int main(int argc, char* argv[]){
    
    ifstream FILEIN;

    FILEIN.open(argv[1], ios::in);
    if (!FILEIN.good()){
        cout << "Open fails.\n";
        return 0;
    }

    int row, col;
    char ch[50];
    int first;
    int movement;
    bool gameover = false;

    FILEIN >> row >> col;
    playing_space space (row, col);
    FILEIN >> ch;
    while (strcmp(ch, "End") && !gameover){    
        char type;
        int type_number;

        if (ch[0] != 'O') {                 // There are different types of blocks
            type_number = atoi(&ch[1]);     // except "O", so we separate them by
            type = ch[0];                   // using "type_number" and "type".
        }
        else type = ch[0];

        FILEIN >> first;                    // Read the initial position.
        first--;
        FILEIN >> movement;                 // Read the move of the block.
        block *B;

        if (!gameover){   
            if (type == 'T')     
                B = new T_block(first, type_number, movement);
            else if (type == 'L')
                B = new L_block(first, type_number, movement);
            else if (type == 'J')
                B = new J_block(first, type_number, movement); 
            else if (type == 'S')
                B = new S_block(first, type_number, movement); 
            else if (type == 'Z')
                B = new Z_block(first, type_number, movement); 
            else if (type == 'I')
                B = new I_block(first, type_number, movement); 
            else
                B = new O_block(first, movement);
            gameover = space.drop(*B);     // The block drops and put the result in the playing space.  
            for (int i = 0, k = 0; i < 4; ++i){    
                if ((B->get_row() - k) >= 4 && space.delete_row(B->get_row() - k));
                else k++;                   
            }       // The rows be deleted can only be the four that are above the reference point,
                    // so we let the loop run four times to check the four row. Olso, if there is
                    // any row removed, then we will pull all the rows above down, thus the next row
                    // we should check is the current row; if not, check the upper row. 
            if (!gameover) gameover = space.check_over();
            delete B;
        }
        if (!gameover) FILEIN >> ch;
    }
    if (gameover) cout << "Out of range or invalid input." << endl;
    space.print();
    FILEIN.close();
    return 0;
}