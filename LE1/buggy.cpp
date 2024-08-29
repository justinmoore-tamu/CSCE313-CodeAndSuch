#include <iostream>
#include <cstring> //included for memcpy. IDK what this does

struct Point {
    int x, y;

    Point () : x(0), y(0) {} // changed to initialize default values of 0
    Point (int _x, int _y) : x(_x), y(_y) {}
};

class Shape { 
    int vertices;
    Point** points; // pointer array

   public: // changed: imma just make the functions public since I was getting some errors with privitization. Probs considered unsafe, but eh.
    Shape (int _vertices) {
        vertices = _vertices;
        points = new Point*[vertices+1]; // +1 vertices to make a closed shape. end vertice is starting vertice
        // makes a new array of Point pointers

        // changed: need to initialize Points within points
        for (int i = 0; i < vertices+1; i++) {
            points[i] = new Point();
        }
    }

    ~Shape () {  // changed: actually destructs the points array 
        // delete all rows
        for (int i = 0; i < vertices+1; i++) { // account for extra vertex
            delete points[i];
        }
        // delete array
        delete [] points;
    }

    // I assume this just adds to the points array?
    void addPoints (/* formal parameter for unsized array called pts */ Point pts[]) { // passed in an array of Points
        for (int i = 0; i <= vertices; i++) {
            memcpy(points[i], &pts[i%vertices], sizeof(Point)); // what does this do
        }
        *points[vertices] = pts[0]; // changed:  end vertice must be the beginning one
    }

    double area () { // changed: does this function actually need to return a reference? (&)
        int temp = 0;
        for (int i = 0; i < vertices; i++) { // changed: removd equal sign to stop reaching outside of points array
            // FIXME: there are two methods to access members of pointers
            //        use one to fix lhs and the other to fix rhs
            int lhs = points[i]->x * (*points[i+1]).y;    // changed to propper member access notation. 
            int rhs = points[i+1]->x * (*points[i]).y;      // (*points[i]).y dereferences points[i] and then gets the value of y
            temp += (lhs - rhs);
        }
        double area = abs(temp)/2.0; // TODO fix?
        return area;  // changed: removed & from area

        // added reference to result instead of reference to local variable
        // return (abs(temp)/2.0);
    }
}; 

int main () {
    // FIXME: create the following points using the three different methods
    //        of defining structs:
    //          tri1 = (0, 0)
    //          tri2 = (1, 2)
    //          tri3 = (2, 0)
    Point tri1 = Point(0,0);
    Point tri2 = {1,2};
    Point tri3;
    tri3.x=2;
    tri3.y=0;
    // adding points to tri
    Point triPts[3] = {tri1, tri2, tri3};
    Shape* tri = new Shape(3); // create pointer to a shape object
    tri->addPoints(triPts); // add points to define the shape

    // FIXME: create the following points using your preferred struct
    //        definition:
    //          quad1 = (0, 0)
    //          quad2 = (0, 2)
    //          quad3 = (2, 2)
    //          quad4 = (2, 0)

    Point quad1 = {0,0};
    Point quad2 = {0,2};
    Point quad3 = {2,2};
    Point quad4 = {2,0};
    // adding points to quad
    Point quadPts[4] = {quad1, quad2, quad3, quad4};
    Shape* quad = new Shape(4);
    quad->addPoints(quadPts);

    // FIXME: print out area of tri and area of quad
    std::cout << "area of tri: " << tri->area() << std::endl;
    std::cout << "area of quad: " << quad->area() << std::endl;

    // changed: deleting tri and quad objects created using "new"
    delete tri;
    delete quad;

    // changed: added return statement to man function
    return 0;
}
