#include <geometry/geometry.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <string>
#include <utility>
#include <stdexcept>

#include <algorithm>
#include <json/json.h>
#include <geometry/clipper.h>
#include <glm/glm.hpp>
#include <render/render.h>

using namespace std;

bool Geometry::between(double x, double min, double max)
{
    return x >= min && x <= max;
}
bool Geometry::points_match(glm::vec2 p1, glm::vec2 p2)
{
    float tolorance = 0.001;
	if (this->between(p1.x, p2.x - tolorance, p2.x + tolorance) && this->between(p1.y, p2.y - tolorance, p2.y + tolorance))
	{
		return true;
	}
	else
	{
		return false;
	}
}
nlohmann::json Geometry::arc_to_line_segments(double cx, double cy, double r, double start_angle, double end_angle, double num_segments)
{
    vector<Point> pointList;
	vector<Point> pointListOut; //List after simplification
    glm::vec2 start;
    glm::vec2 sweeper;
    glm::vec2 end;
    start.x = cx + (r * cosf((start_angle) * 3.1415926f / 180.0f));
	start.y = cy + (r * sinf((start_angle) * 3.1415926f / 180.0f));
    end.x = cx + (r * cosf((end_angle) * 3.1415926f / 180.0f));
	end.y = cy + (r * sinf((end_angle) * 3.1415926 / 180.0f));
    pointList.push_back(Point(start.x, start.y));
    double diff = MAX(start_angle, end_angle) - MIN(start_angle, end_angle);
	if (diff > 180) diff = 360 - diff;
	double angle_increment = diff / num_segments;
	double angle_pointer = start_angle + angle_increment;
    for (int i = 0; i < num_segments; i++)
	{
		sweeper.x = cx + (r * cosf((angle_pointer) * 3.1415926f / 180.0f));
		sweeper.y = cy + (r * sinf((angle_pointer) * 3.1415926f / 180.0f));
		angle_pointer += angle_increment;
        pointList.push_back(Point(sweeper.x, sweeper.y));
	}
    pointList.push_back(Point(end.x, end.y));
    this->RamerDouglasPeucker(pointList, 0.003, pointListOut);
    nlohmann::json geometry_stack;
    nlohmann::json line;
    for(size_t i=1; i< pointListOut.size(); i++)
	{
        line["type"] = "line";
        line["start"]["x"] = pointListOut[i-1].first;
        line["start"]["y"] = pointListOut[i-1].second;
        line["end"]["x"] = pointListOut[i].first;
        line["end"]["y"] = pointListOut[i].second;
        geometry_stack.push_back(line);
	}
    return geometry_stack;
}
nlohmann::json Geometry::circle_to_line_segments(double cx, double cy, double r)
{
    vector<Point> pointList;
	vector<Point> pointListOut; //List after simplification
    for(int i = 0; i < 360; i++)
    {
        double theta = 2.0f * 3.1415926f * i / 360.0f;//get the current angle
        double tx = r * cosf(theta);//calculate the x component
        double ty = r * sinf(theta);//calculate the y component
        pointList.push_back(Point((tx + cx), (ty + cy)));
    }
    this->RamerDouglasPeucker(pointList, 0.003, pointListOut);
    nlohmann::json geometry_stack;
    nlohmann::json line;
    for(size_t i=1; i< pointListOut.size(); i++)
	{
        line["type"] = "line";
        line["start"]["x"] = pointListOut[i-1].first;
        line["start"]["y"] = pointListOut[i-1].second;
        line["end"]["x"] = pointListOut[i].first;
        line["end"]["y"] = pointListOut[i].second;
        geometry_stack.push_back(line);
	}
    line["type"] = "line";
    line["start"]["x"] = pointListOut[pointListOut.size() -1].first;
    line["start"]["y"] = pointListOut[pointListOut.size() -1].second;
    line["end"]["x"] = pointListOut[0].first;
    line["end"]["y"] = pointListOut[0].second;
    geometry_stack.push_back(line);
    return geometry_stack;
}
nlohmann::json Geometry::normalize(nlohmann::json geometry_stack)
{
    nlohmann::json return_stack;
    for (nlohmann::json::iterator it = geometry_stack.begin(); it != geometry_stack.end(); ++it)
    {
        //std::cout << it.key() << " : " << it.value() << "\n";
        if (it.value()["type"] == "line")
        {
            return_stack.push_back(it.value());
        }
        else if (it.value()["type"] == "arc")
        {
            nlohmann::json line_segments = this->arc_to_line_segments(it.value()["center"]["x"], it.value()["center"]["y"], it.value()["radius"], it.value()["start_angle"], it.value()["end_angle"], 100);
            for (nlohmann::json::iterator sub_it = line_segments.begin(); sub_it != line_segments.end(); ++sub_it) return_stack.push_back(sub_it.value());
        }
        else if (it.value()["type"] == "circle")
        {
            nlohmann::json line_segments = this->circle_to_line_segments(it.value()["center"]["x"], it.value()["center"]["y"], it.value()["radius"]);
            for (nlohmann::json::iterator sub_it = line_segments.begin(); sub_it != line_segments.end(); ++sub_it) return_stack.push_back(sub_it.value());
        }
    }
    return return_stack;
}
nlohmann::json Geometry::get_extents(nlohmann::json geometry_stack)
{
    nlohmann::json extents;
    glm::vec2 min;
    min.x = 1000000000000;
    min.y = 1000000000000;
    glm::vec2 max;
    max.x = -1000000000000;
    max.y = -1000000000000;
    for (nlohmann::json::iterator it = geometry_stack.begin(); it != geometry_stack.end(); ++it)
    {
        if (it.value()["type"] == "line")
        {
            if (it.value()["start"]["x"] < min.x) min.x = it.value()["start"]["x"];
			if (it.value()["start"]["y"] < min.y) min.y = it.value()["start"]["y"];
            if (it.value()["end"]["x"] < min.x) min.x = it.value()["end"]["x"];
			if (it.value()["end"]["y"] < min.y) min.y = it.value()["end"]["y"];
            if (it.value()["start"]["x"] > max.x) max.x = it.value()["start"]["x"];
			if (it.value()["start"]["y"] > max.y) max.y = it.value()["start"]["y"];
            if (it.value()["end"]["x"] > max.x) max.x = it.value()["end"]["x"];
			if (it.value()["end"]["y"] > max.y) max.y = it.value()["end"]["y"];
        }
        else if (it.value()["type"] == "arc" || it.value()["type"] == "circle")
        {
            if (double(it.value()["center"]["x"]) - double(it.value()["radius"]) < min.x) min.x = double(it.value()["center"]["x"]) - double(it.value()["radius"]);
			if (double(it.value()["center"]["y"]) - double(it.value()["radius"]) < min.y) min.y = double(it.value()["center"]["y"]) - double(it.value()["radius"]);
			if (double(it.value()["center"]["x"]) + double(it.value()["radius"]) > max.x) max.x = double(it.value()["center"]["x"]) + double(it.value()["radius"]);
			if (double(it.value()["center"]["y"]) + double(it.value()["radius"]) > max.y) max.y = double(it.value()["center"]["x"]) + double(it.value()["radius"]);
        }
    }
    extents["min"]["x"] = min.x;
    extents["min"]["y"] = min.y;
    extents["max"]["x"] = max.x;
    extents["max"]["y"] = max.y;
    return extents;
}
glm::vec2 Geometry::rotate_point(glm::vec2 center, glm::vec2 point, double angle)
{
    glm::vec2 return_point;
    double radians = (3.1415926f / 180.0f) * angle;
	double cos = cosf(radians);
	double sin = sinf(radians);
	return_point.x = (cos * (point.x - center.x)) + (sin * (point.y - center.y)) + center.x;
	return_point.y = (cos * (point.y - center.y)) - (sin * (point.x - center.x)) + center.y;
    return return_point;
}
nlohmann::json Geometry::chainify(nlohmann::json geometry_stack)
{
    nlohmann::json contours;
    std::vector<line_t> haystack;
    nlohmann::json point;
    //Make a copy of geometry stack so we're not modifying it during the chaining process!
    for (nlohmann::json::iterator it = geometry_stack.begin(); it != geometry_stack.end(); ++it)
    {
        if (it.value()["type"] == "line")
        {
            line_t l;
            l.start.x = it.value()["start"]["x"];
            l.start.y = it.value()["start"]["y"];
            l.end.x = it.value()["end"]["x"];
            l.end.y = it.value()["end"]["y"];
            haystack.push_back(l);
        }
    }
    while(haystack.size() > 0)
    {
        nlohmann::json chain;
        glm::vec2 p1;
        glm::vec2 p2;
        line_t first = haystack.back();
        haystack.pop_back();
        point["x"] = first.start.x;
        point["y"] = first.start.y;
        chain.push_back(point);
        point["x"] = first.end.x;
        point["y"] = first.end.y;
        chain.push_back(point);
        bool didSomething;
        do{
            didSomething = false;
            nlohmann::json current_point = chain.back();
            p1.x = current_point["x"];
            p1.y = current_point["y"];
            for (int x = 0; x < haystack.size(); x++)
            {
                p2.x = haystack[x].start.x;
                p2.y = haystack[x].start.y;
                if (this->points_match(p1, p2))
                {
                    point["x"] = haystack[x].end.x;
                    point["y"] = haystack[x].end.y;
                    chain.push_back(point);
                    haystack.erase(haystack.begin()+x);
                    didSomething = true;
                    break;
                }
                p2.x = haystack[x].end.x;
                p2.y = haystack[x].end.y;
                if (this->points_match(p1, p2))
                {
                    point["x"] = haystack[x].start.x;
                    point["y"] = haystack[x].start.y;
                    chain.push_back(point);
                    haystack.erase(haystack.begin()+x);
                    didSomething = true;
                    break;
                }
            }
        } while(didSomething);
        if (chain.size() != 2)
        {
            contours.push_back(chain);
        }
    }
    return contours;
}
nlohmann::json Geometry::offset(nlohmann::json path, double offset)
{
    /*
        Only supports a stack of {x: xxxx, y: xxxx} points
    */
   nlohmann::json ret;
    ClipperLib::Path subj;
    ClipperLib::Paths solution;
    //printf("Path: %s, offset: %.4f\n", path.dump().c_str(), offset);
    for (nlohmann::json::iterator it = path.begin(); it != path.end(); ++it)
    {
        subj << ClipperLib::IntPoint(int((double(it.value()["x"]) * 100000.0f)), int((double(it.value()["y"]) * 100000.0f)));
    }
    ClipperLib::ClipperOffset co;
    co.AddPath(subj, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
    co.Execute(solution, offset * 100000.0f);
    for (int x = 0; x < solution.size(); x++)
    {
        //printf("Solution - %d\n", x);
        nlohmann::json path;
        glm::vec2 first_point;
        for (int y = 0; y < solution[x].size(); y++)
        {
        if (y == 0)
        {
            first_point.x =(double(solution[x][y].X) / 100000.0f);
            first_point.y = (double(solution[x][y].Y) / 100000.0f);
        }
        //printf("\t x: %.4f, y: %.4f\n", (float)(solution[x][y].X / 1000.0f), (float)(solution[x][y].Y / 1000.0f));
        nlohmann::json point;
        point["x"] = (double(solution[x][y].X) / 100000.0f);
        point["y"] = (double(solution[x][y].Y) / 100000.0f);
        path.push_back(point);
        }
        nlohmann::json point;
        point["x"] = first_point.x;
        point["y"] = first_point.y;
        path.push_back(point);
        ret.push_back(path);
    }
    return ret;
}
double Geometry::PerpendicularDistance(const Point &pt, const Point &lineStart, const Point &lineEnd)
{
	double dx = lineEnd.first - lineStart.first;
	double dy = lineEnd.second - lineStart.second;

	//Normalise
	double mag = pow(pow(dx,2.0)+pow(dy,2.0),0.5);
	if(mag > 0.0)
	{
		dx /= mag; dy /= mag;
	}

	double pvx = pt.first - lineStart.first;
	double pvy = pt.second - lineStart.second;

	//Get dot product (project pv onto normalized direction)
	double pvdot = dx * pvx + dy * pvy;

	//Scale line direction vector
	double dsx = pvdot * dx;
	double dsy = pvdot * dy;

	//Subtract this from pv
	double ax = pvx - dsx;
	double ay = pvy - dsy;

	return pow(pow(ax,2.0)+pow(ay,2.0),0.5);
}
void Geometry::RamerDouglasPeucker(const vector<Point> &pointList, double epsilon, vector<Point> &out)
{
	if(pointList.size()<2)
		throw invalid_argument("Not enough points to simplify");

	// Find the point with the maximum distance from line between start and end
	double dmax = 0.0;
	size_t index = 0;
	size_t end = pointList.size()-1;
	for(size_t i = 1; i < end; i++)
	{
		double d = this->PerpendicularDistance(pointList[i], pointList[0], pointList[end]);
		if (d > dmax)
		{
			index = i;
			dmax = d;
		}
	}

	// If max distance is greater than epsilon, recursively simplify
	if(dmax > epsilon)
	{
		// Recursive call
		vector<Point> recResults1;
		vector<Point> recResults2;
		vector<Point> firstLine(pointList.begin(), pointList.begin()+index+1);
		vector<Point> lastLine(pointList.begin()+index, pointList.end());
		RamerDouglasPeucker(firstLine, epsilon, recResults1);
		RamerDouglasPeucker(lastLine, epsilon, recResults2);
 
		// Build the result list
		out.assign(recResults1.begin(), recResults1.end()-1);
		out.insert(out.end(), recResults2.begin(), recResults2.end());
		if(out.size()<2)
			throw runtime_error("Problem assembling output");
	} 
	else 
	{
		//Just return start and end points
		out.clear();
		out.push_back(pointList[0]);
		out.push_back(pointList[end]);
	}
}