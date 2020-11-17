#pragma once

#include "Bonus.h"
#include "GameObjectBase.h"
#include "Playground.h"

enum class Bonus;

// (brick, wood, stone, ...)
class Wall : public GameObjectBase {
public:
	Wall(Playground &playground, bool destroyable)
	    : playground_(playground)
	    , destroyable_(destroyable){};

	/* < IGameObject > */
	void visit(IObjectVisitor &visitor) override;

	void visit(IConstObjectVisitor &visitor) const override;

	void position(Position const &value) override;

	Position const &position() const override;

	TextureRef const &texture() const override;

	void texture(TextureRef const &texture) override;

	/* </IGameObject > */
	void gameTick() override;

	/**
	 * @brief Sets the kind of bonus which drops from the wall when destroyed.
	 */
	void bonus(Bonus value);

	/**
	 * @brief Sets whether the wall is destroyable or not.
	 */
	void destroyable(bool value);

	void destroy();

	bool destroyable() const;

private:
	Position position_{};
	TextureRef texture_;
	Playground &playground_;
	bool destroyable_ = false;
	Bonus bonus_ = Bonus::None;
};