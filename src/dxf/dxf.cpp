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
#include <geometry/spline/Bezier.h>

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
    if (creationClass->current_spline.points.size() > 0)
    {
        creationClass->splines.push_back(creationClass->current_spline); //Push last spline
        creationClass->current_spline.points.clear();
    }

    
    for (int x = 0; x < creationClass->splines.size(); x++)
    {
        std::vector<Point> pointList;
        std::vector<Point> out;
        Curve* curve = new Bezier();
	    curve->set_steps(100);
        //printf("Iterating spline(%d)\n", x);
        for (int y = 0; y < creationClass->splines[x].points.size(); y++)
        {
            //printf("\tAdding point to curve (%.4f, %.4f)\n", creationClass->splines[x].points[y].x, creationClass->splines[x].points[y].y);
            curve->add_way_point(Vector(creationClass->splines[x].points[y].x, creationClass->splines[x].points[y].y, 0));
            /*nlohmann::json j;
            j["type"] = "circle";
            j["center"]["x"] = creationClass->splines[x].points[y].x;
            j["center"]["y"] = creationClass->splines[x].points[y].y;
            j["radius"] = 0.030;
            creationClass->dxfJSON.push_back(j);*/
            //pointList.push_back(Point(creationClass->splines[x].points[y].x, creationClass->splines[x].points[y].y));
        }
        for (int i = 0; i < curve->node_count(); i++)
        {
            pointList.push_back(Point(curve->node(i).x, curve->node(i).y));
        }
        g.RamerDouglasPeucker(pointList, 0.003, out);
        for (int i = 1; i < out.size(); i++)
        {
            //std::cout << "node #" << i << ": X: " << curve->node(i).x << " Y:" << curve->node(i).y << " (length so far: " << curve->length_from_starting_point(i) << ")" << std::endl;
            nlohmann::json line;
            line["layer"] = "spline-" + std::to_string(x);
            line["type"] = "line";
            //line["color"]["r"] = 0;
            //line["color"]["g"] = 1;
            //line["color"]["b"] = 0;
            line["start"]["x"] = (double)out[i-1].first;
            line["start"]["y"] = (double)out[i-1].second;
            line["start"]["z"] = (double)0;
            line["end"]["x"] = (double)out[i].first;
            line["end"]["y"] = (double)out[i].second;
            line["end"]["z"] = (double)0;
            creationClass->dxfJSON.push_back(line);
        }
        if (creationClass->splines[x].isClosed == true)
        {
            nlohmann::json line;
            line["layer"] = "spline-" + std::to_string(x);
            line["type"] = "line";
            //line["color"]["r"] = 0;
            //line["color"]["g"] = 1;
            //line["color"]["b"] = 0;
            line["start"]["x"] = (double)curve->node(0).x;
            line["start"]["y"] = (double)curve->node(0).y;
            line["start"]["z"] = (double)0;
            line["end"]["x"] = (double)curve->node(curve->node_count()-1).x;
            line["end"]["y"] = (double)curve->node(curve->node_count()-1).y;
            line["end"]["z"] = (double)0;
            creationClass->dxfJSON.push_back(line);
        }
        delete curve;
    }
    for (int x = 0; x < creationClass->polylines.size(); x++)
    {
        //printf("Iterating polyline(%d)\n", x);
        for (int y = 0; y < creationClass->polylines[x].points.size()-1; y++)
        {
            //printf("\t Vertex-> point(%.4f, %.4f) bulge: %.4f\n", creationClass->polylines[x].points[y].point.x, creationClass->polylines[x].points[y].point.y, creationClass->polylines[x].points[y].bulge);
            /*nlohmann::json j;
            j["layer"] = "polyline-" + std::to_string(x);
            j["type"] = "circle";
            if (y == 0)
            {
                j["color"]["r"] = 1;
                j["color"]["g"] = 1;
                j["color"]["b"] = 0;
            }
            j["radius"] = 0.005;
            j["center"]["x"] = creationClass->polylines[x].points[y].point.x;
            j["center"]["y"] = creationClass->polylines[x].points[y].point.y;
            creationClass->dxfJSON.push_back(j);*/

            if (creationClass->polylines[x].points[y].bulge != 0)
            {
                nlohmann::json arc;
                glm::vec2 bulgeStart;
                bulgeStart.x = creationClass->polylines[x].points[y].point.x;
                bulgeStart.y = creationClass->polylines[x].points[y].point.y;
				glm::vec2 bulgeEnd;
                bulgeEnd.x = creationClass->polylines[x].points[y + 1].point.x;
                bulgeEnd.y = creationClass->polylines[x].points[y + 1].point.y;
				glm::vec2 midpoint = g.midpoint(bulgeStart, bulgeEnd);
				double distance = g.distance(bulgeStart, midpoint);
				double sagitta = creationClass->polylines[x].points[y].bulge * distance;
                if (sagitta > 0)
                {
                    line_t bulgeLine = g.create_polar_line(midpoint, g.measure_polar_angle(bulgeStart, bulgeEnd) + 270, sagitta);
                    glm::vec2 arc_center = g.three_point_circle_center(bulgeStart, bulgeLine.end, bulgeEnd);
                    double arc_endAngle = g.measure_polar_angle(arc_center, bulgeEnd);
                    double arc_startAngle = g.measure_polar_angle(arc_center, bulgeStart);
                    nlohmann::json arc;
                    arc["layer"] = "polyline-" + std::to_string(x);
                    arc["type"] = "arc";
                    /*if (creationClass->polylines[x].isClosed == true)
                    {
                        arc["color"]["r"] = 1;
                        arc["color"]["g"] = 1;
                        arc["color"]["b"] = 1;
                    }
                    else
                    {
                        arc["color"]["r"] = 0;
                        arc["color"]["g"] = 1;
                        arc["color"]["b"] = 0;
                    }*/
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
                    arc["layer"] = "polyline-" + std::to_string(x);
                    arc["type"] = "arc";
                    /*if (creationClass->polylines[x].isClosed == true)
                    {
                        arc["color"]["r"] = 1;
                        arc["color"]["g"] = 1;
                        arc["color"]["b"] = 1;
                    }
                    else
                    {
                        arc["color"]["r"] = 0;
                        arc["color"]["g"] = 1;
                        arc["color"]["b"] = 0;
                    }*/
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
                line["layer"] = "polyline-" + std::to_string(x);
                line["type"] = "line";
                /*if (creationClass->polylines[x].isClosed == true)
                {
                    line["color"]["r"] = 1;
                    line["color"]["g"] = 1;
                    line["color"]["b"] = 1;
                }
                else
                {
                    line["color"]["r"] = 0;
                    line["color"]["g"] = 1;
                    line["color"]["b"] = 0;
                }*/
                line["start"]["x"] = (double)creationClass->polylines[x].points[y].point.x;
                line["start"]["y"] = (double)creationClass->polylines[x].points[y].point.y;
                line["start"]["z"] = (double)0;
                line["end"]["x"] = (double)creationClass->polylines[x].points[y+1].point.x;
                line["end"]["y"] = (double)creationClass->polylines[x].points[y+1].point.y;
                line["end"]["z"] = (double)0;
                creationClass->dxfJSON.push_back(line);
            }
            
        }
        /*nlohmann::json j;
        j["layer"] = "polyline-" + std::to_string(x);
        j["type"] = "circle";
        j["radius"] = 0.003;
        j["color"]["r"] = 0;
        j["color"]["g"] = 1;
        j["color"]["b"] = 0;
        j["center"]["x"] = creationClass->polylines[x].points.back().point.x;
        j["center"]["y"] = creationClass->polylines[x].points.back().point.y;
        creationClass->dxfJSON.push_back(j);*/
        //printf("First point: (%.4f, %.4f) Last Point: (%.4f, %.4f)\n", creationClass->polylines[x].points[0].point.x, creationClass->polylines[x].points[0].point.y, creationClass->polylines[x].points[creationClass->polylines[x].points.size()-1].point.x, creationClass->polylines[x].points[creationClass->polylines[x].points.size()-1].point.y);
        //printf("Start-Endpoint Distance: %.4f\n", g.distance(creationClass->polylines[x].points[0].point, creationClass->polylines[x].points[creationClass->polylines[x].points.size()-1].point));

        //if (creationClass->polylines[x].isClosed == true)
        {
            //Create a closing line only if our endpoint or startpoint is NOT shared by any other entities endpoint or start point
            int shared = 0; //Assume we are not shared
            glm::vec2 our_endpoint = creationClass->polylines[x].points.back().point;
            glm::vec2 our_startpoint = creationClass->polylines[x].points.front().point;
            for (nlohmann::json::iterator it = creationClass->dxfJSON.begin(); it != creationClass->dxfJSON.end(); ++it)
            {
                if (it.value()["type"] == "line")
                {
                    if (it.value()["start"]["x"] == our_endpoint.x && it.value()["start"]["y"] == our_endpoint.y)
                    {
                        shared++;
                    }
                    if (it.value()["end"]["x"] == our_endpoint.x && it.value()["end"]["y"] == our_endpoint.y)
                    {
                        shared++;
                    }
                    if (it.value()["start"]["x"] == our_startpoint.x && it.value()["start"]["y"] == our_startpoint.y)
                    {
                        shared++;
                    }
                    if (it.value()["end"]["x"] == our_startpoint.x && it.value()["end"]["y"] == our_startpoint.y)
                    {
                        shared++;
                    }
                }
                else if (it.value()["type"] == "arc")
                {
                    glm::vec2 center;
                    center.x = it.value()["center"]["x"];
                    center.y = it.value()["center"]["y"];
                    glm::vec2 start_point = g.create_polar_line(center, it.value()["start_angle"], it.value()["radius"]).end;
                    glm::vec2 end_point = g.create_polar_line(center, it.value()["end_angle"], it.value()["radius"]).end;
                    if (start_point.x == our_endpoint.x && start_point.y == our_endpoint.y)
                    {
                        shared++;
                    }
                    if (end_point.x == our_endpoint.x && end_point.y == our_endpoint.y)
                    {
                        shared++;
                    }
                    if (start_point.x == our_startpoint.x && start_point.y == our_startpoint.y)
                    {
                        shared++;
                    }
                    if (end_point.x == our_startpoint.x && end_point.y == our_startpoint.y)
                    {
                        shared++;
                    }
                }
            }
            //printf("Shared: %d\n", shared);
            if (shared == 2)
            {
                nlohmann::json line;
                line["layer"] = "polyline-" + std::to_string(x);
                line["type"] = "line";
                /*line["color"]["r"] = 0.5;
                line["color"]["g"] = 0.5;
                line["color"]["b"] = 0.5;*/
                line["start"]["x"] = (double)creationClass->polylines[x].points.front().point.x;
                line["start"]["y"] = (double)creationClass->polylines[x].points.front().point.y;
                line["start"]["z"] = (double)0;
                line["end"]["x"] = (double)creationClass->polylines[x].points.back().point.x;
                line["end"]["y"] = (double)creationClass->polylines[x].points.back().point.y;
                line["end"]["z"] = (double)0;
                creationClass->dxfJSON.push_back(line);
            }
        }
    }
    //delete creationClass;
    return creationClass->dxfJSON;
}