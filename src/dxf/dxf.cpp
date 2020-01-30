#include <dxf/dxf.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <dxflib/dl_dxf.h>
#include <dxflib/dl_creationadapter.h>
#include <dxf/DXFParse_Class.h>
#include <json/json.h>
#include <geometry/geometry.h>
#include <glm/glm.hpp>

nlohmann::json DXF::readDXF(const char* file)
{
    Geometry g;
    // Load DXF file into memory:
    //std::cout << "Reading file " << file << "...\n";
    DXFParse_Class* creationClass = new DXFParse_Class();
    DL_Dxf* dxf = new DL_Dxf();
    if (!dxf->in(file, creationClass)) { // if file open failed
        std::cerr << file << " could not be opened.\n";
        return "{}";
    }
    delete dxf;
    /*
        Convert polylines to arc and line segments and append to dxfJSON
    */
    if (creationClass->current_polyline.points.size() > 0)
    {
        creationClass->polylines.push_back(creationClass->current_polyline); //Push last polyline
        creationClass->current_polyline.points.clear();
    }
    /*line_t l;
    l.start.x = 0;
    l.start.y = 0;
    l = g.create_polar_line(l.start, 45, 10);
    nlohmann::json line;
    line["layer"] = "test";
    line["type"] = "line";
    line["start"]["x"] = (double)l.start.x;
    line["start"]["y"] = (double)l.start.y;
    line["start"]["z"] = (double)0;
    line["end"]["x"] = (double)l.end.x;
    line["end"]["y"] = (double)l.end.y;
    line["end"]["z"] = (double)0;
    creationClass->dxfJSON.push_back(line);
    printf("Test line is at %.4f polar degrees\n", g.measure_polar_angle(l.start, l.end));*/
    for (int x = 0; x < creationClass->polylines.size(); x++)
    {
        //printf("Iterating polyline(%d)\n", x);
        for (int y = 0; y < creationClass->polylines[x].points.size(); y++)
        {
            //printf("\t Vertex-> point(%.4f, %.4f) bulge: %.4f\n", creationClass->polylines[x].points[y].point.x, creationClass->polylines[x].points[y].point.y, creationClass->polylines[x].points[y].bulge);
            if (creationClass->polylines[x].points[y].bulge != 0)
            {
                nlohmann::json arc;
                glm::vec2 bulgeStart;
                bulgeStart.x = creationClass->polylines[x].points[y].point.x;
                bulgeStart.y = creationClass->polylines[x].points[y].point.y;

                /*nlohmann::json circle;
                circle["layer"] = "test";
                circle["type"] = "circle";
                circle["center"]["x"] = (double)bulgeStart.x;
                circle["center"]["y"] = (double)bulgeStart.y;
                circle["radius"] = 0.020;
                creationClass->dxfJSON.push_back(circle);*/

				glm::vec2 bulgeEnd;
                if (y < creationClass->polylines[x].points.size()-1)
                {
                    bulgeEnd.x = creationClass->polylines[x].points[y + 1].point.x;
                    bulgeEnd.y = creationClass->polylines[x].points[y + 1].point.y;
                }
                else
                {
                    bulgeEnd.x = creationClass->polylines[x].points[0].point.x;
                    bulgeEnd.y = creationClass->polylines[x].points[0].point.y;
                }
                //nlohmann::json circle;
                /*circle["layer"] = "test";
                circle["type"] = "circle";
                circle["center"]["x"] = (double)bulgeEnd.x;
                circle["center"]["y"] = (double)bulgeEnd.y;
                circle["radius"] = 0.020;
                creationClass->dxfJSON.push_back(circle);*/

				glm::vec2 midpoint = g.midpoint(bulgeStart, bulgeEnd);

                //nlohmann::json circle;
                /*circle["layer"] = "test";
                circle["type"] = "circle";
                circle["center"]["x"] = (double)midpoint.x;
                circle["center"]["y"] = (double)midpoint.y;
                circle["radius"] = 0.020;
                creationClass->dxfJSON.push_back(circle);*/

				double distance = g.distance(bulgeStart, midpoint);
				double sagitta = creationClass->polylines[x].points[y].bulge * distance;
                if (sagitta > 0)
                {
                    line_t bulgeLine = g.create_polar_line(midpoint, g.measure_polar_angle(bulgeStart, bulgeEnd) + 270, sagitta);
                    
                    /*nlohmann::json line;
                    line["layer"] = "test";
                    line["type"] = "line";
                    line["start"]["x"] = (double)bulgeLine.start.x;
                    line["start"]["y"] = (double)bulgeLine.start.y;
                    line["start"]["z"] = (double)0;
                    line["end"]["x"] = (double)bulgeLine.end.x;
                    line["end"]["y"] = (double)bulgeLine.end.y;
                    line["end"]["z"] = (double)0;
                    creationClass->dxfJSON.push_back(line);*/
                    
                    glm::vec2 arc_center = g.three_point_circle_center(bulgeStart, bulgeLine.end, bulgeEnd);

                    //nlohmann::json circle;
                    /*circle["layer"] = "test";
                    circle["type"] = "circle";
                    circle["center"]["x"] = (double)arc_center.x;
                    circle["center"]["y"] = (double)arc_center.y;
                    circle["radius"] = 0.020;
                    creationClass->dxfJSON.push_back(circle);*/

                    double arc_endAngle = g.measure_polar_angle(arc_center, bulgeEnd);
                    double arc_startAngle = g.measure_polar_angle(arc_center, bulgeStart);
                    nlohmann::json arc;
                    arc["layer"] = std::to_string(x);
                    arc["type"] = "arc";
                    arc["center"]["x"] = (double)arc_center.x;
                    arc["center"]["y"] = (double)arc_center.y;
                    arc["start_angle"] = (double)arc_startAngle;
                    arc["end_angle"] = (double)arc_endAngle;
                    arc["radius"] = g.distance(arc_center, bulgeStart);
                    creationClass->dxfJSON.push_back(arc);
                }
                else
                {
                    line_t bulgeLine = g.create_polar_line(midpoint, g.measure_polar_angle(bulgeStart, bulgeEnd) + 270, sagitta);
                    glm::vec2 arc_center = g.three_point_circle_center(bulgeStart, bulgeLine.end, bulgeEnd);
                    //printf("\t\t(s-)Center: (%.4f, %.4f)\n", arc_center.x, arc_center.y);
                    double arc_endAngle = g.measure_polar_angle(arc_center, bulgeStart);
                    double arc_startAngle = g.measure_polar_angle(arc_center, bulgeEnd);
                    nlohmann::json arc;
                    arc["layer"] = std::to_string(x);
                    arc["type"] = "arc";
                    arc["center"]["x"] = (double)arc_center.x;
                    arc["center"]["y"] = (double)arc_center.y;
                    arc["start_angle"] = (double)arc_startAngle;
                    arc["end_angle"] = (double)arc_endAngle;
                    arc["radius"] = g.distance(arc_center, bulgeStart);
                    creationClass->dxfJSON.push_back(arc);
                }
            }
            else
            {
                nlohmann::json line;
                line["layer"] = std::to_string(x);
                line["type"] = "line";
                line["start"]["x"] = (double)creationClass->polylines[x].points[y].point.x;
                line["start"]["y"] = (double)creationClass->polylines[x].points[y].point.y;
                line["start"]["z"] = (double)0;
                if (y < creationClass->polylines[x].points.size()-1)
                {
                    line["end"]["x"] = (double)creationClass->polylines[x].points[y+1].point.x;
                    line["end"]["y"] = (double)creationClass->polylines[x].points[y+1].point.y;
                    line["end"]["z"] = (double)0;
                }
                else
                {
                    line["end"]["x"] = (double)creationClass->polylines[x].points[0].point.x;
                    line["end"]["y"] = (double)creationClass->polylines[x].points[0].point.y;
                    line["end"]["z"] = (double)0;
                }
                creationClass->dxfJSON.push_back(line);
            }
            
        }
    }
    //delete creationClass;
    return creationClass->dxfJSON;
}