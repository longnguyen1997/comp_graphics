#include "parse.h"
#include <map>
using namespace std;


namespace {

    // read in dim-dimensional control points into a vector
    vector<Vector3f> readCps(istream &in, unsigned dim)
    {    
        // number of control points    
        unsigned n;
        in >> n;

        cerr << "  " << n << " cps" << endl;
    
        // vector of control points
        vector<Vector3f> cps(n);

        char delim;
        float x;
        float y;
        float z;

        for( unsigned i = 0; i < n; ++i )
        {
            switch (dim)
            {
            case 2:
                in >> delim;
                in >> x;
                in >> y;
                cps[i] = Vector3f( x, y, 0 );
                in >> delim;
                break;
            case 3:
                in >> delim;
                in >> x;
                in >> y;
                in >> z;
                cps[i] = Vector3f( x, y, z );
                in >> delim;
                break;            
            default:
                abort();
            }
        }

        return cps;
    }
}



bool parseFile(istream &in,
               vector<vector<Vector3f> > &ctrlPoints, 
               vector<Curve>             &curves,
               vector<string>            &curveNames,
               vector<Surface>           &surfaces,
               vector<string>            &surfaceNames)
{
    ctrlPoints.clear();
    curves.clear();
    curveNames.clear();
    surfaces.clear();
    surfaceNames.clear();    
    
    string objType;

    // For looking up curve indices by name
    map<string,unsigned> curveIndex;

    // For looking up surface indices by name
    map<string,unsigned> surfaceIndex;
        
    // For storing dimension of curve
    vector<unsigned> dims;

    unsigned counter = 0;
    
    while (in >> objType) 
    {
        cerr << ">object " << counter++ << endl;
        string objName;
        in >> objName;

        bool named = (objName != ".");
        
        vector<Vector3f> cpsToAdd;
        
        if (curveIndex.find(objName) != curveIndex.end() ||
            surfaceIndex.find(objName) != surfaceIndex.end())
        {
            cerr << "error, [" << objName << "] already exists" << endl;
            return false;
        }

        unsigned steps;

        if (objType == "bez2")
        {
            in >> steps;
            cerr << " reading bez2 " << "[" << objName << "]" << endl;
            curves.push_back( evalBezier(cpsToAdd = readCps(in, 2), steps) );
            curveNames.push_back(objName);
            dims.push_back(2);
            if (named) curveIndex[objName] = (int)dims.size()-1;
            
        }
        else if (objType == "bsp2")
        {
            cerr << " reading bsp2 " << "[" << objName << "]" << endl;
            in >> steps;
            curves.push_back( evalBspline(cpsToAdd = readCps(in, 2), steps) );
            curveNames.push_back(objName);
            dims.push_back(2);
            if (named) curveIndex[objName] = (int)dims.size()-1;
        }
        else if (objType == "bez3")
        {
            cerr << " reading bez3 " << "[" << objName << "]" << endl;
            in >> steps;
            curves.push_back( evalBezier(cpsToAdd = readCps(in, 3), steps) );
            curveNames.push_back(objName);
            dims.push_back(3);
            if (named) curveIndex[objName] = (int)dims.size()-1;

        }
        else if (objType == "bsp3")
        {
            cerr << " reading bsp3 " << "[" << objName << "]" << endl;
            in >> steps;
            curves.push_back( evalBspline(cpsToAdd = readCps(in, 3), steps) );
            curveNames.push_back(objName);
            dims.push_back(3);
            if (named) curveIndex[objName] = (int)dims.size()-1;
        }
        else if (objType == "srev")
        {
            cerr << " reading srev " << "[" << objName << "]" << endl;
            in >> steps;

            // Name of the profile curve
            string profName;
            in >> profName;

            cerr << "  profile [" << profName << "]" << endl;
            
            map<string,unsigned>::const_iterator it = curveIndex.find(profName);

            // Failure checks
            if (it == curveIndex.end()) {                
                cerr << "failed: [" << profName << "] doesn't exist!" << endl; return false;
            }
            if (dims[it->second] != 2) {
                cerr << "failed: [" << profName << "] isn't 2d!" << endl; return false;
            }

            // Make the surface
            surfaces.push_back( makeSurfRev( curves[it->second], steps ) );
            surfaceNames.push_back(objName);
            if (named) surfaceIndex[objName] = (int)surfaceNames.size()-1;
        }
        else if (objType == "gcyl")
        {
            cerr << " reading gcyl " << "[" << objName << "]" << endl;
            
            // Name of the profile curve and sweep curve
            string profName, sweepName;
            in >> profName >> sweepName;

            cerr << "  profile [" << profName << "], sweep [" << sweepName << "]" << endl;

            map<string,unsigned>::const_iterator itP, itS;

            // Failure checks for profile
            itP = curveIndex.find(profName);
            
            if (itP == curveIndex.end()) {                
                cerr << "failed: [" << profName << "] doesn't exist!" << endl; return false;
            }
            if (dims[itP->second] != 2) {
                cerr << "failed: [" << profName << "] isn't 2d!" << endl; return false;
            }

            // Failure checks for sweep
            itS = curveIndex.find(sweepName);
            if (itS == curveIndex.end()) {                
                cerr << "failed: [" << sweepName << "] doesn't exist!" << endl; return false;
            }

            // Make the surface
            surfaces.push_back( makeGenCyl( curves[itP->second], curves[itS->second] ) );
            surfaceNames.push_back(objName);
            if (named) surfaceIndex[objName] = (int)surfaceNames.size()-1;

        }
        else if (objType == "circ")
        {
            cerr << " reading circ " << "[" << objName << "]" << endl;

            unsigned steps;
            float rad;
            in >> steps >> rad;
            cerr << "  radius [" << rad << "]" << endl;

            curves.push_back( evalCircle(rad, steps) );
            curveNames.push_back(objName);
            dims.push_back(2);
            if (named) curveIndex[objName] = (int)dims.size()-1;
        }
        else
        {
            cerr << "failed: type " << objType << " unrecognized." << endl;
            return false;
        }

        ctrlPoints.push_back(cpsToAdd);
    }

    return true;
}




