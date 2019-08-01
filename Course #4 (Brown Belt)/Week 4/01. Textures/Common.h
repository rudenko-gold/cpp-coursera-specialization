#pragma once

#include <string>
#include <vector>
#include <memory>

struct Point {
    int x;
    int y;
};

struct Size {
    int width;
    int height;
};

inline bool IsPointInEllipse(Point p, Size size) {
    double x = (p.x + 0.5) / (size.width / 2.0) - 1;
    double y = (p.y + 0.5) / (size.height / 2.0) - 1;
    return x * x + y * y <= 1;
}

using Image = std::vector<std::string>;

enum class ShapeType { Rectangle, Ellipse };

class ITexture {
public:
    virtual ~ITexture() = default;

    virtual Size GetSize() const = 0;
    virtual const Image& GetImage() const = 0;
};

class IShape {
public:
    virtual ~IShape() = default;

    virtual std::unique_ptr<IShape> Clone() const = 0;

    virtual void SetPosition(Point) = 0;
    virtual Point GetPosition() const = 0;

    virtual void SetSize(Size) = 0;
    virtual Size GetSize() const = 0;

    virtual void SetTexture(std::shared_ptr<ITexture>) = 0;
    virtual ITexture* GetTexture() const = 0;

    virtual void Draw(Image&) const = 0;
};

std::unique_ptr<IShape> MakeShape(ShapeType shape_type);
