#pragma once
#include "plugin.hpp"
#include <thread>


template < typename MODULE, typename BASE = ModuleWidget >
struct ThemedModuleWidget : BASE {
	MODULE* module;
	std::string baseName;
	SvgPanel* darkPanel;

	ThemedModuleWidget(MODULE* module, std::string baseName) {
		this->module = module;
		this->baseName = baseName;

		if (module) {
			BASE::setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/" + baseName + ".svg")));
			darkPanel = new SvgPanel();
			darkPanel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/dark/" + baseName + ".svg")));
			darkPanel->visible = false;
			BASE::addChild(darkPanel);
		}
		else {
			if (pluginSettings.panelThemeDefault == 0)
				BASE::setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/" + baseName + ".svg")));
			if (pluginSettings.panelThemeDefault == 1)
				BASE::setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/dark/" + baseName + ".svg")));
		}
	}

	void appendContextMenu(Menu* menu) override {
		struct ManualItem : MenuItem {
			std::string baseName;
			void onAction(const event::Action& e) override {
				std::thread t(system::openBrowser, "https://github.com/stoermelder/vcvrack-packone/blob/v1/docs/" + baseName + ".md");
				t.detach();
			}
		};

		struct PanelMenuItem : MenuItem {
			MODULE* module;

			PanelMenuItem() {
				rightText = RIGHT_ARROW;
			}

			Menu* createChildMenu() override {
				struct PanelThemeItem : MenuItem {
					MODULE* module;
					int theme;

					void onAction(const event::Action& e) override {
						module->panelTheme = module->panelTheme == theme ? 0 : theme;
					}
					void step() override {
						rightText = module->panelTheme == theme ? "✔" : "";
						MenuItem::step();
					}
				};

				struct PanelThemeDefaultItem : MenuItem {
					int theme;

					void onAction(const event::Action& e) override {
						pluginSettings.panelThemeDefault = pluginSettings.panelThemeDefault == theme ? 0 : theme;
						pluginSettings.saveToJson();
					}
					void step() override {
						rightText = pluginSettings.panelThemeDefault == theme ? "✔" : "";
						MenuItem::step();
					}
				};

				Menu* menu = new Menu;
				menu->addChild(construct<PanelThemeItem>(&MenuItem::text, "Dark panel", &PanelThemeItem::module, module, &PanelThemeItem::theme, 1));
				menu->addChild(construct<PanelThemeDefaultItem>(&MenuItem::text, "Dark panel as default", &PanelThemeDefaultItem::theme, 1));
				return menu;
			}
		};

		menu->addChild(construct<ManualItem>(&MenuItem::text, "Module Manual", &ManualItem::baseName, baseName));
		menu->addChild(new MenuSeparator());
		menu->addChild(construct<PanelMenuItem>(&MenuItem::text, "Panel", &PanelMenuItem::module, module));
		BASE::appendContextMenu(menu);
	}

	void step() override {
		if (module) {
			BASE::panel->visible = module->panelTheme == 0;
			darkPanel->visible  = module->panelTheme == 1;
		}
		BASE::step();
	}
};


struct LongPressButton {
	enum Event {
		NO_PRESS,
		SHORT_PRESS,
		LONG_PRESS
	};

	Param* param;
	float pressedTime = 0.f;
	dsp::BooleanTrigger trigger;

	inline Event process(float sampleTime) {
		Event result = NO_PRESS;
		bool pressed = param->value > 0.f;
		if (pressed && pressedTime >= 0.f) {
			pressedTime += sampleTime;
			if (pressedTime >= 1.f) {
				pressedTime = -1.f;
				result = LONG_PRESS;
			}
		}

		// Check if released
		if (trigger.process(!pressed)) {
			if (pressedTime >= 0.f) {
				result = SHORT_PRESS;
			}
			pressedTime = 0.f;
		}

		return result;
	}
};


template <typename TBase>
struct TriangleLeftLight : TBase {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, this->box.size.x, 0);
		nvgLineTo(args.vg, this->box.size.x, this->box.size.y);
		nvgLineTo(args.vg, 0, this->box.size.y / 2.f);
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
			nvgFill(args.vg);
		}

		// Foreground
		if (this->color.a > 0.0) {
			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}

		// Border
		if (this->borderColor.a > 0.0) {
			nvgStrokeWidth(args.vg, 0.5);
			nvgStrokeColor(args.vg, this->borderColor);
			nvgStroke(args.vg);
		}
	}
};

