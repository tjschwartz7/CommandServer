#ifndef MATRIX_GRAPH_H
#define MATRIX_GRAPH_H

class MatrixGraph
{

    int numRows;
    int numCols;
    bool* matrix;

public:

    /*
      col 1 2 3 4 5 6 7 8 ... n    
          _______________________________
    Row  1|
         2|
         3|
         .|
         m|
    
    */
    MatrixGraph(int numRows, int numCols)
    {
        this->numRows = numRows;
        this->numCols = numCols;
        matrix = new bool[numCols * numRows];
    }

    //Destructor
    ~MatrixGraph()
    {
        delete matrix;
    }

    bool EditConnection(int row, int col, bool isConnected)
    {
        //User specifies row#, group is column#.
        //One row is 'numCols' size.
        int index = (row * numCols) + col;

        std::cout << index << std::endl;
        bool ret = *(matrix + index);

        //Create or remove connection between row and col
        *(matrix + index) = isConnected;

        //Return whether or this row and col used to be connected
        return ret;
    }

    bool IsConnected(int row, int col)
    {
        //User specifies row#, group is column#.
        //One row is 'numCols' size. 
        int index = (row * numCols) + col;

        //Return whether or this row is connected
        return *(matrix + index);
    }


};


#endif

