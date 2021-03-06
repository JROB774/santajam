#pragma once

INTERNAL void FadeFrom (Vec4 color, float speed);
INTERNAL void FadeTo   (Vec4 color, float speed);

INTERNAL bool IsFadeToDone ();

INTERNAL void   InitFade ();
INTERNAL void RenderFade ();
