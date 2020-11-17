#include "Flame.h"
#include "Visitors.h"

void Flame::visit(IObjectVisitor &visitor) {
	visitor(*this);
}

void Flame::visit(IConstObjectVisitor &visitor) const {
	visitor(*this);
}

void Flame::position(Position const &value) {
	position_ = value;
}

Position const &Flame::position() const {
	return position_;
}

TextureRef const &Flame::texture() const {
	return texture_;
}

void Flame::texture(TextureRef const &texture) {
	texture_ = texture;
}

void Flame::gameTick() {
	GameObjectBase::gameTick();
	if (ticsLeft_-- && ticsLeft_ == 0)
		playground_.remove(*this);
}
