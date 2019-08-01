#include "Common.h"

using namespace std;

class EllipseShape : public  IShape {
public:
    std::unique_ptr<IShape> Clone() const override {
        unique_ptr<EllipseShape> clonedShape = make_unique<EllipseShape>();

        clonedShape.get()->texture = this->texture;
        clonedShape.get()->position = this->position;
        clonedShape.get()->size = this->size;

        return clonedShape;
    }

    void SetPosition(Point position) override  {
        this->position = position;
    }

    Point GetPosition() const override {
        return this->position;
    }

    void SetSize(Size size) override {
        this->size = size;
    }

    Size GetSize() const override {
        return this->size;
    }

    void SetTexture(std::shared_ptr<ITexture> texture) override {
        this->texture = texture;
    }

    ITexture* GetTexture() const override {
        return texture.get();
    }

    void Draw(Image& image) const override {
        for (int y = 0; y < size.height; ++y) {
            for (int x = 0; x < size.width; ++x) {
                if (IsPointInEllipse({x, y}, size)) {
                    if (texture &&
                        x < texture.get()->GetSize().width &&
                        y < texture.get()->GetSize().height) {
                        image[position.y + y][position.x + x] = texture.get()->GetImage()[y][x];
                    } else {
                        image[position.y + y][position.x + x] = '.';
                    }
                }
            }
        }
    }
private:
    std::shared_ptr<ITexture> texture;
    Point position;
    Size size;
};

class RectangleShape : public  IShape {
public:
    std::unique_ptr<IShape> Clone() const override {
        unique_ptr<RectangleShape> clonedShape = make_unique<RectangleShape>();

        clonedShape.get()->texture = this->texture;
        clonedShape.get()->position = this->position;
        clonedShape.get()->size = this->size;

        return clonedShape;
    }

    void SetPosition(Point position) override  {
        this->position = position;
    }

    Point GetPosition() const override {
        return this->position;
    }

    void SetSize(Size size) override {
        this->size = size;
    }

    Size GetSize() const override {
        return this->size;
    }

    void SetTexture(std::shared_ptr<ITexture> texture) override {
        this->texture = texture;
    }

    ITexture* GetTexture() const override {
        return texture.get();
    }

    void Draw(Image& image) const override {
        for (int y = 0; y < size.height; ++y) {
            for (int x = 0; x < size.width; ++x) {
                if (position.y + y < image.size() &&
                    position.x + x < image[position.y + y].size()) {
                    if (texture &&
                        x < texture.get()->GetSize().width &&
                        y < texture.get()->GetSize().height) {
                        image[position.y + y][position.x + x] = texture.get()->GetImage()[y][x];
                    } else {
                        image[position.y + y][position.x + x] = '.';
                    }
                }
            }
        }
    }

private:
    std::shared_ptr<ITexture> texture;
    Point position;
    Size size;
};

unique_ptr<IShape> MakeShape(ShapeType shape_type) {
    switch (shape_type) {
        case ShapeType::Rectangle:
            return make_unique<RectangleShape>();
        case ShapeType::Ellipse:
            return make_unique<EllipseShape>();
        default:
            return make_unique<RectangleShape>();
    }
}
