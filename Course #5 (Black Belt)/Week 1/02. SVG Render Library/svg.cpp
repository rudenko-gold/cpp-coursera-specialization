#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <iostream>

namespace Svg {
    struct Point {
        Point() {}
        Point(double x, double y) : x(x), y(y) {}
        std::string to_string() const {
            return std::to_string(x) + "," + std::to_string(y);
        }

        double x, y;
    };

    struct Rgb {
        Rgb() {}
        Rgb(unsigned char red, unsigned char green, unsigned char blue) : red(red), green(green), blue(blue) {}

        std::string to_string() const {
            return "rgb(" + std::to_string(red) + "," + std::to_string(green) + "," + std::to_string(blue) + ")";
        }

        unsigned char red, green, blue;
    };


    struct Color {
        Color() : is_none(true) {}
        Color(std::string value) : is_none(false), is_rgb_type(false), str_val(std::move(value)) {}
        Color(Rgb value) : is_none(false), is_rgb_type(true), rgb_val(value) {}

        std::string Render() const {
            if (is_none) {
                return "none";
            } else {
                if (is_rgb_type) {
                    return rgb_val.to_string();
                } else {
                    return str_val;
                }
            }
        }

        std::string str_val;
        Rgb rgb_val;
        bool is_rgb_type;
        bool is_none;
    };

    const Color NoneColor = Color();

    class Circle {
    public:
        Circle() : fill(NoneColor), stroke(NoneColor), stroke_width(1.0), center(0.0, 0.0), radius(1.0) {}

        Circle SetCenter(Point point) {
            center = point;
            return *this;
        }

        Circle SetRadius(double value) {
            radius = value;
            return *this;
        }

        Circle SetFillColor(std::string color) {
            fill = std::move(color);
            return *this;
        }

        Circle SetStrokeColor(std::string color) {
            stroke = std::move(color);
            return *this;
        }

        Circle SetFillColor(Color color) {
            fill = std::move(color);
            return *this;
        }

        Circle SetStrokeColor(Color color) {
            stroke = std::move(color);
            return *this;
        }

        Circle SetFillColor(Rgb color) {
            fill = color;
            return *this;
        }

        Circle SetStrokeColor(Rgb color) {
            stroke = color;
            return *this;
        }

        Circle SetStrokeWidth(double width) {
            stroke_width = width;
            return *this;
        }

        Circle SetStrokeLineCap(const std::string& value) {
            stroke_linecap = value;
            return *this;
        }

        Circle SetStrokeLineJoin(const std::string& value) {
            stroke_linejoin = value;
            return *this;
        }

        std::string Render() const {
            std::string result = "";

            result += "<circle ";

            result += "cx=\"" + std::to_string(center.x) + "\" ";
            result += "cy=\"" + std::to_string(center.y) + "\" ";
            result += "r=\"" + std::to_string(radius) + "\" ";

            result += "fill=\"" + fill.Render() + "\" ";
            result += "stroke=\"" + stroke.Render() + "\" ";
            result += "stroke-width=\"" + std::to_string(stroke_width) + "\" ";

            if (!stroke_linecap.empty()) {
                result += "stroke-linecap=\"" + stroke_linecap + "\" ";
            }
            if (!stroke_linejoin.empty()) {
                result += "stroke-linejoin=\"" + stroke_linecap + "\" ";
            }

            result += "/>";

            return result;
        }
    private:
        Color fill;
        Color stroke;
        double stroke_width;
        std::string stroke_linecap;
        std::string stroke_linejoin;
        Point center;
        double radius;
    };


    class Polyline {
    public:
        Polyline() : fill(NoneColor), stroke(NoneColor), stroke_width(1.0) {}

        Polyline AddPoint(Point point) {
            points.push_back(point);
            return *this;
        }

        Polyline SetFillColor(std::string color) {
            fill = std::move(color);
            return *this;
        }

        Polyline SetStrokeColor(std::string color) {
            stroke = std::move(color);
            return *this;
        }

        Polyline SetFillColor(Rgb color) {
            fill = color;
            return *this;
        }

        Polyline SetStrokeColor(Rgb color) {
            stroke = color;
            return *this;
        }

        Polyline SetFillColor(Color color) {
            fill = std::move(color);
            return *this;
        }

        Polyline SetStrokeColor(Color color) {
            stroke = std::move(color);
            return *this;
        }



        Polyline SetStrokeWidth(double width) {
            stroke_width = width;
            return *this;
        }

        Polyline SetStrokeLineCap(const std::string& value) {
            stroke_linecap = value;
            return *this;
        }

        Polyline SetStrokeLineJoin(const std::string& value) {
            stroke_linejoin = value;
            return *this;
        }

