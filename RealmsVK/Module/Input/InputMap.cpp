#include "InputMap.h"

using namespace rlms;

void InputMap::bindKey (std::string name, int && key) {

	auto it = m_assignedInputs.find (name);

	//Entity exists
	if (it != m_assignedInputs.end ()) {
		unbindInput (name);
	}

	m_assignedInputs.insert (std::make_pair (name, new KeyInputAssignement (std::move (key))));
}

void InputMap::bindMouseButton (std::string name, int && button) {
	auto it = m_assignedInputs.find (name);

	//Assign exists
	if (it != m_assignedInputs.end ()) {
		unbindInput (name);
	}

	m_assignedInputs.insert (std::make_pair (name, new MouseButtonInputAssignement (std::move (button))));
}

void InputMap::bindKeySlide (std::string name, int && key) {

	auto it = m_assignedInputs.find (name);

	//Assign exists
	if (it != m_assignedInputs.end ()) {
		unbindInput (name);
	}

	m_assignedInputs.insert (std::make_pair (name, new KeyMouseSlideAssignement (std::move (key))));
}

void InputMap::bindMouseButtonSlide (std::string name, int && button) {
	auto it = m_assignedInputs.find (name);

	//Assign exists
	if (it != m_assignedInputs.end ()) {
		unbindInput (name);
	}

	m_assignedInputs.insert (std::make_pair (name, new MouseButtonSlideAssignement (std::move (button))));
}

//void rlms::InputMap::bindEvent (std::string const& name, sf::Event::EventType && e) {
//	auto it = m_assignedInputs.find (name);
//
//	//Assign exists
//	if (it != m_assignedInputs.end ()) {
//		unbindInput (name);
//	}
//
//	m_assignedInputs.insert (std::make_pair (name, new EventAssignement (std::move (e))));
//}

void InputMap::unbindInput (std::string name) {
	auto it = m_assignedInputs.find (name);

	//Assign exists
	if (it != m_assignedInputs.end ()) {
		it->second->~IAssignement ();
		delete it->second;
		m_assignedInputs.erase (name);
	}
}

void InputMap::clearInputs () {
	while (!m_assignedInputs.empty ()) {
		auto it = m_assignedInputs.begin ();
		it->second->~IAssignement ();
		delete it->second;
		m_assignedInputs.erase (it);
	}
}

void InputMap::update (Input &n) {
	if(m_active){
		for (auto it = m_assignedInputs.begin (); it != m_assignedInputs.end (); ++it) {
			it->second->update (n);
		}
	}
}

void rlms::InputMap::reset () {
	if (m_active) {
		for (auto it = m_assignedInputs.begin (); it != m_assignedInputs.end (); ++it) {
			it->second->reset ();
		}
	}
}

void rlms::InputMap::enable () {
	m_active = true;
}

void rlms::InputMap::disable () {
	m_active = false;
}

const bool& rlms::InputMap::active () {
	return m_active;
}

const bool InputMap::hasInput (std::string const& name) {
	return m_assignedInputs.find (name) != m_assignedInputs.end ();
}

const bool InputMap::isPressed (std::string const& name) const { //Action

	auto it = m_assignedInputs.find (name);

	if (it != m_assignedInputs.end ()) {
		return it->second->isPressed ();
	}
	return false;
}
const bool InputMap::isDown (std::string const& name) const { //State

	auto it = m_assignedInputs.find (name);

	if (it != m_assignedInputs.end ()) {
		return it->second->isDown ();
	}
	return false;
}
const bool InputMap::isReleased (std::string const& name) const { //State

	auto it = m_assignedInputs.find (name);

	if (it != m_assignedInputs.end ()) {
		return it->second->isReleased ();
	}
	return false;
}

const rlms::Vec2i rlms::InputMap::getStartPos (std::string const& name) const {
	auto it = m_assignedInputs.find (name);

	if (it != m_assignedInputs.end ()) {
		return it->second->getStartPos ();
	}
	return rlms::Vec2i();
}

const rlms::Vec2i InputMap::getDeltaPos (std::string const& name) const { //State

	auto it = m_assignedInputs.find (name);

	if (it != m_assignedInputs.end ()) {
		return it->second->getDeltaPos ();
	}
	return rlms::Vec2i ();
}

const rlms::Vec2i InputMap::getEndPos (std::string const& name) const { //State

	auto it = m_assignedInputs.find (name);

	if (it != m_assignedInputs.end ()) {
		return it->second->getEndPos ();
	}
	return rlms::Vec2i ();
}

//const sf::Event* InputMap::getEvent (std::string const& name) const { //State
//
//	auto it = m_assignedInputs.find (name);
//
//	if (it != m_assignedInputs.end ()) {
//		return it->second->getEvent ();
//	}
//	return nullptr;
//}