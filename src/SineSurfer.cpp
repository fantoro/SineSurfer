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

extern "C" BScreenSaver*
instantiate_screen_saver(BMessage* msg, image_id id)
{
	return new SineSurfer(msg, id);
}

SineSurfer::SineSurfer(BMessage *msg, image_id image)
	: BScreenSaver(msg, image)
{
	rBackground = 0;
	gBackground = 0;
	bBackground = 0;
	aBackground = 255;

	circleColor = 255;
	circleAlpha = 255;
}

SineSurfer::~SineSurfer()
{

}

void SineSurfer::StartConfig(BView *view)
{
	BWindow* win = view->Window();

	if (win)
		win->AddHandler(this);

	BStringView* name = new BStringView("name", "Sine Surfer");
	BStringView* author = new BStringView("author", "fantoro");
	name->SetFont(be_bold_font);

	BLayoutBuilder::Group<>(view, B_VERTICAL, B_USE_ITEM_SPACING)
		.SetInsets(B_USE_WINDOW_INSETS)
		.SetExplicitAlignment(BAlignment(B_ALIGN_HORIZONTAL_CENTER, B_ALIGN_TOP))
		.AddGroup(B_HORIZONTAL)
			.Add(name)
			.Add(author)
			.AddGlue()
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

	view->SetLowColor(rBackground, gBackground, bBackground, aBackground);
	view->SetHighColor(circleColor, circleColor, circleColor, circleAlpha);

	if (frame == 1) {
		view->FillRect(view->Bounds(), B_SOLID_LOW);
		fCenter.Set(0-fHeight*0.01, fHeight*(1.0/16.0)+10.0);
	}

	if (fCenter.y > fHeight+fHeight*(1.0/16.0))
		return;

	BPoint sineWavePoint(fCenter);
	sineWavePoint.y += fHeight*(1.0/16.0)*sin(2*3.14*30*frame);

	fCenter.x++;

	if (fCenter.x > (fWidth + fHeight*0.01)) {
		fCenter.y += fHeight*(1.0/16.0);
		fCenter.x = 0-fHeight*0.01;
		circleColor -= 255/(fHeight/(fHeight*(1.0/16.0)));
	}

	view->StrokeEllipse(sineWavePoint, fHeight*0.01, fHeight*0.01);
}

void SineSurfer::_Restart(BView* view)
{
	fRestartNeeded = false;

	Draw(view, 0);
}

void SineSurfer::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	default:
		BHandler::MessageReceived(msg);	
	}
}

status_t SineSurfer::SaveState(BMessage *msg) const
{
	return B_OK;
}