template <typename TBase>
struct TriangleRightLight : TBase {
	void drawLight(const widget::Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 0, 0);
		nvgLineTo(args.vg, 0, this->box.size.y);
		nvgLineTo(args.vg, this->box.size.x, this->box.size.y / 2.f);
		nvgClosePath(args.vg);

		// Background
		if (this->bgColor.a > 0.0) {
			nvgFillColor(args.vg, this->bgColor);
			nvgFill(args.vg);
		}

		// Foreground
		if (this->color.a > 0.0) {
			nvgFillColor(args.vg, this->color);
			nvgFill(args.vg);
		}

		// Border
		if (this->borderColor.a > 0.0) {
			nvgStrokeWidth(args.vg, 0.5);
			nvgStrokeColor(args.vg, this->borderColor);
			nvgStroke(args.vg);
		}
	}
};


struct StoermelderBlackScrew : app::SvgScrew {
	widget::TransformWidget* tw;

	StoermelderBlackScrew() {
		fb->removeChild(sw);

		tw = new TransformWidget();
		tw->addChild(sw);
		fb->addChild(tw);

		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Screw.svg")));

		tw->box.size = sw->box.size;
		box.size = tw->box.size;

		float angle = random::uniform() * M_PI;
		tw->identity();
		// Rotate SVG
		math::Vec center = sw->box.getCenter();
		tw->translate(center);
		tw->rotate(angle);
		tw->translate(center.neg());
	}
};

struct StoermelderTrimpot : app::SvgKnob {
	StoermelderTrimpot() {
		minAngle = -0.75 * M_PI;
		maxAngle = 0.75 * M_PI;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Trimpot.svg")));
		sw->setSize(Vec(16.6f, 16.6f));
	}
};

struct StoermelderSmallKnob : app::SvgKnob {
	StoermelderSmallKnob() {
		minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/SmallKnob.svg")));
		sw->setSize(Vec(22.7f, 22.7f));
	}
};

struct StoermelderPort : app::SvgPort {
	StoermelderPort() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Port.svg")));
		box.size = Vec(22.2f, 22.2f);
	}
};

template <typename TBase>
struct StoermelderPortLight : TBase {
	float size = 24.8f;

	StoermelderPortLight() {
		this->box.size = math::Vec(size, size);
	}

	void drawLight(const widget::Widget::DrawArgs& args) override {
		float radius = size / 2.0f;
		float radius2 = 22.2f / 2.0f;

		// Background
		if (TBase::bgColor.a > 0.0) {
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, radius, radius, radius);
			nvgFillColor(args.vg, TBase::bgColor);
			nvgFill(args.vg);
		}

		// Foreground
		if (TBase::color.a > 0.0) {
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, radius, radius, radius);
			nvgCircle(args.vg, radius, radius, radius2);
			nvgPathWinding(args.vg, NVG_HOLE);	// Mark second circle as a hole.
			nvgFillColor(args.vg, TBase::color);
			nvgFill(args.vg);
		}

		// Border
		if (TBase::borderColor.a > 0.0) {
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, radius, radius, radius);
			nvgStrokeWidth(args.vg, 0.5);
			nvgStrokeColor(args.vg, TBase::borderColor);
			nvgStroke(args.vg);
		}
	}

	void drawHalo(const widget::Widget::DrawArgs& args) override {
		float radius = size / 2.0f;
		float oradius = 2.5f * radius;

		nvgBeginPath(args.vg);
		nvgRect(args.vg, radius - oradius, radius - oradius, 2 * oradius, 2 * oradius);

		NVGpaint paint;
		NVGcolor icol = color::mult(TBase::color, 0.07);
		NVGcolor ocol = nvgRGB(0, 0, 0);
		paint = nvgRadialGradient(args.vg, radius, radius, radius, oradius, icol, ocol);
		nvgFillPaint(args.vg, paint);
		nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
		nvgFill(args.vg);
	}
};


