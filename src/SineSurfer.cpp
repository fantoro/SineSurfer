/* 
 * The MIT License (MIT)
 * 
 * Copyright 2020, fantoro
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "SineSurfer.h"

#include <math.h>
#include <stdlib.h>
#include <cstdio>

#include <Alignment.h>
#include <Button.h>
#include <LayoutBuilder.h>
#include <Slider.h>
#include <String.h>
#include <StringView.h>
#include <Window.h>
#include <View.h>


static const BString kName = "Sine Surfer";
static const BString kAuthor = "fantoro";

static const int32 K_SET_FREQUENCY = 'sfrq';
//static const int32 K_SET_AMPLITUDE = 'samp';
static const int32 K_SET_RED = 'sred';
static const int32 K_SET_GREEN = 'sgrn';
static const int32 K_SET_BLUE = 'sblu';

extern "C" BScreenSaver*
instantiate_screen_saver(BMessage* msg, image_id id)
{
	return new SineSurfer(msg, id);
}

SineSurfer::SineSurfer(BMessage *msg, image_id image)
	: BScreenSaver(msg, image), fAddXAxis(20, true), fRandomNumber(NULL)
{
	rBackground = 0;
	gBackground = 0;
	bBackground = 0;
	aBackground = 255;

	circleRed = 255;
	circleGreen = 255;
	circleBlue = 255;
	circleAlpha = 255;

	circleRed = msg->GetInt8("red", circleRed);
	circleGreen = msg->GetInt8("green", circleGreen);
	circleBlue = msg->GetInt8("blue", circleBlue);

	fFrequency = 30;
	fFrequency = msg->GetInt32("frequency", fFrequency);

	srandom(time(NULL));
}

SineSurfer::~SineSurfer()
{
	fAddXAxis.MakeEmpty();
}

void SineSurfer::StartConfig(BView *view)
{
	BString label;

	BWindow* win = view->Window();

	if (win)
		win->AddHandler(this);

	BStringView* name = new BStringView("name", "Sine Surfer");
	BStringView* author = new BStringView("author", "fantoro");
	name->SetFont(be_bold_font);

	BStringView* sineSettingsLabel = new BStringView("sineSettings", "Sine wave properties");
	sineSettingsLabel->SetFont(be_bold_font);

	frequencySlider = new BSlider("frequency", "Frequency", new BMessage(
		K_SET_FREQUENCY), 1, 60, B_HORIZONTAL);
	frequencySlider->SetValue(fFrequency);
	frequencySlider->SetTarget(this);
	frequencySlider->SetLimitLabels("1", "60");
	frequencySlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	frequencySlider->SetHashMarkCount(15);

	label.SetToFormat("Frequency: %d", fFrequency);

	frequencySlider->SetLabel(label);

	BStringView* colorSettingsLabel = new BStringView("colorSettings", "Sine wave color");
	colorSettingsLabel->SetFont(be_bold_font);

	redSlider = new BSlider("red", "Red", new BMessage(
		K_SET_RED), 0, 255, B_HORIZONTAL);
	redSlider->SetValue(circleRed);
	redSlider->SetTarget(this);
	redSlider->SetLimitLabels("0", "255");
	redSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	redSlider->SetHashMarkCount(17);

	label.SetToFormat("Red: %d", circleRed);

	redSlider->SetLabel(label);

	greenSlider = new BSlider("green", "Green", new BMessage(
		K_SET_GREEN), 0, 255, B_HORIZONTAL);
	greenSlider->SetValue(circleGreen);
	greenSlider->SetTarget(this);
	greenSlider->SetLimitLabels("0", "255");
	greenSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	greenSlider->SetHashMarkCount(17);

	label.SetToFormat("Green: %d", circleGreen);

	greenSlider->SetLabel(label);

	blueSlider = new BSlider("blue", "Blue", new BMessage(
		K_SET_BLUE), 0, 255, B_HORIZONTAL);
	blueSlider->SetValue(circleBlue);
	blueSlider->SetTarget(this);
	blueSlider->SetLimitLabels("0", "255");
	blueSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	blueSlider->SetHashMarkCount(17);

	label.SetToFormat("Blue: %d", circleBlue);

	blueSlider->SetLabel(label);

	BLayoutBuilder::Group<>(view, B_VERTICAL, B_USE_ITEM_SPACING)
		.SetInsets(B_USE_WINDOW_INSETS)
		.SetExplicitAlignment(BAlignment(B_ALIGN_HORIZONTAL_CENTER, B_ALIGN_TOP))
		.AddGroup(B_HORIZONTAL)
			.Add(name)
			.Add(author)
			.AddGlue()
		.End()
		.AddGlue()
		.AddGroup(B_VERTICAL)
			.Add(sineSettingsLabel)
			.Add(frequencySlider)
			.AddGlue()
		.End()
		.AddGlue()
		.AddGroup(B_VERTICAL)
			.Add(colorSettingsLabel)
			.Add(redSlider)
			.Add(greenSlider)
			.Add(blueSlider)
		.End();
}

status_t SineSurfer::StartSaver(BView* view, bool rev)
{
	_Restart(view);

	return B_OK;
}

void SineSurfer::Draw(BView* view, int32 frame)
{

	if (fRestartNeeded)
		_Restart(view);

	rect = view->Bounds();
	fWidth = (int) rect.Width() + 1;
	fHeight = (int) rect.Height() + 1;

	fAmplitude = fHeight*(1.0/16.0);
	circleRadius = fHeight*0.01;

	view->SetLowColor(rBackground, gBackground, bBackground, aBackground);

	if (frame == 1 ||
		fCenter.x > (fWidth + circleRadius + circleRadius*(fHeight/fAmplitude))) {
		//fAddXAxis = (random() % 50) - 25;

		view->FillRect(view->Bounds(), B_SOLID_LOW);
		fCenter.Set(0-circleRadius, fAmplitude+10.0);

		fAddXAxis.MakeEmpty();

		for (int i = 0; i < fHeight/fAmplitude; i++) {
			fRandomNumber = new int(random() % (7 + 1 + 7) - 7);
			fAddXAxis.AddItem(fRandomNumber, i);
		}
	}

	for (int i = 0; i < fHeight/fAmplitude; i++) {
		BPoint sineWavePoint(fCenter);
		sineWavePoint.x -= circleRadius*i;
		sineWavePoint.y += fAmplitude*i;
		if (fRandomNumber != NULL)
			sineWavePoint.y += fAmplitude*sin(2 * 3.14 * fFrequency * (frame + *(fAddXAxis.ItemAt(i))));

		int waveRed = circleRed-(i*(circleRed/(fHeight/fAmplitude)));
		int waveGreen = circleGreen-(i*(circleGreen/(fHeight/fAmplitude)));
		int waveBlue = circleBlue-(i*(circleBlue/(fHeight/fAmplitude)));
		view->SetHighColor(waveRed, waveGreen, waveBlue, circleAlpha);

		view->StrokeEllipse(sineWavePoint, circleRadius, circleRadius);
	}

	fCenter.x++;
}

void SineSurfer::_Restart(BView* view)
{
	fRestartNeeded = false;

	Draw(view, 0);
}

void SineSurfer::MessageReceived(BMessage* msg)
{
	BString label;
	switch (msg->what) {
		case K_SET_FREQUENCY: {
			fFrequency = msg->GetInt32("be:value", fFrequency);
			label.SetToFormat("Frequency: %d", fFrequency);
			frequencySlider->SetLabel(label);
			fRestartNeeded = true;
			break;
		}

		case K_SET_RED: {
			circleRed = (__haiku_std_uint8)(msg->GetInt32("be:value", circleRed));
			label.SetToFormat("Red: %d", circleRed);
			redSlider->SetLabel(label);
			fRestartNeeded = true;
			break;
		}
		case K_SET_GREEN: {
			circleGreen = (__haiku_std_uint8)(msg->GetInt32("be:value", circleGreen));
			label.SetToFormat("Green: %d", circleGreen);
			greenSlider->SetLabel(label);
			fRestartNeeded = true;
			break;
		}
		case K_SET_BLUE: {
			circleBlue = (__haiku_std_uint8)(msg->GetInt32("be:value", circleBlue));
			label.SetToFormat("Blue: %d", circleBlue);
			blueSlider->SetLabel(label);
			fRestartNeeded = true;
			break;
		}
		default:
			BHandler::MessageReceived(msg);
	}
}

status_t SineSurfer::SaveState(BMessage *msg) const
{
	msg->AddInt32("frequency", fFrequency);
	msg->AddInt8("red", circleRed);
	msg->AddInt8("green", circleGreen);
	msg->AddInt8("blue", circleBlue);
	return B_OK;
}
