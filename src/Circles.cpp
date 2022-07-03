#include "plugin.hpp"
#include <iostream>
#include <iomanip>

struct Circles : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	dsp::Timer stepTimer;
	// dsp::SchmittTrigger sequenceTrigger;
	float currentVoltage=0;
	const float START_FREQ = 0; // Start at zero for base freq which is usually FREQ_C4 (0 volts)
	float curFreq = START_FREQ;
	int circleStep = 0;

	Circles() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configOutput(OUTPUT_OUTPUT, "");
		stepTimer.reset();
	}

	void process(const ProcessArgs& args) override 
	{
		if (args.frame == 0) {
			std::cerr << "Setting to initial frequency voltage of " << curFreq << "\n";
			outputs[0].setVoltage(curFreq);
		} else if (stepTimer.process(args.sampleTime) > 1) {
			stepTimer.reset();
			if (circleStep == 11) {
				circleStep = 0;
				curFreq = START_FREQ;
			} else {
				circleStep++;
				curFreq = START_FREQ + /* 1v * */ simd::pow(2.f, circleStep/12.f);
			}
			std::cerr << "Updating output to " << std::fixed << std::setw(4) << std::setprecision(4) << curFreq << "V \n";
			outputs[0].setVoltage(curFreq);
		}
	}
};


struct CirclesWidget : ModuleWidget {

	CirclesWidget(Circles* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Circles.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.486, 115.128)), module, Circles::OUTPUT_OUTPUT));
	}
};


Model* modelCircles = createModel<Circles, CirclesWidget>("CirclesOfHell");