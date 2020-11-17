#pragma once

#include "GameObjectBase.h"
#include "Playground.h"

class Player : public GameObjectBase {
public:
	Player(Playground &playground)
	    : playground_(playground){};

	/* < IGameObject > */
	void visit(IObjectVisitor &visitor) override;
	void visit(IConstObjectVisitor &visitor) const override;
	void position(Position const &value) override;
	Position const &position() const override;
	TextureRef const &texture() const override;
	void texture(TextureRef const &texture) override;
	/* </IGameObject > */

	/**
	 * Player should start moving in the given direction.
	 * In every game tick (that is, in every call to gameTick() method),
	 * the player should move by an unit vector in the given direction.
	 * However, if there is an obstacle which can be avoided, during the
	 * game tick the player moves by an unit vector according to the
	 * obstacle avoiding algorithm. See the specification for more detail.
	 */
	void move(Direction direction, int ticks);

	/**
	 * @brief Returns true if player is moving.
	 * The player is moving since `Player::move` is called
	 * until the `ticks`-th call of gameTick().
	 */
	bool isMoving() const;

	/**
	 * @brief Drops a bomb on player's current position.
	 */
	void bomb();

	/**
	 * @brief Returns true if player is dead.
	 */
	bool isDead() const;

	void gameTick() override;

	void bombExploded();

	std::vector<const Bomb *> plantedBombs{};

private:
	Position position_{};
	TextureRef texture_;
	Playground &playground_;
	void move(const Direction &direction);
	Direction circumventingDir(const Position &current, const Position &rounded) const;
	std::pair<int, Direction> movePlayer{}; //Stores amount of ticks left a direction
	bool isMoving_ = false;
	bool isDead_ = false;
	bool isPoisoned_ = false;
	int activeBombs_ = 0;
	int maxBombs_ = 1;
	int bombRange_ = 1;
	int poisonTicksLeft_ = 0;
	void createBomb();
	/**
     * Checks if player is standing on bonus and handles it
     */
	void handleBonus();
};
