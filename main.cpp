/* 
Author: Stella Fournier
Class: ECE4122 A 
Last Date Modified: 9/25/2022 
 
Description: 
This is the code file for the SudokuSolver problem. This file contains the SudokuGrid 
class and the member functions definition for the operator overloading functions. 
It takes in an input file from the command line and creates an output file with the 
solved sudoku puzzles.
*/ 

using namespace std;

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <mutex>
#include <list>
#include <chrono>
#include <ctime>
#include "SudokuSolver.cpp"

#define N 9

//Global Variables
fstream myFile;
fstream outFile;
mutex outFileMutex; 
mutex inFileMutex; 

//This is a class called SudokuGrid with member variables for n_strGridName and the sudoku grid called 
//gridElement and also contains two member functions for overloading >> and << operators.
class SudokuGrid
{
    public:
        string n_strGridName;
        unsigned char gridElement[N][N];
        friend fstream& operator>>(fstream& os, SudokuGrid & gridIn);
        friend fstream& operator<<(fstream& os, const SudokuGrid & gridOut);
};

//This member function for SudokuGrid class overloads the >> operator to 
//read a single SudokuGrid object from a fstream file
fstream& operator>>(fstream& os, SudokuGrid& gridIn)
{
    string myString;
    
    while(getline(os, myString))
    {
        if(myString[0] == 'G')
        {
            gridIn.n_strGridName = myString;
            break;
        }
    }
    for(int i = 0; i < 9; i++)
    { 
        getline(os, myString);
        for(int j = 0; j < 9; j++)
        {
            gridIn.gridElement[i][j] = myString[j];
        }  
    } 
    return os;
}

//This member function for SudokuGrid class overloads the << operator to write the
//SudokuGrid object to a file in the same format that is used in reading in the object 
fstream& operator<<(fstream& os, const SudokuGrid& gridOut)
{
    os << gridOut.n_strGridName << "\n";
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            os << gridOut.gridElement[i][j];
        }
        os << "\n";
    }
    return os;
}

//This function basically takes the input file, solve the sudoku puzzles, then output a file
//with the solved puzzles.
void solveSudokuPuzzles()
{
    SudokuGrid myGrid;
    //This while loop will continue to solve the puzzle then output a file with the solved puzzles
    //until it detects the end of input file. 
    do
    {
        inFileMutex.lock();
        //If it is the end of file for the input file, it will unlock the inFileMutex then jump out of the while loop.
        if(myFile.eof())
        {
            inFileMutex.unlock();
            return;
        }
        myFile >> myGrid;
        inFileMutex.unlock();
        int grid[N][N];

        //convert grid from int to unsigned char.
        for(int i = 0; i < N; i++)
        {
            for(int j = 0; j < N; j++)
            {
                grid[i][j] = myGrid.gridElement[i][j] - 48;
            }
        }
        //refereced code from Geeks for Geeks to solve the sudoku puzzle.
        solveSudoku(grid, 0, 0);
      
        //convert grid back from unsigned char to int
        for(int i = 0; i < N; i++)
        {
            for(int j = 0; j < N; j++)
            {
                myGrid.gridElement[i][j] = grid[i][j] + 48;
            }
        }
        outFileMutex.lock();
        outFile << myGrid; 
        outFileMutex.unlock();
    } 
    while(1);
}

int main(int argc, char *argv[])
{
    //Open myFile and outFile    
    myFile.open(argv[1]);
    outFile.open("Lab1Prob2.txt", ios::out);

    unsigned int nthreads = thread::hardware_concurrency() - 1;
    list<thread> mythreads;
    int i = 0;
    //Thie creates max threads availalbe
    for (i = 0; i < nthreads; i++) 
    { 
        //It pushes thread to list
        mythreads.push_back(thread(solveSudokuPuzzles));   
    }
    //This waits for threads to finish
    auto originalthread = mythreads.begin();
    while (originalthread != mythreads.end()) 
    {
        originalthread->join();
        originalthread++;
    }

    //closes outFile and myFile
    outFile.close();
    myFile.close();
}