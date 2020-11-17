#include "Wall.h"
#include "Visitors.h"

void Wall::visit(IObjectVisitor &visitor) {
	visitor(*this);
}

void Wall::visit(IConstObjectVisitor &visitor) const {
	visitor(*this);
}

void Wall::position(Position const &value) {
	position_ = value;
}

Position const &Wall::position() const {
	return position_;
}

TextureRef const &Wall::texture() const {
	return texture_;
}

void Wall::texture(TextureRef const &texture) {
	texture_ = texture;
}

void Wall::bonus(Bonus value) {
	bonus_ = value;
}

void Wall::gameTick() {
	GameObjectBase::gameTick();
}

void Wall::destroy() {
	if (bonus_ != Bonus::None) {
		BonusItem &bonusItem = playground_.createBonusItem(bonus_);
		bonusItem.position(position_);
	}
	playground_.remove(*this);
}

bool Wall::destroyable() const {
	return destroyable_;
}

void Wall::destroyable(bool value) {
	destroyable_ = value;
}
