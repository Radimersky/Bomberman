#pragma once

#include "Geometry.h"
#include "IGameObject.h"
#include "TextureRef.h"
#include <utility> // move

/**
 * This is a simple base class for all game objects.
 * Here we can implement the common features of all game objects.
 * It can significantly reduce the amount of code,
 * because the concrete classes then do not have to implement
 * trivial or unneeded methods.
 *
 * Feel free to implement trivial methods directly in this declaration.
 */

class GameObjectBase : public IGameObject {
public:
	/*
	 * This is intentionally implemented in a trivial manner,
	 * so that a game object that does not need to implement it
	 * does not have to.
	 */
	void gameTick() override {}

	/*
	 * This is intentionally not implemented, because
	 * we want concrete objects to implement it themselves.
	 */
	// void visit(IObjectVisitor & visitor) override {}
	// void visit(IConstObjectVisitor & visitor) const override {}
};
