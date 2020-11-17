#pragma once

#include "Bonus.h"
#include "IGameObject.h"
#include <memory> // unique_ptr

class Wall;
class Flame;
class Player;
class Bomb;
class BonusItem;
enum class Bonus;
class Playground;

/**
 * When constructing players, bricks and other objects,
 * they have to be parametrized with texture (and potentially other things).
 * This class knows which object has which texture,
 * so that the ones who wants to create new objects does not have to
 * know it.
 */
class GameObjectFactory {
public:
	/**
	 * The constructor must have exactly those parameters.
	 */
	explicit GameObjectFactory(Playground& playground);

	std::unique_ptr<Wall> createBrick() const;
	std::unique_ptr<Wall> createStone() const;
	std::unique_ptr<Flame> createFlame() const;

	std::unique_ptr<Player> createPlayer() const;
	std::unique_ptr<Bomb> createBomb() const;
	std::unique_ptr<BonusItem> createBonusItem(Bonus bonus) const;

private:
	Playground& playground_;
};
