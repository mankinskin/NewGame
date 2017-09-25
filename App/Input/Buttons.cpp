#include "../Global/stdafx.h"
#include "Buttons.h"
#include "Mouse.h"
#include <OpenGL\GUI\GUI.h>
#include <OpenGL\BaseGL\Framebuffer.h>
std::vector<App::Input::ButtonCondition> App::Input::allButtonStates;
std::vector<App::Input::KeyEvent> App::Input::mouseButtonEventBuffer;
std::vector<App::Input::ButtonEvent> App::Input::buttonEventBuffer;
std::vector<int> App::Input::allButtonFlags;
std::vector<unsigned int> App::Input::allButtonQuads;


unsigned int App::Input::addButton(unsigned int pQuadIndex)
{
        allButtonQuads.push_back(pQuadIndex);
        return allButtonQuads.size() - 1;
}
void App::Input::toggleButton(unsigned int pButtonIndex) {
        allButtonFlags[pButtonIndex] = !allButtonFlags[pButtonIndex];
}
void App::Input::hideButton(unsigned int pButtonIndex) {
        allButtonFlags[pButtonIndex] = 0;
}
void App::Input::unhideButton(unsigned int pButtonIndex) {
        allButtonFlags[pButtonIndex] = 1;
}
void App::Input::fetchButtonEvents()
{
        size_t button_count = allButtonQuads.size();
        std::vector<ButtonEvent> evnts(button_count);
        unsigned int evnt_count = 0;
        std::vector<unsigned int> buttonIndexFrame(gl::screenWidth * gl::screenHeight);
        glGetTextureImage(gl::Texture::buttonIndexTexture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, gl::screenWidth * gl::screenHeight, &buttonIndexFrame[0]);
        unsigned int cursor_button_index = buttonIndexFrame[gl::screenWidth * absoluteCursorPosition.y + absoluteCursorPosition.x];
        
        if (!evnt_count)
                return;
        buttonEventBuffer.resize(evnt_count);
        std::memcpy(&buttonEventBuffer[0], &evnts[0], sizeof(ButtonEvent)*evnt_count);
        
}
void App::Input::checkButtonEvents()
{
        size_t buttonEventCount = buttonEventBuffer.size();
        unsigned int signalOffset = signalBuffer.size();
        signalBuffer.resize(signalOffset + buttonEventCount);
        unsigned int signalCount = 0;
        for (unsigned int e = 0; e < buttonEventCount; ++e) {
                ButtonEvent& kev = buttonEventBuffer[e];
                for (unsigned int ks = 0; ks < EventSlot<ButtonEvent>::instance_count(); ++ks) {
                        EventSlot<ButtonEvent>& slot = EventSlot<ButtonEvent>::get(ks);
                        if (slot.evnt == kev) {
                                signalBuffer[signalOffset + signalCount++] = slot.signal_index;
                        }
                }

        }
        buttonEventBuffer.clear();
        signalBuffer.resize(signalOffset + signalCount);
}

void App::Input::clearButtons()
{
        allButtonFlags.clear();
        allButtonQuads.clear();
        allButtonStates.clear();
}

