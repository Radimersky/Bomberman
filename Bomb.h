#pragma once

#include "Config.h"
#include "GameObjectBase.h"
#include "Player.h"
#include "Playground.h"

class Bomb : public GameObjectBase {
public:
	Bomb(Playground &playground)
	    : playground_(playground){};

	/* < IGameObject > */
	void visit(IObjectVisitor &visitor) override;
	void visit(IConstObjectVisitor &visitor) const override;
	void position(Position const &value) override;
	Position const &position() const override;
	TextureRef const &texture() const override;
	void texture(TextureRef const &texture) override;
	/* </IGameObject > */
	void gameTick() override;

	void owner(Player *owner);

	Player *owner() const;

	int range = 1;
	bool canBeStepped = true;
	std::vector<Player *> playersOnBomb{};

private:
	Player *owner_{};
	Position position_{};
	TextureRef texture_;
	Playground &playground_;
	int ticsTillExplosion_ = Config::bombTime;
	std::vector<Direction> directions_{ Direction::Left, Direction::Right, Direction::Up, Direction::Down };

	void explode();
};