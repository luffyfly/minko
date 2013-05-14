#include "Node.hpp"
#include "minko/scene/controller/AbstractController.hpp"

using namespace minko::scene;

unsigned int Node::_id = 0;

Node::Node() :
	enable_shared_from_this(),
	_name("Node_" + std::to_string(Node::_id++)),
	_tags(1),
	_root(nullptr),
	_parent(nullptr),
	_bindings(DataBindings::create()),
	_added(Signal<ptr, ptr, ptr>::create()),
	_removed(Signal<ptr, ptr, ptr>::create()),
	_controllerAdded(Signal<ptr, ptr, Node::AbsCtrlPtr>::create()),
	_controllerRemoved(Signal<ptr, ptr, Node::AbsCtrlPtr>::create()),
	_tagsChanged(Signal<ptr, ptr>::create())
{
}

Node::ptr
Node::addChild(Node::ptr child)
{
	if (child->_parent)
		child->_parent->removeChild(child);

	_children.push_back(child);

	child->_parent = shared_from_this();
	child->updateRoot();

	// bubble down
	auto descendants = NodeSet::create(child)->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_added->execute(descendant, child, shared_from_this());

	// bubble up
	auto ancestors = NodeSet::create(shared_from_this())->ancestors(true);
	for (auto ancestor : ancestors->nodes())
		ancestor->_added->execute(ancestor, child, shared_from_this());

	return shared_from_this();
}

Node::ptr
Node::removeChild(Node::ptr child)
{
	std::list<Node::ptr>::iterator it = std::find(_children.begin(), _children.end(), child);

	if (it == _children.end())
		throw std::invalid_argument("child");

	_children.erase(it);

	child->_parent = nullptr;
	child->updateRoot();

	// bubble down
	auto descendants = NodeSet::create(child)->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_removed->execute(descendant, child, shared_from_this());

	// bubble up
	auto ancestors = NodeSet::create(shared_from_this())->ancestors(true);
	for (auto ancestor : ancestors->nodes())
		ancestor->_removed->execute(ancestor, child, shared_from_this());

	return shared_from_this();
}

bool
Node::contains(Node::ptr node)
{
	return std::find(_children.begin(), _children.end(), node) != _children.end();
}

Node::ptr
Node::addController(std::shared_ptr<AbstractController> controller)
{
	if (hasController(controller))
		throw std::logic_error("The same controller cannot be added twice.");

	_controllers.push_back(controller);
	controller->_targets.push_back(shared_from_this());

	// bubble down
	auto descendants = NodeSet::create(shared_from_this())->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_controllerAdded->execute(descendant, shared_from_this(), controller);

	// bubble up
	auto ancestors = NodeSet::create(shared_from_this())->ancestors();
	for (auto ancestor : ancestors->nodes())
		ancestor->_controllerAdded->execute(ancestor, shared_from_this(), controller);

	controller->targetAdded()->execute(controller, shared_from_this());

	return shared_from_this();
}

Node::ptr
Node::removeController(std::shared_ptr<AbstractController> controller)
{
	std::list<AbstractController::ptr>::iterator it = std::find(
		_controllers.begin(), _controllers.end(), controller
	);

	if (it == _controllers.end())
		throw std::invalid_argument("controller");

	_controllers.erase(it);
	controller->_targets.erase(
		std::find(controller->_targets.begin(), controller->_targets.end(), shared_from_this())
	);

	// bubble down
	auto descendants = NodeSet::create(shared_from_this())->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_controllerRemoved->execute(descendant, shared_from_this(), controller);

	// bubble up
	auto ancestors = NodeSet::create(shared_from_this())->ancestors();
	for (auto ancestor : ancestors->nodes())
		ancestor->_controllerRemoved->execute(ancestor, shared_from_this(), controller);

	controller->targetRemoved()->execute(controller, shared_from_this());

	return shared_from_this();
}

bool
Node::hasController(std::shared_ptr<AbstractController> controller)
{
	return std::find(_controllers.begin(), _controllers.end(), controller) != _controllers.end();
}

void
Node::updateRoot()
{
	_root = _parent ? (_parent->_root ? _parent->_root : _parent) : shared_from_this();

	for (auto child : _children)
		child->updateRoot();
}
