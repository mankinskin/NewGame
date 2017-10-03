#pragma once
#include "Signal.h"
namespace App {
        namespace Input {
		template<typename EventType>
		struct ClickInterface {
			ClickInterface(EventType pClickEvent)
				:click_event(EventSlot<EventType>(pClickEvent)){}
			EventSlot<EventType> click_event;
		};
		template<typename EventType>
		struct ToggleInterface {
			ToggleInterface(EventType pOnEvent, EventType pOffEvent)
				:on_event(EventSlot<EventType>(pOnEvent)), off_event(EventSlot<EventType>(pOffEvent)) {}
			EventSlot<EventType> on_event;
			EventSlot<EventType> off_event;
		};
		struct Vec3Control {
			ToggleInterface<KeyEvent> x_plus;//toggle x_plus on/off
			ToggleInterface<KeyEvent> x_minus;//a
		
			ToggleInterface<KeyEvent> y_plus;//space
			ToggleInterface<KeyEvent> y_minus;//shift/y
		
			ToggleInterface<KeyEvent> z_plus;//s
			ToggleInterface<KeyEvent> z_minus;//w
		
			FuncSlot<void> x_plus_func;
			FuncSlot<void> x_minus_func;
			FuncSlot<void> x_stop_func;
		
			FuncSlot<void> y_plus_func;
			FuncSlot<void> y_minus_func;
			FuncSlot<void> y_stop_func;
		
			FuncSlot<void> z_plus_func;
			FuncSlot<void> z_minus_func;
			FuncSlot<void> z_stop_func;
		};
        }
}