#include "messages.h"
#include "text.h"
#include "motionimpl.h"
#include "easing.h"
#include "widget.h"

using namespace std;

//TODO: copied from MoveAnimator...
// better than IMotionPtr...
class Mover : public Component {

	float destX, destY;
	float srcX, srcY;
	ComponentPtr target;
	int totalSteps;
	int currentStep;
	// EasingFuncPtr f;
	function<void(ComponentPtr &c)> onFinishedFunc;

public:
	Mover (const ComponentPtr &target, float destX, float destY, int steps) :
		destX(destX), destY(destY), target(target), totalSteps(steps), currentStep(0) {
		srcX = target->getx();
		srcY = target->gety();
		setVisible(false);
	}

	virtual void update() {
		if (!target) return;

		// interpolate...
		double delta = (double)currentStep / (double)totalSteps;

		//TODO
		// apply easing func
		// double ease = f(delta);
		double ease = delta;
		double newx = srcX + ease * (destX - srcX);
		double newy = srcY + ease * (destY - srcY);
		target->setxy(newx, newy);
		// target->repr(0, cout);

		currentStep++;

		if (currentStep > totalSteps) {
			target->handleMessage(shared_from_this(), MSG_ANIMATION_DONE);
			onFinishedFunc(target);
			kill();
			target = nullptr;
		}
	}

	virtual std::string const className() const override { return "Mover"; }

	void onFinished(function<void(ComponentPtr &)> f) {
		onFinishedFunc = f;
	}
};

class RainbowGenerator2 : public IColorGenerator
{
private:
	int span;
public:
	RainbowGenerator2(int span);
	virtual ALLEGRO_COLOR getColor(int msec, int index) override;
};

RainbowGenerator2::RainbowGenerator2(int span) : span(span) {};

ALLEGRO_COLOR RainbowGenerator2::getColor(int msec, int index)
{
	return getRainbowColor(index, span);
}

void Messages::showMessage(std::string text, Behavior behavior) {

	// auto t = Text::buildf(WHITE, "TEST MESSAGE").center().get();
	// add (t);
	// cout << text << endl;
	shared_ptr<Mover> motion;
	shared_ptr<Text> t;

	const int HEIGHT = 120;
	if (behavior == POP_UP) {
		t = Text::build(WHITE, ALLEGRO_ALIGN_CENTER, text).xywh(0, 480 + HEIGHT, 640, HEIGHT).get();
		motion = make_shared<Mover>(t, 0, -HEIGHT, 100);
	}
	else if (behavior == RIGHT_TO_LEFT) {
		int yco = (480 - HEIGHT) / 2;
		t = Text::build(WHITE, ALLEGRO_ALIGN_CENTER, text).xywh(640, yco, 640, HEIGHT).get();
		t->setLetterColorGenerator(make_shared<RainbowGenerator2>(12));
		motion = make_shared<Mover>(t, -640, yco, 160);
	}

	add(t);
	motion->onFinished(
		[=](ComponentPtr &t) {
			t->kill();
		}
	);
	add(motion);

};
