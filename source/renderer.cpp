GLOBAL Vec2 gRenderOffset = { 0,0 };

INTERNAL void ScissorOn (float x, float y, float w, float h)
{
    SDL_Rect region = { (int)x, (int)y, (int)w, (int)h };
    SDL_RenderSetClipRect(gWindow.renderer, &region);
}

INTERNAL void ScissorOn (Rect rect)
{
    SDL_Rect region = { (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h };
    SDL_RenderSetClipRect(gWindow.renderer, &region);
}

INTERNAL void ScissorOff ()
{
    SDL_RenderSetClipRect(gWindow.renderer, NULL);
}

INTERNAL void DrawPoint (float x, float y, Vec4 color)
{
    SDL_Color c = ColorToSDLColor(color);
    SDL_SetRenderDrawColor(gWindow.renderer, c.r,c.g,c.b,c.a);
    x -= gRenderOffset.x;
    y -= gRenderOffset.y;
    SDL_RenderDrawPointF(gWindow.renderer, x,y);
}

INTERNAL void DrawLine (float x1, float y1, float x2, float y2, Vec4 color)
{
    SDL_Color c = ColorToSDLColor(color);
    SDL_SetRenderDrawColor(gWindow.renderer, c.r,c.g,c.b,c.a);
    x1 -= gRenderOffset.x;
    y1 -= gRenderOffset.y;
    x2 -= gRenderOffset.x;
    y2 -= gRenderOffset.y;
    SDL_RenderDrawLineF(gWindow.renderer, x1,y1,x2,y2);
}

INTERNAL void DrawRect (float x, float y, float w, float h, Vec4 color)
{
    SDL_Color c = ColorToSDLColor(color);
    SDL_SetRenderDrawColor(gWindow.renderer, c.r,c.g,c.b,c.a);
    SDL_FRect rect = { x-gRenderOffset.x,y-gRenderOffset.y,w,h };
    SDL_RenderDrawRectF(gWindow.renderer, &rect);
}

INTERNAL void DrawFill (float x, float y, float w, float h, Vec4 color)
{
    SDL_Color c = ColorToSDLColor(color);
    SDL_SetRenderDrawColor(gWindow.renderer, c.r,c.g,c.b,c.a);
    SDL_FRect rect = { x-gRenderOffset.x,y-gRenderOffset.y,w,h };
    SDL_RenderFillRectF(gWindow.renderer, &rect);
}

INTERNAL void DrawRect (Rect rect, Vec4 color)
{
    DrawRect(rect.x, rect.y, rect.w, rect.h, color);
}

INTERNAL void DrawFill (Rect rect, Vec4 color)
{
    DrawFill(rect.x, rect.y, rect.w, rect.h, color);
}

INTERNAL void DrawImage (std::string image_name, float x, float y, Vec2 scale, Vec2 center, float angle, Flip flip, Vec4 color, const SDL_Rect* clip)
{
    Image* image = GetAsset<AssetImage>(image_name);
    if (!image) return;
    SDL_Color c = ColorToSDLColor(color);
    SDL_SetTextureColorMod(image->texture, c.r,c.g,c.b);
    SDL_SetTextureAlphaMod(image->texture, c.a);
    SDL_FRect rect = { x,y,image->w,image->h };
    if (clip) { rect.w = (float)clip->w, rect.h = (float)clip->h; }
    rect.x = roundf(rect.x) - gRenderOffset.x;
    rect.y = roundf(rect.y) - gRenderOffset.y;
    rect.w = roundf(rect.w) * scale.x;
    rect.h = roundf(rect.h) * scale.y;
    SDL_FPoint point = { center.x, center.y };
    SDL_RenderCopyExF(gWindow.renderer, image->texture, clip, &rect, angle, &point, flip);
}

INTERNAL void DrawText (std::string font_name, std::string text, float x, float y, Vec4 color, float scale)
{
    // NOTE: We just assume the caller wants multi-line text to be center aligned.

    Font* font = GetAsset<AssetFont>(font_name);

    float ix = x;
    float iy = y;

    int line = 0;

    for (int i=0; i<text.length(); ++i)
    {
        if (text[i] == '\n')
        {
            ix = x + (((GetTextLineWidth(font_name, text, 0)/2) - (GetTextLineWidth(font_name, text, line+1)/2)) * scale);
            iy += font->charh * scale;
            line++;
        }
        else
        {
            DrawImage(font->image, ix, iy, {scale,scale}, {0,0}, 0.0f, FLIP_NONE, color, &font->bounds[static_cast<U8>(text.at(i))]);
            ix += font->charw * scale;
        }
    }
}