template < typename LIGHT = BlueLight, int COLORS = 1>
struct PolyLedWidget : Widget {
	PolyLedWidget() {
		box.size = mm2px(Vec(6.f, 6.f));
	}

	void setModule(Module* module, int firstlightId) {
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(0, 0)), module, firstlightId + COLORS * 0));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(2, 0)), module, firstlightId + COLORS * 1));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(4, 0)), module, firstlightId + COLORS * 2));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(6, 0)), module, firstlightId + COLORS * 3));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(0, 2)), module, firstlightId + COLORS * 4));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(2, 2)), module, firstlightId + COLORS * 5));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(4, 2)), module, firstlightId + COLORS * 6));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(6, 2)), module, firstlightId + COLORS * 7));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(0, 4)), module, firstlightId + COLORS * 8));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(2, 4)), module, firstlightId + COLORS * 9));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(4, 4)), module, firstlightId + COLORS * 10));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(6, 4)), module, firstlightId + COLORS * 11));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(0, 6)), module, firstlightId + COLORS * 12));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(2, 6)), module, firstlightId + COLORS * 13));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(4, 6)), module, firstlightId + COLORS * 14));
		addChild(createLightCentered<TinyLight<LIGHT>>(mm2px(Vec(6, 6)), module, firstlightId + COLORS * 15));
	}
};


template < typename MODULE, int SCENE_COUNT >
struct SceneLedDisplay : LedDisplayChoice {
	MODULE* module;

	SceneLedDisplay() {
		color = nvgRGB(0xf0, 0xf0, 0xf0);
		box.size = Vec(16.9f, 16.f);
		textOffset = Vec(3.f, 11.5f);
	}

	void step() override {
		if (module) {
			text = string::f("%02d", module->sceneSelected + 1);
		} 
		else {
			text = "00";
		}
		LedDisplayChoice::step();
	}

	void onButton(const event::Button& e) override {
		if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
			createContextMenu();
			e.consume(this);
		}
		LedDisplayChoice::onButton(e);
	}

	void createContextMenu() {
		ui::Menu* menu = createMenu();

		struct SceneItem : MenuItem {
			MODULE* module;
			int scene;
			
			void onAction(const event::Action& e) override {
				module->sceneSet(scene);
			}

			void step() override {
				rightText = module->sceneSelected == scene ? "✔" : "";
				MenuItem::step();
			}
		};

		struct CopyMenuItem : MenuItem {
			MODULE* module;
			CopyMenuItem() {
				rightText = RIGHT_ARROW;
			}

			Menu* createChildMenu() override {
				Menu* menu = new Menu;

				struct CopyItem : MenuItem {
					MODULE* module;
					int scene;
					
					void onAction(const event::Action& e) override {
						module->sceneCopy(scene);
					}
				};

				for (int i = 0; i < SCENE_COUNT; i++) {
					menu->addChild(construct<CopyItem>(&MenuItem::text, string::f("%02u", i + 1), &CopyItem::module, module, &CopyItem::scene, i));
				}

				return menu;
			}
		};

		menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Scene"));
		for (int i = 0; i < SCENE_COUNT; i++) {
			menu->addChild(construct<SceneItem>(&MenuItem::text, string::f("%02u", i + 1), &SceneItem::module, module, &SceneItem::scene, i));
		}
		menu->addChild(new MenuSeparator());
		menu->addChild(construct<CopyMenuItem>(&MenuItem::text, "Copy to", &CopyMenuItem::module, module));
	}
};


template < typename BASE, typename MODULE >
struct MatrixButtonLight : BASE {
	MatrixButtonLight() {
		this->box.size = math::Vec(26.5f, 26.5f);
	}

	void drawLight(const Widget::DrawArgs& args) override {
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.8f, 0.8f, this->box.size.x - 2 * 0.8f, this->box.size.y - 2 * 0.8f, 3.4f);

		//nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
		nvgFillColor(args.vg, this->color);
		nvgFill(args.vg);
	}
};

struct MatrixButton : app::SvgSwitch {
	MatrixButton() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/MatrixButton.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/MatrixButton1.svg")));
		fb->removeChild(shadow);
		delete shadow;
	}
};