#include "Player.h"
#include "Visitors.h"

class Flame;

void Player::visit(IObjectVisitor& visitor) {
	visitor(*this);
}

void Player::visit(IConstObjectVisitor& visitor) const {
	visitor(*this);
}

void Player::position(Position const& value) {
	position_ = value;
}

Position const& Player::position() const {
	return position_;
}

TextureRef const& Player::texture() const {
	return texture_;
}

void Player::texture(TextureRef const& texture) {
	texture_ = texture;
}

void Player::move(Direction dir, int tics) {
	movePlayer.first = tics;
	if (isPoisoned_) {
		movePlayer.second = opposite(dir);
	} else {
		movePlayer.second = dir;
	}
	isMoving_ = true;
}

bool Player::isMoving() const {
	return isMoving_;
}

void Player::bomb() {
	if (activeBombs_ < maxBombs_) {
		Position futureBombPos = round(position_);
		BombPlant visitor;
		playground_.visitAll(visitor, playground_.Overlapping(futureBombPos));
		if (visitor.canPlantBomb) {
			createBomb();
			activeBombs_++;
		}
	}
}

bool Player::isDead() const {
	return isDead_;
}

void Player::move(const Direction& dir) {
	if (isMoving_) {
		position(position() + toVector(dir));
	}
}

void Player::gameTick() {
	GameObjectBase::gameTick();

	Position futurePos = position_ + toVector(movePlayer.second);
	PlayerMove visitor(plantedBombs);
	playground_.visitAll(visitor, playground_.Overlapping(futurePos));

	for (auto& bmb : plantedBombs) { // Controls if player that planted the bomb can move on that bomb
		if (bmb->owner() != this || !bmb->canBeStepped) {
			visitor.isFreeToMove = false;
		}
	}

	PlayerOnBomb onBomb;
	playground_.visitAll(onBomb, playground_.Overlapping(position_));

	if (onBomb.canBeSteppedOn && plantedBombs.size() == 1 && !visitor.facingWall) // Controls if enemy player can move on bomb that is not his
		visitor.isFreeToMove = true;

	plantedBombs.clear();

	if (visitor.isFreeToMove && playground_.isValid(futurePos) && isMoving_ && !isDead_) { // Perform move
		move(movePlayer.second);
	} else if (!isCentered(position_)) { // If player may possibly go around square
		futurePos = round(position_) + toVector(movePlayer.second); //futurePos = position where player should go when circumventing.
		PlayerMoveCircumventing visitorCirc;
		playground_.visitAll(visitorCirc, playground_.Overlapping(futurePos));
		if (visitorCirc.isFreeToMove && playground_.isValid(futurePos) && isMoving_ && !isDead_) {
			move(circumventingDir(position_, round(position_)));
		}
	}

	movePlayer.first--; // substract from remaining move tics
	if (movePlayer.first <= 0)
		isMoving_ = false;

	handleBonus();

	if (onBomb.isKilled) {
		playground_.killPlayer(*this);
		isDead_ = true;
		return; // In case some more code is added beneath this statement
	}
}

Direction Player::circumventingDir(const Position& current, const Position& rounded) const {
	Vector position = current.from - rounded.from;
	if (position.y <= -1)
		return Direction::Down;
	if (position.y >= 1)
		return Direction::Up;
	if (position.x <= -1)
		return Direction::Right;
	if (position.x >= 1)
		return Direction::Left;
	return Direction::None;
}

void Player::bombExploded() {
	activeBombs_--;
}

void Player::createBomb() {
	Bomb& bomb = playground_.createBomb();
	bomb.position(round(position_));
	bomb.range = bombRange_;
	bomb.owner(this);
	plantedBombs.push_back(&bomb);
}

void Player::handleBonus() {
	BonusPickup visitorBonus;
	playground_.visitAll(visitorBonus, playground_.Overlapping(position_));
	if (visitorBonus.bonus == Bonus::BombMax) {
		maxBombs_++;
	} else if (visitorBonus.bonus == Bonus::BombRange) {
		bombRange_++;
	} else if (visitorBonus.bonus == Bonus::Poison) {
		isPoisoned_ = true;
		poisonTicksLeft_ = Config::poisonTime + 1;
	}

	if (isPoisoned_)
		poisonTicksLeft_--;
	if (poisonTicksLeft_ == 0)
		isPoisoned_ = false;
}