        std::string Render() const {
            std::string result = "";

            result += "<polyline ";

            result += "points=\"";

            for (auto& point : points) {
                result += point.to_string() + " ";
            }

            result += "\" ";

            result += "fill=\"" + fill.Render() + "\" ";
            result += "stroke=\"" + stroke.Render() + "\" ";
            result += "stroke-width=\"" + std::to_string(stroke_width) + "\" ";

            if (!stroke_linecap.empty()) {
                result += "stroke-linecap=\"" + stroke_linecap + "\" ";
            }
            if (!stroke_linejoin.empty()) {
                result += "stroke-linejoin=\"" + stroke_linecap + "\" ";
            }

            result += "/>";
            return result;
        }
    private:
        Color fill;
        Color stroke;
        double stroke_width;
        std::string stroke_linecap;
        std::string stroke_linejoin;
        std::vector<Point> points;
    };

    class Text {
    public:
        Text() : fill(NoneColor), stroke(NoneColor), stroke_width(1.0), center(0.0, 0.0), offset(0.0, 0.0), font_size(1), data("") {}

        Text SetPoint(Point point) {
            center = point;
            return *this;
        }

        Text SetOffset(Point point) {
            offset = point;
            return *this;
        }

        Text SetFontSize(uint32_t size) {
            font_size = size;
            return *this;
        }

        Text SetFontFamily(const std::string& value) {
            font_family = value;
            return *this;
        }

        Text SetData(const std::string& value) {
            data = value;
            return *this;
        }

        Text SetFillColor(std::string color) {
            fill = std::move(color);
            return *this;
        }

        Text SetFillColor(Rgb color) {
            fill = color;
            return *this;
        }

        Text SetFillColor(Color color) {
            fill = std::move(color);
            return *this;
        }

        Text SetStrokeColor(std::string color) {
            stroke = std::move(color);
            return *this;
        }

        Text SetStrokeColor(Rgb color) {
            stroke = color;
            return *this;
        }

        Text SetStrokeColor(Color color) {
            stroke = std::move(color);
            return *this;
        }

        Text SetStrokeWidth(double width) {
            stroke_width = width;
            return *this;
        }

        Text SetStrokeLineCap(const std::string& value) {
            stroke_linecap = value;
            return *this;
        }

        Text SetStrokeLineJoin(const std::string& value) {
            stroke_linejoin = value;
            return *this;
        }

        std::string Render() const {
            std::string result = "";

            result += "<text ";

            result += "x=\"" + std::to_string(center.x) + "\" ";
            result += "y=\"" + std::to_string(center.y) + "\" ";
            result += "dx=\"" + std::to_string(offset.x) + "\" ";
            result += "dy=\"" + std::to_string(offset.y) + "\" ";
            result += "font-size=\"" + std::to_string(font_size) + "\" ";

            if (!font_family.empty()) {
                result += "font-family=\"" + font_family + "\" ";
            }

            result += "fill=\"" + fill.Render() + "\" ";
            result += "stroke=\"" + stroke.Render() + "\" ";
            result += "stroke-width=\"" + std::to_string(stroke_width) + "\" ";

            if (!stroke_linecap.empty()) {
                result += "stroke-linecap=\"" + stroke_linecap + "\" ";
            }
            if (!stroke_linejoin.empty()) {
                result += "stroke-linejoin=\"" + stroke_linecap + "\" ";
            }

            result += ">";
            result += data;
            result += "</text>";

            return result;
        }

    private:
        Color fill;
        Color stroke;
        double stroke_width;
        std::string stroke_linecap;
        std::string stroke_linejoin;
        Point center;
        Point offset;
        std::uint32_t font_size;
        std::string font_family;
        std::string data;
    };

    class Document {
    public:
        Document() = default;

        void Add(const Circle& circle) {
            objects += circle.Render();
        }

        void Add(const Polyline& polyline) {
            objects += polyline.Render();
        }

        void Add(const Text& text) {
            objects += text.Render();
        }

        void Render(std::ostream& os) const {
            std::string result = "";
            result += "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
            result += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";

            result += objects;

            result += "</svg>";
            os << result;
        }
    private:
        std::string objects;
    };
};

int main() {
    // Example
    Svg::Document svg;

    svg.Add(
            Svg::Polyline{}
                    .SetStrokeColor(Svg::Rgb{140, 198, 63})  // soft green
                    .SetStrokeWidth(16)
                    .SetStrokeLineCap("round")
                    .AddPoint({50, 50})
                    .AddPoint({250, 250})
    );

    for (const auto point : {Svg::Point{50, 50}, Svg::Point{250, 250}}) {
        svg.Add(
                Svg::Circle{}
                        .SetFillColor("white")
                        .SetRadius(6)
                        .SetCenter(point)
        );
    }

    svg.Add(
            Svg::Text{}
                    .SetPoint({50, 50})
                    .SetOffset({10, -10})
                    .SetFontSize(20)
                    .SetFontFamily("Verdana")
                    .SetFillColor("black")
                    .SetData("C")
    );
    svg.Add(
            Svg::Text{}
                    .SetPoint({250, 250})
                    .SetOffset({10, -10})
                    .SetFontSize(20)
                    .SetFontFamily("Verdana")
                    .SetFillColor("black")
                    .SetData("C++")
    );

    svg.Render(std::cout);
    
    return 0;
}
