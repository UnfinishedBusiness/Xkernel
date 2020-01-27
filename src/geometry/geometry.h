#ifndef GEOMETRY_H
#define GEOMETRY_H

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include <string>
#include <json/json.h>
#include <geometry/clipper.h>
#include <glm/glm.hpp>
/*********************
 *      DEFINES
 *********************/
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
/**********************
 *      TYPEDEFS
 **********************/
typedef std::pair<double, double> Point;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
class Geometry{
    public:
        /*
            Check if number is bewteen a given min/max range
        */
        bool between(double x, double min, double max);

        /*
            Check if both points match
        */
        bool points_match(glm::vec2 p1, glm::vec2 p2);

        /*
            Output line segments from described arc
        */
        nlohmann::json arc_to_line_segments(double cx, double cy, double r, double start_angle, double end_angle, double num_segments);

        /*
            Output line segments from described circle
        */
        nlohmann::json circle_to_line_segments(double cx, double cy, double r);

        /*
            Input a json stack of lines, arc, circles, etc and Output a stack with non-lines converted to line segments.
            ! Does not reguire stack to be vectorized !
        */
        nlohmann::json normalize(nlohmann::json geometry_stack);

        /*
            Input a json stack of lines, arcs, circles and return a json object
            min/max = {x: xxx, max: xxx};
            return {min, max};
        */
        nlohmann::json get_extents(nlohmann::json geometry_stack);

        /*
            Rotate a point around a specified point by angle in degrees
            Returns new point with rotation applied
        */
        glm::vec2 rotate_point(glm::vec2 center, glm::vec2 point, double angle);

        /*
            Chainify line segments
            Inputs a "normalized" (meaning only line segments) stack of entities and vectors them aka chaining
            Returns an array of contours
        */
        nlohmann::json chainify(nlohmann::json geometry_stack);

        /*
            Offset contour
            Inputs a list of points representing a closed contour and a distance to offset
                negative offsets offset inside and positive offsets offset to the outside
            Returns a list of points with offset applied
        */
        nlohmann::json offset(nlohmann::json path, double offset);

        /*
            Ramer Douglas Peucker Algorythm for simplifying points
        */
        void RamerDouglasPeucker(const std::vector<Point> &pointList, double epsilon, std::vector<Point> &out);


    private:
        /*
            Used by RamerDouglasPeucker method
        */
        double PerpendicularDistance(const Point &pt, const Point &lineStart, const Point &lineEnd);
};

/**********************
 * VIEWER PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif