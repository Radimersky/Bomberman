#pragma once

// A collection of useful visitors.
#include "Bomb.h"
#include "BonusItem.h"
#include "ObjectVisitorSkeleton.h"
#include "Player.h"
#include "Wall.h"

struct DeadPlayerDetector : ConstObjectVisitorSkeleton {
	bool detected = false;
	void operator()(Player const& player) override { detected = detected || player.isDead(); }
};

struct PlayerOnBomb : ConstObjectVisitorSkeleton {
	bool canBeSteppedOn = false;
	bool isKilled = false;
	void operator()(const Bomb&) override {
		canBeSteppedOn = true;
	}
	void operator()(const Flame&) override {
		isKilled = true;
	}
};

struct PlayerMove : ConstObjectVisitorSkeleton {
	PlayerMove(std::vector<const Bomb*>& bombs)
	    : bombs_(bombs) {}
	std::vector<const Bomb*>& bombs_;
	bool isFreeToMove = true;
	bool facingWall = false;

	void operator()(const Wall&) override {
		isFreeToMove = false;
		facingWall = true;
	}

	void operator()(const Bomb& bomb) override {
		bombs_.push_back(&bomb);
	}
};

struct BombPlant : ConstObjectVisitorSkeleton {
	bool canPlantBomb = true;
	void operator()(const Bomb&) override {
		canPlantBomb = false;
	}
};

struct PlayerMoveCircumventing : ConstObjectVisitorSkeleton {
	bool isFreeToMove = true;

	void operator()(const Wall&) override {
		isFreeToMove = false;
	}

	void operator()(const Bomb&) override {
		isFreeToMove = false;
	}
};

struct FlameVisitor : ObjectVisitorSkeleton {
	bool isStopped = false;

	void operator()(Wall& wall) override {
		isStopped = true;
		if (wall.destroyable()) {
			wall.destroy();
		}
	}

	void operator()(BonusItem& item) override {
		item.destroy(false);
	}
};

struct BombVisitor : ObjectVisitorSkeleton {
	BombVisitor(const Bomb& bomb, std::vector<Player*>& players)
	    : bomb_(bomb)
	    , playersOnBomb_(players) {}

	const Bomb& bomb_;
	std::vector<Player*>& playersOnBomb_;
	bool isInsideFlame = false, canBeSteppedOn = false;

	void operator()(Flame&) override {
		isInsideFlame = true;
	}

	void operator()(Player& player) override {
		playersOnBomb_.push_back(&player);
		if (bomb_.owner() == &player) {
			canBeSteppedOn = true;
		}
	}
};

struct BonusPickup : ObjectVisitorSkeleton {
	Bonus bonus = Bonus::None;
	void operator()(BonusItem& item) override {
		bonus = item.bonus();
		item.destroy(true);
	}
};
