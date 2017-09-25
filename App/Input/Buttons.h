#pragma once
#include "Input.h"

namespace App {
        namespace Input {

                struct Detector {
                        unsigned int quad;
                };
                struct ButtonCondition {
                        ButtonCondition()
                                :action(0), // 0 = release, 1 = press
                                in(0) {}
                        ButtonCondition(int pAction, int pIn)
                                :action(pAction),
                                in(pIn)
                        {}
                        int action;
                        int in;
                };
                inline bool operator==(ButtonCondition const & l, ButtonCondition const& r) {
                        return l.action == r.action && l.in == r.in;
                }
                inline bool operator!=(ButtonCondition const & l, ButtonCondition const& r) {
                        return l.action != r.action || l.in != r.in;
                }

                class ButtonEvent {
                        public:
                                ButtonEvent()
                                        :button(-1), change(ButtonCondition()) {}
                                ButtonEvent(unsigned pButton, ButtonCondition pChange)
                                        :button(pButton), change(pChange) {}
                                ButtonEvent(unsigned pButton, unsigned pAction, unsigned pIn)
                                        :button(pButton), change(ButtonCondition(pAction, pIn)) {}

                                unsigned button;
                                ButtonCondition change;
                };
                
                inline bool operator==(ButtonEvent const & l, ButtonEvent const& r) {
                        return l.button == r.button && l.change == r.change;
                }
                
                //Buttons
                unsigned int addButton(unsigned int pQuadIndex);
                void hideButton(unsigned int pButtonIndex);
                void unhideButton(unsigned int pButtonIndex);
                void toggleButton(unsigned int pButtonIndex);
                
                

                void clearButtons();
                void fetchButtonEvents();
                void checkButtonEvents();


                extern std::vector<ButtonEvent> buttonEventBuffer;
                extern std::vector<unsigned int> allButtonQuads;
                extern std::vector<int> allButtonFlags;
                extern std::vector<ButtonCondition> allButtonStates;
                extern std::vector<EventSlot<ButtonEvent>> allButtonSignals;
        }
}
