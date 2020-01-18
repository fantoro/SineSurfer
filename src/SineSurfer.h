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

#ifndef SINESURFER_H
#define SINESURFER_H

#include <Handler.h>
#include <ScreenSaver.h>
#include <ObjectList.h>
#include <Slider.h>
#include <config/types.h>

class SineSurfer : public BScreenSaver, public BHandler {
public:
						SineSurfer(BMessage* archive, image_id id);
						~SineSurfer();

	virtual void			StartConfig(BView* view);

	virtual status_t		StartSaver(BView* view, bool preview);
	virtual status_t		SaveState(BMessage* msg) const;
	virtual void			Draw(BView* view, int32 frame);

	virtual void			MessageReceived(BMessage* msg);

private:
	void				_Restart(BView* view);

	bool				fRestartNeeded;

	__haiku_std_uint8		rBackground;
	__haiku_std_uint8		gBackground;
	__haiku_std_uint8		bBackground;
	__haiku_std_uint8		aBackground;

	int				circleRadius;
	__haiku_std_uint8		circleRed;
	__haiku_std_uint8		circleGreen;
	__haiku_std_uint8		circleBlue;
	__haiku_std_uint8		circleAlpha;

	int		fWidth;
	int		fHeight;
	BPoint		fCenter;

	int		fFrequency;
	int		fAmplitude;
	BObjectList<int> fAddXAxis;
	int		*fRandomNumber;

	BSlider		*frequencySlider;
	BSlider		*redSlider;
	BSlider		*greenSlider;
	BSlider		*blueSlider;

	BRect		rect;
	BRegion		*screen;
};

#endif
