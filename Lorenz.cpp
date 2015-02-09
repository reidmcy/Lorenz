#include <iostream>
#include "discpp.h" //The plotting library used DISLIN, www.dislin.de

using namespace std;


const double h = 0.000001;  //This is the step length
const int steps = 100000;  //This is the number of steps at which a point is recorded
const int factor = 1000;  //Yhis is the number of steps in between recording of points
/*
 This three values above are a trade off between run time, accuracy and smoothness of the plots.
 My tests indicate that the system has all the properties of the Lorenz system with the hvalue of 0.000001.
 The steps value I picked is the highest value for which my program would not routinely seg fault and give nice long curves.
 The factor value of 1000 is small enough that the plotted curves do not show any jerkyness even at large zooms.
 */
Dislin G;

class Lorenz {
    //Stores a single point in R^3
    //Allows the point to by moved by one step of the Lorenz system
    //Also allows a step where the final position and time are recoded
    //Additionaly there is a function that dumps to cout all stored variables and paramters
public:
    double sigma, rho, beta;
    double x, y, z, t;
    double delx() {
        return sigma * (y - x);
    }
    double dely() {
        return x*(rho - z) - y;
    }
    double delz() {
        return x*y - beta*z;
    }
    Lorenz(double aS, double aR, double aB, double aX, double aY, double aZ, double aT = 0) {
        sigma = aS;
        rho = aR;
        beta = aB;
        x = aX;
        y = aY;
        z = aZ;
        t = aT;
    }
    void print() {
        cout << sigma << '\t' << rho << '\t' << beta << endl;
        cout << x << '\t' << y << '\t' << z << '\t' << t << endl;
    }
    void step() {
        x = delx() * h + x;
        y = dely() * h + y;
        z = delz() * h + z;
        t = t + h;
    }
    void writeStep(double &iX, double &iY, double &iZ, double &iT) {
        iX = delx() * h + x;
        iY = dely() * h + y;
        iZ = delz() * h + z;
        x = iX;
        y = iY;
        z = iZ;
        t = t + h;
        iT = t;
    }
};

void returnMap(double zValue, double* xm, double* ym, double* tm, bool* up, double x[steps], double y[steps], double z[steps], double t[steps], int points) {
    //Not used for anything visible in the final project
    //not fully debugged
    int hits = 0;
    for (int loop = 0; loop < steps - 1; loop++) {
        if (((z[loop] - zValue) < 0) && ((z[loop + 1] - zValue) > 0)) {
            xm[hits] = (x[loop] + x[loop + 1]) / 2;
            ym[hits] = (y[loop] + y[loop + 1]) / 2;
            tm[hits] = (t[loop] + t[loop + 1]) / 2;
            up[hits] = true;
            hits++;
        }
        else if(((z[loop] - zValue) > 0) && ((z[loop + 1] - zValue) < 0)) {
            xm[hits] = (x[loop] + x[loop + 1]) / 2;
            ym[hits] = (y[loop] + y[loop + 1]) / 2;
            tm[hits] = (t[loop] + t[loop + 1]) / 2;
            up[hits] = false;
            hits++;
        }
        if (hits >= points) {
            break;
        }
    }
    if (hits != points) {
        cout << "Return Map only has " << hits << " Points it needs " << points << endl;
    }
}

void minMax(double &min, double &max, double* a1, double* a2) {
    //Dynamic memory allocation tricks make this unnessary
    min = a1[0];
    max = a1[0];
    for(int loop = 0; loop < steps; loop++) {
        if (min > a1[loop]) {
            min = a1[loop];
        }
        else if  (min > a2[loop]) {
            min = a2[loop];
        }
        else if  (max < a1[loop]) {
            max = a1[loop];
        }
        else if  (max < a2[loop]) {
            max = a2[loop];
        }
    }
}


void plot3D(double* x, double* y, double* z) {
    //Makes a 2D plot of a curve in DISLIN
    G.disini();
    G.setscl(x, steps, "X");
    G.setscl(y, steps, "Y");
    G.setscl(z, steps, "Z");
    G.name("X-axis", "X");
    G.name("Y-axis", "Y");
    G.name("Z-axis", "Z");
    float minX, maxX, minY, maxY, minZ, maxZ, stepX, stepY, stepZ;
    G.graf3d(minX, maxX, minX, stepX, minY, maxY, minY, stepY, minZ, maxZ, minZ, stepZ);
    G.color("GREEN");
    G.curv3d(x,y,z,steps);
    G.disfin();
}

void multiPlot3D(double* x1, double* x2, double* y1, double* y2, double* z1, double* z2) {
    //Makes a 3D plot in DISLIN of 2 curves
    G.disini();
    G.setscl(x1, 2*steps, "X");
    G.setscl(y1, 2*steps, "Y");
    G.setscl(z1, 2*steps, "Z");
    G.name("X-axis", "X");
    G.name("Y-axis", "Y");
    G.name("Z-axis", "Z");
    float minX, maxX, minY, maxY, minZ, maxZ, stepX, stepY, stepZ;
    G.graf3d(minX, maxX, minX, stepX, minY, maxY, minY, stepY, minZ, maxZ, minZ, stepZ);
    G.color("GREEN");
    G.curv3d(x1,y1,z1,steps);
    G.color("RED");
    G.curv3d(x2,y2,z2,steps);
    G.disfin();
}

void plot2D(double* x, double* y) {
    //Makes a 3D plot in DISLIN of a curve
    G.disini();
    G.setscl(x, steps, "X");
    G.setscl(y, steps, "Y");
    G.name("X-axis", "X");
    G.name("Y-axis", "Y");
    G.titlin("2D Plot",1);
    float minX, maxX, minY, maxY, stepX, stepY;
    G.graf(minX, maxX, minX, stepX, minY, maxY, minY, stepY);
    G.title();
    G.color("RED");
    G.curve(x,y,steps);
    G.disfin();
}

int main() {
    //Two Lorenz stystem's trajectories that are stepped through to create the data for plotting
    Lorenz L1(10,28,8/3,-7,-7,23);
    Lorenz L2(10,100,8/3,0,-0.01,20);
    //Dynamically allocating memory this way makes plotting the axis of the multiplot simpler
    double *X1, *Y1, *Z1, *T1;
    double *X2, *Y2, *Z2, *T2;
    X1 = new double[2*steps];
    X2 = X1 + steps;
    Y1 = new double[2*steps];
    Y2 = Y1 + steps;
    Z1 = new double[2*steps];
    Z2 = Z1 + steps;
    T1 = new double[2*steps];
    T2 = T1 + steps;
    //This loop steps through the Lorenz system Trajectories
    for (int loop = 0; loop < steps * factor; loop++) {
        if (loop % factor) {
            L1.step();
            L2.step();
        }
        else {
            L1.writeStep(X1[loop/factor], Y1[loop/factor], Z1[loop/factor], T1[loop/factor]);
            L2.writeStep(X2[loop/factor], Y2[loop/factor], Z2[loop/factor], T2[loop/factor]);
        }
    }
    //Initializer for DISLIN
    G.metafl("XWIN"); //sets the type of output file or sends output to X11
    G.scrmod("REVERS"); //Makes X11 output look nicer
    bool twoCurves = 1; //Set to true for plotting two curves, false for one
    if (twoCurves) {
        multiPlot3D(X1,X2,Y1,Y2,Z1,Z2);
    }
    else {
        plot3D(X1,Y1,Z1);
    }
    delete [] X1;
    delete [] Y1;
    delete [] Z1;
    delete [] T1;
}
