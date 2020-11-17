#include "Bomb.h"
#include "BonusItem.h"
#include "Flame.h"
#include "Player.h"
#include "Wall.h"

#include "GameObjectFactory.h"

GameObjectFactory::GameObjectFactory(Playground& playground)
    : playground_(playground) {
}

std::unique_ptr<Wall> GameObjectFactory::createBrick() const {
	// Brick is a destroyable wall.
	// It can be destroed by an explosion of a bomb.

	// Set those textures on the objects created there.
	TextureRef texture{ "brick_gray0.png" };

	auto ptrWall = std::make_unique<Wall>(playground_, true);
	ptrWall->texture(texture);
	return ptrWall;
}

std::unique_ptr<Wall> GameObjectFactory::createStone() const {
	// Stone is a non-destroyable wall.
	TextureRef texture{ "stone2_gray0.png" };

	auto ptrWall = std::make_unique<Wall>(playground_, false);
	ptrWall->texture(texture);
	return ptrWall;
}

std::unique_ptr<BonusItem> GameObjectFactory::createBonusItem(Bonus bonus) const {
	TextureRef texture = [&] {
		switch (bonus) {
		case Bonus::Poison:
			return TextureRef{ "potion_bubbly.png" };
		default:
			return TextureRef{ "sticky_flame.png" };
		}
	}();
	auto ptrBonus = std::make_unique<BonusItem>(playground_, bonus);
	ptrBonus->texture(texture);
	return ptrBonus;
}

std::unique_ptr<Flame> GameObjectFactory::createFlame() const {
	TextureRef texture{ "conjure_flame.png" };
	auto ptrFlame = std::make_unique<Flame>(playground_);
	ptrFlame->texture(texture);
	return ptrFlame;
}

std::unique_ptr<Player> GameObjectFactory::createPlayer() const {
	TextureRef texture{ "human.png" };
	auto ptrPlayer = std::make_unique<Player>(playground_);
	ptrPlayer->texture(texture);
	return ptrPlayer;
}

std::unique_ptr<Bomb> GameObjectFactory::createBomb() const {
	TextureRef texture{ "delayed_fireball.png" };
	auto ptrBomb = std::make_unique<Bomb>(playground_);
	ptrBomb->texture(texture);
	return ptrBomb;
}
