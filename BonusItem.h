#pragma once

#include "Bonus.h"
#include "GameObjectBase.h"
#include "Playground.h"

enum class Bonus;

class BonusItem : public GameObjectBase {
public:
	BonusItem(Playground &playground, Bonus bonus)
	    : playground_(playground)
	    , bonus_(bonus){};

	/* < IGameObject > */
	void visit(IObjectVisitor &visitor) override;
	void visit(IConstObjectVisitor &visitor) const override;
	void position(Position const &value) override;
	Position const &position() const override;
	TextureRef const &texture() const override;
	void texture(TextureRef const &texture) override;
	/* </IGameObject > */

	/**
	 * @brief Sets the kind of bonus.
	 */
	void bonus(Bonus value);

	Bonus bonus() const;

	void gameTick() override;

	void destroy(bool now);

	bool dostroySelf = false;

private:
	Position position_{};
	TextureRef texture_;
	Playground &playground_;
	Bonus bonus_ = Bonus::None;
};
