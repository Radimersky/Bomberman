#include "BonusItem.h"
#include "Visitors.h"

void BonusItem::visit(IObjectVisitor &visitor) {
	visitor(*this);
}

void BonusItem::visit(IConstObjectVisitor &visitor) const {
	visitor(*this);
}

void BonusItem::position(Position const &value) {
	position_ = value;
}

Position const &BonusItem::position() const {
	return position_;
}

TextureRef const &BonusItem::texture() const {
	return texture_;
}

void BonusItem::texture(TextureRef const &texture) {
	texture_ = texture;
}

void BonusItem::bonus(Bonus value) {
	bonus_ = value;
}

void BonusItem::gameTick() {
	GameObjectBase::gameTick();
}

Bonus BonusItem::bonus() const {
	return bonus_;
}

void BonusItem::destroy(bool now) {
	if (now)
		playground_.removeNow(*this);
	else
		playground_.remove(*this);
}
