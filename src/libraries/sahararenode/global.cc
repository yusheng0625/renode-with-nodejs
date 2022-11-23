/*
 * global.cc
 *
 *  Created on: Dec 23, 2021
 *      Author: keny
 */

#include "global.h"
Global* Global::m_inst = nullptr;

Global::Global() {
	_started = false;
}

Global::~Global() {
	//delete _circuit;
}

Global* Global::instance()
{
	if(m_inst == nullptr)
	{
		m_inst = new Global();
	}
	return m_inst;
}
