#include "Bomb.h"
#include "Flame.h"
#include "Visitors.h"

void Bomb::visit(IObjectVisitor &visitor) {
	visitor(*this);
}

void Bomb::visit(IConstObjectVisitor &visitor) const {
	visitor(*this);
}

void Bomb::position(Position const &value) {
	position_ = value;
}

Position const &Bomb::position() const {
	return position_;
}

TextureRef const &Bomb::texture() const {
	return texture_;
}

void Bomb::texture(TextureRef const &texture) {
	texture_ = texture;
}

void Bomb::owner(Player *owner) {
	owner_ = owner;
}

void Bomb::gameTick() {
	GameObjectBase::gameTick();
	ticsTillExplosion_--;

	BombVisitor bombVisitor(*this, playersOnBomb);
	playground_.visitAll(bombVisitor, playground_.Overlapping(position_));

	if (!bombVisitor.canBeSteppedOn)
		canBeStepped = false;
	if (bombVisitor.isInsideFlame)
		ticsTillExplosion_ = 0;

	if (ticsTillExplosion_ == 0) {
		explode(); // Create flames
		if (owner_) {
			owner_->bombExploded();
		}
		playground_.remove(*this);
	}
}

Player *Bomb::owner() const {
	return owner_;
}

void Bomb::explode() {

	for (auto &dir : directions_) {
		for (int i = 1; i <= range; ++i) {
			Position flamePos = next(position_, dir, i);
			FlameVisitor flameVisitor;
			playground_.visitAll(flameVisitor, playground_.Overlapping(flamePos));

			if (playground_.isValid(flamePos) && !flameVisitor.isStopped) {
				Flame &fl = playground_.createFlame();
				fl.position(flamePos);
			} else
				break;
		}
	}

	Flame &fl = playground_.createFlame();
	fl.position(position_);
}
